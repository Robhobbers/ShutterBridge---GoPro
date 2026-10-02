# Project decisions

## 2026-10-02 — Keep ShutterBridge separate from IbexFuel

ShutterBridge is hosted on `ibexfuel.com` as a side project, but its product
identity and documentation remain about the camera-control hardware. It may share
IbexFuel's visual system without being presented as a nutrition product.

## 2026-10-02 — Use Cloudflare Pages with the fork as the source

Cloudflare Pages is connected to `Robhobbers/ShutterBridge---GoPro`, branch `main`,
with the site rooted at `/site`. Published GitHub releases from the fork feed the
browser flasher.

## 2026-10-02 — Firmware downloads must be served by a Pages Function

The Astro API route was not present in the Cloudflare Pages deployment and returned
the site's HTML fallback for firmware requests. The flasher then wrote HTML bytes to
the board. The production fix is `site/functions/api/gh-asset.ts`, with client-side
image validation in `site/src/components/flasher/Flasher.svelte`.

## 2026-10-02 — Bluetooth identity is IbexCam

The firmware uses `BLE_DEVICE_NAME` set to `IbexCam` for its local NimBLE identity.
The ESP32 is a BLE client connecting to the GoPro, so the GoPro may still show `...`
for the paired controller. That label is controlled by the GoPro's pairing UI and
is not guaranteed to reflect the ESP32's local name.
