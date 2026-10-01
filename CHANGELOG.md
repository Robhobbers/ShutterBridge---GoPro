# Changelog

Changes specific to Robhobbers/ShutterBridge---GoPro are recorded here.
Upstream attribution and the GPL-3.0-or-later license remain unchanged.

## 0.1.1-rob.1 — 2026-10-01 (prerelease)

### Recording fixes

- New input demands replace pending commands even when their delay is zero.
- Process arm/AUX transitions before expired timers, preventing an old stop from
  firing when re-arming at or after its deadline before the next control update.
- Retain record-on-arm demand when the camera connects late, including starting
  the bridge while the FC already reports armed.
- Reconcile missing recording confirmation while armed, with start attempts at
  least 20 seconds apart during the same arm cycle. A disarm cancels start demand.
- Wait for connected, fresh, ready telemetry before automatic starts; do not send
  video starts in reported photo mode or repeat starts while recording/starting.
- A second momentary shutter press reverses a pending delayed command.
- Preserve configured start/stop delays, manual shutter/photo controls and the
  existing GoPro connection timestamp fix.

### Pairing and settings

- Pairing waits for the save response instead of rebooting after an independent
  three-second countdown. Duplicate clicks cannot initiate concurrent saves.
- NVS saves report failure unless the write size and read-back match. On failure
  the save endpoint restores the prior in-memory settings and does not reboot.
- Device-owned reboot occurs after a verified change to camera/Wi-Fi boot settings;
  browser disconnection after saving cannot prevent the reboot.
- Save requests time out after ten seconds, surface unconfirmed saves, and never
  send a second reboot after a lost acknowledgement. Explicit reboot remains available.
- The pairing screen says it is saving until persistence has been confirmed.

### USB release identity

- Use version `0.1.1-rob.1`, fork identity and generated source revision in the
  firmware startup log/config API; expose build details on the version badge.
- Point the website's repository/release selection and README download links to
  this fork; retain upstream author attribution and wiring documentation links.
- Draft release titles and merged filenames identify Robhobbers, GoPro and the
  ESP32-S3-Zero USB target. Hyphenated versions are marked prereleases.
- Attach source/board/offset metadata, SHA-256 checksums and USB flashing instructions.
- Run regressions before building releases; validate tag/version agreement.
- Constrain packaging esptool to its supported 4.x command syntax and invoke it
  via `python -m esptool` so packaging does not depend on a console-script alias.
- Document clean-install merged images versus firmware/filesystem-only updates
  that preserve NVS with this unchanged settings schema and partition layout.

### Verification and repository records

- Add host C++ recording and persistent-settings regression checks, Node tests of
  the actual pairing/save JavaScript, and Python release-package checks.
- Run regression checks in CI alongside existing firmware/filesystem/site builds.
- Add `docs/VALIDATION.md` and `docs/USB_RELEASES.md` with reproducible checks,
  known limits and physical bench-test requirements.

No Wi-Fi updater, new camera backend, FC failsafe policy, or general warning
framework is included in this release. Actual GoPro mode readout and full
start/stop acknowledgement/error reporting remain separate improvements.
