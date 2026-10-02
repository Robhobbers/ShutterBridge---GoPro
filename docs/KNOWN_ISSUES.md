# Known issues and limitations

- The GoPro may display `...` for the paired ShutterBridge controller even though
  the firmware's local Bluetooth identity is `IbexCam`. The standard Open GoPro BLE
  API does not provide a custom accessory-name field for this client connection.
- The status LED is state-based: yellow while searching, blue while a device is on
  the configuration Wi-Fi, green when the camera is online, and blinking red while
  recording.
- DJI Osmo Nano must be awake before recording or the resulting clip can be
  corrupted at a very low frame rate.
- The configuration Wi-Fi and Bluetooth share the ESP32 radio. Keeping a phone
  connected to the setup Wi-Fi during flight can affect the camera link; the System
  setting can disable Wi-Fi while armed.
- Physical camera, flight-controller and airframe validation is still required for
  changes that affect real hardware.
