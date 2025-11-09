# **Video Streaming Experiments**

This repository is a collection of small, focused experiments exploring how video data moves — over different protocols, under different constraints, and with varying approaches to metadata and reliability.  

Each experiment lives in its own directory and focuses on a single aspect of video transmission or signaling. The work is intended for low-level systems developers, embedded engineers, and anyone working with streaming pipelines that need to survive the real world.

---

## **Experiments**

### [`RTP-RFC-8285`](./RTP-RFC-8285)
Implements per-packet stream identification using **RTP header extensions**, as described in [RFC 8285](https://www.rfc-editor.org/rfc/rfc8285).  
It shows how to embed a unique stream ID (or other metadata) directly into each RTP packet using GStreamer — a minimal but effective method to make media streams self-identifying at the packet level.

📖 Related article:  
[Identifying Video Streams Using RTP Header Extensions](https://sysdev.me/2025/11/09/identifying-video-streams-using-rtp-header-extensions/)

