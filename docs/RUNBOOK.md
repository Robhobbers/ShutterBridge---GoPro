# ShutterBridge project runbook

## Start a firmware change

1. Confirm the target board and camera behavior before editing.
2. Read the relevant source and these project notes.
3. Make the smallest change that owns the behavior.
4. Run `bash tests/run.sh`.
5. Run the PlatformIO firmware and filesystem builds when PlatformIO is available.
6. Update `CHANGELOG.md` and the relevant knowledge-base file for user-visible changes.

## Publish firmware

1. Bump `FW_VERSION` in `firmware/src/config.h`.
2. Commit and push the change to `main`.
3. Wait for the CI firmware build and regression tests to pass.
4. Create and push a matching `v<FW_VERSION>` tag.
5. Wait for the release workflow to build the five flash images and checksums.
6. Publish the draft release as a pre-release while physical validation is pending.
7. Confirm the release appears in the IbexFuel flasher and verify the live asset
   proxy returns `application/octet-stream` and the published SHA-256 values.

## Flash and pair a board

1. Open <https://ibexfuel.com/en/flash/> in desktop Chrome or Edge.
2. Select the latest published fork release.
3. Use **Full wipe** only when intentionally clearing saved settings and pairing.
4. Flash the Waveshare ESP32-S3-Zero, unplug it, and reconnect it normally.
5. Join Wi-Fi `ShutterBridge` with password `shutterbridge`.
6. Open <http://10.0.0.1>, select the camera type, scan and pair the camera.
7. Confirm the status LED and the camera status before testing Betaflight controls.

## Website deployment

Cloudflare Pages automatically deploys the connected `main` branch. The site uses
`site/` as its root, `pnpm build` as its build command, and `/dist/client` as its
output directory. Pages Functions live under `site/functions/`; adding an Astro
server route alone does not make a function available in this deployment.
