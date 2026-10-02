# ShutterBridge / IbexFuel project context

This repository is Robhobbers' fork of [YLabs-FPV/ShutterBridge](https://github.com/YLabs-FPV/ShutterBridge).
It is hosted as a technical side project on the IbexFuel website. IbexFuel itself is
an independent sports-carb drink and sports-nutrition brand; ShutterBridge is not a
nutrition product.

## Repositories and services

- Fork: <https://github.com/Robhobbers/ShutterBridge---GoPro>
- Upstream: <https://github.com/YLabs-FPV/ShutterBridge>
- Website: <https://ibexfuel.com>
- Flasher: <https://ibexfuel.com/en/flash/>
- Cloudflare Pages project: `ibexfuel`
- Pages root directory: `/site`
- Build command: `pnpm build`
- Build output directory: `/dist/client`

## Hardware and firmware

- Target board: Waveshare ESP32-S3-Zero (ESP32-S3FH4R2, 4 MB flash, 2 MB PSRAM).
- Flight-controller link: Betaflight over MSP on the configured UART.
- Camera link: Bluetooth LE to DJI Osmo, DJI Action / 360, or GoPro HERO9+.
- Current published firmware: `v0.1.1-rob.3`.
- Current Bluetooth local identity: `IbexCam`.
- Configuration Wi-Fi remains `ShutterBridge`, password `shutterbridge`.
- Configuration page: `http://10.0.0.1`.

## Product and design direction

ShutterBridge uses IbexFuel's visual language: alpine green, mineral white, glacier,
lichen and signal orange, with a clean technical and outdoors-oriented feel. The
ShutterBridge logo and product identity remain separate from the IbexFuel nutrition
brand, while sharing the same visual system for now.

## Working conventions

- Prefer the fork's code, releases and flasher over upstream assets.
- Preserve existing camera, pairing, settings and Wi-Fi behavior unless a change is
  explicitly requested.
- Explain technical changes in plain language and include the relevant validation.
- Treat the GitHub repository documentation as the canonical technical record.
- Never store API tokens, passwords, or other secrets in this project documentation.
