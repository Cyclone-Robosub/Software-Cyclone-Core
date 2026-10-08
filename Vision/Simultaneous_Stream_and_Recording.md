# Raspberry Pi Camera: Simultaneous Stream & Record Guide

## 1. The Strategy: "Zero-CPU Passthrough"
Since the Raspberry Pi has limited CPU power and writing to SD cards is slow, we avoid re-encoding video at all costs. 

* **Source:** We use the camera's internal **H.264 hardware encoder** (usually `/dev/video2`).
* **Process:** We use FFmpeg to "copy" the packets. The Pi acts only as a courier, moving data from USB to Ethernet and Disk without modifying it.
* **Result:** * **CPU Load:** Negligible (<5%).
    * **Latency:** Low (<500ms).
    * **Storage:** Efficient (~1GB/hour).

## 2. Architecture
```mermaid
graph LR
    Cam[Camera Hardware H.264] -->|USB| FFmpeg
    FFmpeg -->|Copy Stream| Disk[recordings/output.mp4]
    FFmpeg -->|Copy Stream| Server[MediaMTX Localhost]
    Server -->|RTSP/WebRTC| Browser[Web Dashboard]
```

### A. Install MediaMTX (The Streaming Server)

This lightweight server bridges the raw RTSP stream to a web-friendly format (WebRTC).

**Download & Run (on the Pi):**
```bash
# Download (Check generic arm64)
wget https://github.com/bluenviron/mediamtx/releases/download/v1.15.6/mediamtx_v1.15.6_linux_arm64.tar.gz
# Extract
tar -xvzf mediamtx_v1.6.0_linux_arm64.tar.gz

# Run in background
./mediamtx &

```

### B. Prepare Directories

Ensure the recording folder exists to prevent FFmpeg errors.

```bash
mkdir -p ~/recordings
```
---

## 4. The Run Command

Run this command to start the pipeline.

**Key Flags Explained:**

* `-fflags +genpts`: Generates new timestamps to fix missing camera clock data.
* `-c:v copy`: **Critical.** Copies video data directly without re-encoding (saves CPU).
* `-avoid_negative_ts make_zero`: Ensures the recording file starts at 00:00:00.

```bash
ffmpeg -f v4l2 -input_format h264 -video_size 1920x1080 -framerate 30 \
-fflags +genpts \
-i /dev/video2 \
-c:v copy -f rtsp rtsp://localhost:8554/cam \
-c:v copy -avoid_negative_ts make_zero -f mp4 ~/recordings/output_$(date +%Y%m%d_%H%M%S).mp4
```

*(Note: The command above adds a timestamp to the filename so you don't overwrite old recordings).*

---

## 5. Viewing the Output

### A. The Web Stream

Access the stream from any computer on the same network. MediaMTX provides a built-in viewer.

* **URL:** `http://<IP-OF-RASPBERRY-PI>:8889/cam`

---

## 6. Troubleshooting

* **Warnings (`Non-monotonic DTS` / `Timestamps unset`):**
* These are common with USB H.264 cameras.
* **Verdict:** Safe to ignore as long as `speed=1.0x` and the resulting MP4 file plays back correctly.
