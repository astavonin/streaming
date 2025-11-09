// rtp_ext_reader.c
// Server-side RTP receiver that extracts RFC-8285 header extensions
#include <gst/gst.h>
#include <gst/rtp/gstrtpbuffer.h>

static GstPadProbeReturn extract_ext_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    (void)pad;        // Unused parameter
    (void)user_data;  // Unused parameter

    GstBuffer *buf = GST_PAD_PROBE_INFO_BUFFER(info);
    if (!buf) return GST_PAD_PROBE_OK;

    GstRTPBuffer rtp = GST_RTP_BUFFER_INIT;
    if (gst_rtp_buffer_map(buf, GST_MAP_READ, &rtp)) {
        // Check if the buffer has extensions
        if (gst_rtp_buffer_get_extension(&rtp)) {
            guint8 *data = NULL;
            guint size = 0;

            // Try to read extension with ID=1 (matching sender's extmap-1)
            if (gst_rtp_buffer_get_extension_onebyte_header(&rtp, 1, 0, (gpointer*)&data, &size)) {
                g_print("✓ RTP Extension found (ID=1, size=%u): ", size);
                for (guint i = 0; i < size; i++) {
                    g_print("%02X ", data[i]);
                }
                g_print("\n");

                // Interpret as custom stream ID
                if (size == 4) {
                    guint32 stream_id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
                    g_print("  → Decoded Stream ID: 0x%08X\n", stream_id);
                }
            } else {
                g_print("RTP packet has extensions but ID=1 not found\n");
            }
        }
        gst_rtp_buffer_unmap(&rtp);
    }

    return GST_PAD_PROBE_OK;
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    (void)bus;  // Unused parameter
    GMainLoop *loop = (GMainLoop *)data;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR: {
            gchar *debug;
            GError *error;
            gst_message_parse_error(msg, &error, &debug);
            g_printerr("Error: %s\n", error->message);
            g_error_free(error);
            g_free(debug);
            g_main_loop_quit(loop);
            break;
        }
        default:
            break;
    }
    return TRUE;
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    // Build the receiver pipeline
    GstElement *pipeline = gst_parse_launch(
        "udpsrc port=5000 caps=\"application/x-rtp,media=video,encoding-name=H264,payload=96,clock-rate=90000\" ! "
        "rtpjitterbuffer name=jitter ! "
        "rtph264depay name=depay ! "
        "avdec_h264 ! "
        "videoconvert ! "
        "autovideosink sync=false", NULL);

    if (!pipeline) {
        g_printerr("Failed to create pipeline\n");
        return -1;
    }

    // Add probe on jitterbuffer's sink pad to inspect incoming RTP packets
    GstElement *jitter = gst_bin_get_by_name(GST_BIN(pipeline), "jitter");
    if (jitter) {
        GstPad *sinkpad = gst_element_get_static_pad(jitter, "sink");
        gst_pad_add_probe(sinkpad, GST_PAD_PROBE_TYPE_BUFFER,
                         (GstPadProbeCallback)extract_ext_probe, NULL, NULL);
        gst_object_unref(sinkpad);
        gst_object_unref(jitter);
    }

    // Set up message bus
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    gst_bus_add_watch(bus, bus_call, loop);
    gst_object_unref(bus);

    // Start playing
    g_print("========================================\n");
    g_print("RTP RFC-8285 Extension Reader\n");
    g_print("========================================\n");
    g_print("Listening on UDP port 5000...\n");
    g_print("Waiting for RTP stream with header extensions...\n\n");

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
