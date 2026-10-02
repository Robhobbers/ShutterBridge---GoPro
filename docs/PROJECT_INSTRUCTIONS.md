# Instructions for the IbexFuel / ShutterBridge project

Use this repository and its documentation as the technical source of truth.

The project has two related but separate identities: IbexFuel is a sports-carb and
sports-nutrition brand, while ShutterBridge is an ESP32 camera-control side project
hosted on the IbexFuel domain. Keep ShutterBridge's technical purpose clear while
using the shared IbexFuel visual style.

Use the Robhobbers fork and its Cloudflare Pages deployment. Target the
Waveshare ESP32-S3-Zero. Preserve existing camera, pairing, settings, Wi-Fi and
Betaflight behavior unless the user asks for a change. Explain changes plainly,
run the nearest relevant checks, and report what was verified and what still needs
physical hardware testing.

Before changing production deployment, firmware releases or hardware behavior,
complete the local validation and CI checks available in the repository. Do not
put secrets in source files or project notes.
