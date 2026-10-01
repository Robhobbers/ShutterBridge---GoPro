# Robhobbers USB firmware releases

This fork targets **Waveshare ESP32-S3-Zero / ESP32-S3FH4R2, 4 MB flash**.
Do not assume a differently named ESP32-S3 Mini has the same pins and flash layout.
No Wi-Fi firmware updater is required.

## Identify the correct build

Download from https://github.com/Robhobbers/ShutterBridge---GoPro/releases.
The upstream website's installer installs upstream firmware, not this fork.

Each release contains:

- `ShutterBridge-GoPro-Robhobbers-<version>-merged.bin`: complete USB image.
- Five separate `.bin` parts for the fork's browser flasher.
- `release.json`: full source commit, version, target board, offsets and hashes.
- `SHA256SUMS`: checksums for images, metadata and these instructions.

The first version with these fixes is `0.1.1-rob.1`. It is a prerelease until
physical camera/flight validation is recorded. Draft releases are visible to
maintainers but are not offered by the public browser installer.

Firmware identifies itself in the USB serial startup log and `/config.json`
with the fork, version and build revision. The web UI version badge includes the
fork/revision in its tooltip. Local modified builds append `-dirty` to the revision.

## Flash over USB-C

1. Remove propellers and connect the board with a data-capable USB-C cable.
2. Download the assets from one release; do not mix firmware and web UI versions.
3. Check downloaded files with `shasum -a 256 -c SHA256SUMS` (macOS) or
   `sha256sum -c SHA256SUMS` (Linux). Download every listed file for this check.
4. Install esptool in a Python virtual environment: `python -m pip install 'esptool>=4.8,<5'`.
5. Replace `PORT` and the version in this command with your port and downloaded filename:

```sh
python -m esptool --chip esp32s3 --port PORT write_flash \
  --flash_mode dio --flash_freq 80m --flash_size 4MB \
  0x0 ShutterBridge-GoPro-Robhobbers-0.1.1-rob.1-merged.bin
```

**The complete merged image is a clean-install/recovery image. Back up your settings
manually first: writing its padded gaps can overwrite NVS configuration and pairing.**
For an update that retains the current partition layout and NVS, use the matching
separate firmware/web images instead, without `erase_flash`:

```sh
python -m esptool --chip esp32s3 --port PORT write_flash \
  0x10000 firmware.bin 0x310000 littlefs.bin
```

This release does not change the settings schema. Future incompatible schemas may
reset settings even when NVS is retained. Verify pairing, channels and OSD after
any update, and confirm recording with propellers removed before flying.

If USB is not detected, use the board's documented BOOT/reset download procedure
and retry with a known data cable. Keep USB accessible in the mount for recovery.

## Build from source

Use Python 3.10+ and PlatformIO. From `firmware/`:

```sh
pio run -e esp32s3-zero -t upload
pio run -e esp32s3-zero -t uploadfs
```

Both firmware and filesystem must come from the same revision. No erase step is
required for this release's unchanged partition layout.

## Maintainer release procedure

1. Update `FW_VERSION` in `firmware/src/config.h` and the changelog.
2. Run `bash tests/run.sh`, build firmware and LittleFS, and record validation.
3. Commit the changes. Create and push a tag matching the version, such as
   `v0.1.1-rob.1`. The tag-triggered workflow runs the regressions again.
4. The workflow builds USB images and creates a **draft** release with explicit
   fork/board identity, metadata and checksums. Check its artifacts and validation
   notes before publishing. Tags containing a hyphen are marked prereleases.
5. The site reads releases from this fork. Hosting a fork-specific site remains
   a separate step; changing repository configuration does not deploy a website.
