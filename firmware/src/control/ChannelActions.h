#pragma once

#include <cstdint>

#include "camera/Camera.h"
#include "control/Modes.h"
#include "fc/FlightController.h"

class ChannelActions {
   public:
    void loadFromModes(const ModeRange* modes, ShutterVideoMode svm, ShutterVideoMode modeStyle,
                       uint16_t startDelayMs, uint16_t stopDelayMs, OpMode opMode);
    void update(const RcState& rc, Camera& cam, bool armed);

   private:
    ModeRange        modes_[FUNC_COUNT];
    bool             prevActive_[FUNC_COUNT] = {false};
    bool             primed_                 = false;  // first frame seeds prevActive_
    ShutterVideoMode svm_                    = ShutterVideoMode::TwoPos;
    ShutterVideoMode modeStyle_              = ShutterVideoMode::TwoPos;
    OpMode           opMode_                 = OpMode::Manual;
    bool             prevArmed_              = false;  // for record-on-arm edge detection
    bool             armPrimed_              = false;

    enum PendingRec : uint8_t { REC_NONE, REC_START, REC_STOP };
    void       scheduleRec(bool start);
    void       processPending(Camera& cam, uint32_t now);
    bool       armRecordingRequested_ = false;
    bool       startAttempted_ = false;
    uint32_t   lastStartAttemptMs_ = 0;
    uint16_t   startDelayMs_ = 0;
    uint16_t   stopDelayMs_  = 0;
    PendingRec pendingRec_   = REC_NONE;
    uint32_t   pendingAtMs_  = 0;
};
