# Simple Makefile for RTP header extension injector
# Works with GStreamer ≥1.18 (tested on Ubuntu 24.04)

APP       := rtp_ext_inject
SRC       := rtp_ext_inject.c

# Get compiler flags and libs from pkg-config
PKGS      := gstreamer-1.0 gstreamer-rtp-1.0 gstreamer-video-1.0
CFLAGS    := -Wall -Wextra -O2 `pkg-config --cflags $(PKGS)`
LIBS      := `pkg-config --libs $(PKGS)`

# Default build rule
all: $(APP)

$(APP): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

# Install binary (optional)
install: $(APP)
	sudo install -m 755 $(APP) /usr/local/bin/$(APP)

# Clean up build artifacts
clean:
	rm -f $(APP)

# Print compiler flags for debugging
info:
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LIBS:   $(LIBS)"

.PHONY: all clean install info
