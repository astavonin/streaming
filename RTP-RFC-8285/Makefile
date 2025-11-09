# Simple Makefile for RTP header extension sender/receiver
# Works with GStreamer ≥1.18 (tested on Ubuntu 24.04)

SENDER    := rtp_ext_inject
RECEIVER  := rtp_ext_reader
APPS      := $(SENDER) $(RECEIVER)

# Get compiler flags and libs from pkg-config
PKGS      := gstreamer-1.0 gstreamer-rtp-1.0 gstreamer-video-1.0
CFLAGS    := -Wall -Wextra -O2 `pkg-config --cflags $(PKGS)`
LIBS      := `pkg-config --libs $(PKGS)`

# Default build rule - build both
all: $(APPS)

$(SENDER): rtp_ext_inject.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

$(RECEIVER): rtp_ext_reader.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

# Install binaries (optional)
install: $(APPS)
	sudo install -m 755 $(SENDER) /usr/local/bin/$(SENDER)
	sudo install -m 755 $(RECEIVER) /usr/local/bin/$(RECEIVER)

# Clean up build artifacts
clean:
	rm -f $(APPS)

# Print compiler flags for debugging
info:
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LIBS:   $(LIBS)"

.PHONY: all clean install info
