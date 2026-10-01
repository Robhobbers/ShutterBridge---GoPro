# Validation record: 0.1.1-rob.1

Base reviewed: `bc082abf9cfda2bae195372bf09e516d6bdc792b` (2026-10-01).
Changes are listed in `CHANGELOG.md`; release mechanics are in `USB_RELEASES.md`.

## Reproduced before changes

The original `ChannelActions.cpp` was compiled with a mock camera and clock:

- Arm at 100 ms, disarm at 200 ms, re-arm at 1000 ms, then update at 2200 ms:
  the pending stop still fired (two start calls, one stop call).
- Start requested while disconnected, followed by connection while still armed:
  no retry occurred (only the original unsuccessful start call).
- Source inspection showed pairing's save promise was not awaited and errors were
  suppressed before a separate three-second reboot countdown.

## Automated regression checks

Run from the repository root:

```sh
bash tests/run.sh
node --check firmware/data/app.js
git diff --check
```

The tests compile the production recording controller and settings implementation;
JavaScript tests execute the save/pairing functions extracted from the production UI.
They cover:

- Re-arm before, exactly at, and after a stop deadline before timer processing.
- Disarm cancelling delayed start; start/stop delay preservation; millisecond wraparound.
- Late connection, reconnect, rejected start retry throttling and disarm cancellation.
- No duplicate starts with recording telemetry; stale/not-ready/photo start guards.
- Manual two-position control, momentary cancellation and photo triggering.
- NVS open/write/read-back failures and a successful settings round trip.
- Delayed save acknowledgement, duplicate pairing clicks, failed saves, lost-response
  timeout, unexpected responses and device-owned versus explicit reboot.
- Release metadata identity, offsets, checksums and rejection of missing images or
  mismatched release versions.

All of the above host checks passed during implementation.

## Build verification

Passed locally:

- `pio run -e esp32s3-zero`: full firmware build, 40.0% of application flash and
  17.3% of internal RAM according to PlatformIO's size check.
- `pio run -e esp32s3-zero -t buildfs`: LittleFS web UI image.
- Frozen-lockfile site dependency install and `pnpm build`: all site routes built.
- USB image merge using `python -m esptool` 4.12.0, release metadata generation,
  and SHA-256 verification of every packaged image/document.
- Workflow YAML parsing, JavaScript syntax check and whitespace/diff checks.

Environment: Python 3.12, PlatformIO 6.2.0, pioarduino 55.3.312,
Arduino ESP32 3.3.12, NimBLE 2.5.1, ArduinoJson 7.4.3. The repository's existing
floating platform/dependency constraints were retained. Initial system Python 3.9
was too old for the board toolchain; validation used Python 3.12. Astro telemetry
was disabled locally to avoid writing outside the workspace. The site build emits
an existing markdown-plugin deprecation warning.

CI repeats builds and host tests on the pull request. These are software build
results, not physical hardware qualification.

## Limits and physical validation

No camera, flight controller or ESP32 was connected during these checks. Before
publishing as a stable release, record camera model/firmware, board and FC version,
and validate with propellers removed:

1. Arm/disarm/re-arm within the stop delay: the same recording continues.
2. Arm before camera connection; connect it later: recording starts when ready.
3. Interrupt/recover BLE while armed; verify eventual recording without request flooding.
4. Disarm before a delayed start; verify no later unintended start.
5. Pair a different camera, wait for restart and power-cycle: selection persists.
6. Disconnect the browser during a successful save: device reboots and keeps settings.
7. Flash matching USB firmware/filesystem images without erasing: settings survive.
8. Confirm fork, version and revision in the startup log and configuration API.

The controller relies on the FC's existing arm indication and the backend's
recording telemetry. This patch does not add FC stale-arm/failsafe policy, GoPro
mode readout, or automatic retries/confirmation for failed stop commands. A manual
stop on the camera while record-on-arm demand remains active can be followed by an
automatic restart after the retry interval; use Manual mode for independent takes.
