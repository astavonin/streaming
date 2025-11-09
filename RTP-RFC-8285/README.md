# **RTP Header Extensions (RFC 8285)**

This experiment demonstrates how to embed per-stream metadata directly into RTP packets using **RTP header extensions** — a feature defined in [RFC 8285](https://www.rfc-editor.org/rfc/rfc8285).  

The idea is simple: each packet carries a small identifier in its header, allowing a receiver or backend system to know *which stream it belongs to* without depending on connection state, SSRC mapping, or control channels.

For a full explanation and background, read the accompanying article:  
📖 [Identifying Video Streams Using RTP Header Extensions](https://sysdev.me/2025/11/09/identifying-video-streams-using-rtp-header-extensions/)

---

## **Structure**

```
RTP-RFC-8285/
├── Makefile          # Simple GStreamer build for sender/receiver
├── rtp_ext_inject.c  # Sender: GStreamer pad probe injecting stream ID into RTP packets
└── rtp_ext_reader.c  # Receiver: Extracts and displays RFC-8285 header extensions
```

---

## **Build**

Install dependencies:

```bash
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-good
```

Then build:

```bash
make
```

This produces two binaries: `rtp_ext_inject` (sender) and `rtp_ext_reader` (receiver).

---

## **Run**

### Option 1: Using the Custom Receiver (Recommended)

Start the receiver in one terminal:

```bash
./rtp_ext_reader
```

Then launch the sender in another terminal:

```bash
./rtp_ext_inject
```

You should see:
- A test pattern video window displaying a moving ball
- Console output showing extracted RFC-8285 header extensions:
  ```
  ✓ RTP Extension found (ID=1, size=4): 12 34 56 78
    → Decoded Stream ID: 0x12345678
  ```

### Option 2: Using gst-launch (Basic)

Start a basic receiver in one terminal:

```bash
gst-launch-1.0 udpsrc port=5000 caps="application/x-rtp,media=video,encoding-name=H264,payload=96,clock-rate=90000" ! rtpjitterbuffer ! rtph264depay ! avdec_h264 ! autovideosink sync=false
```

Then launch the sender:

```bash
./rtp_ext_inject
```

You should see the test pattern. To verify the header extensions, inspect the traffic in Wireshark (filter: `udp.port==5000`) — you'll see a **Header Extension** field with stream ID `12 34 56 78`.

