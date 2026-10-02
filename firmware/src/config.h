#pragma once

#define FW_VERSION "0.1.1-rob.3"
#define FW_VARIANT "Robhobbers/ShutterBridge---GoPro"
#define BLE_DEVICE_NAME "IbexCam"
#include "build_identity.h"

#define FC_UART       Serial1
#define FC_BAUD       115200
#define FC_RX_PIN     44
#define FC_TX_PIN     43
#define FC_RC_POLL_MS 50

#define OSD_UPDATE_MS  250
#define OSD_REFRESH_MS 1000

#define STATUS_LED_PIN 21

#define CONSOLE_BAUD 115200
