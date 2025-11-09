// rtp_ext_inject.c
#include <gst/gst.h>
#include <gst/rtp/gstrtpbuffer.h>   // <-- use the buffer API

static GstPadProbeReturn add_ext_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    (void)pad;        // Unused parameter
    (void)user_data;  // Unused parameter

    GstBuffer *buf = GST_PAD_PROBE_INFO_BUFFER(info);
    if (!buf) return GST_PAD_PROBE_OK;

    // Make writable in case upstream shares the buffer
    GstBuffer *w = gst_buffer_make_writable(buf);

    GstRTPBuffer rtp = GST_RTP_BUFFER_INIT;
    if (gst_rtp_buffer_map(w, GST_MAP_WRITE, &rtp)) {
        // Example 4-byte Terminal ID payload
        const guint8 data[4] = { 0x12, 0x34, 0x56, 0x78 };
        // ID=1 (maps to extmap-1=urn:example:stream-id)
        if (!gst_rtp_buffer_add_extension_onebyte_header(&rtp, 1, data, sizeof(data))) {
            g_printerr("Failed to add RTP one-byte header extension\n");
        }
        gst_rtp_buffer_unmap(&rtp);
    }

    // hand the (possibly new) buffer back to the pipeline
    GST_PAD_PROBE_INFO_DATA(info) = w;
    return GST_PAD_PROBE_OK;
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    GstElement *pipeline = gst_parse_launch(
        "videotestsrc is-live=true pattern=ball ! videoconvert ! "
        "x264enc tune=zerolatency bitrate=800 speed-preset=superfast ! "
        "rtph264pay pt=96 config-interval=1 name=pay ! "
        // extmap advertises mapping (optional but nice)
        "application/x-rtp,extmap-1=urn:example:stream-id ! "
        "udpsink host=127.0.0.1 port=5000", NULL);

    // Add the probe on the payloader's src pad
    GstElement *pay = gst_bin_get_by_name(GST_BIN(pipeline), "pay");
    GstPad *srcpad = gst_element_get_static_pad(pay, "src");
    gst_pad_add_probe(srcpad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback)add_ext_probe, NULL, NULL);
    gst_object_unref(srcpad);
    gst_object_unref(pay);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_print("Streaming with RTP header extension (ID=1)…\n");
    g_print("Verify in Wireshark: udp.port==5000 → RTP → Header Extension.\n");

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);
    return 0;
}

