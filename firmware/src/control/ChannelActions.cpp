#include "control/ChannelActions.h"

#include <Arduino.h>

void ChannelActions::loadFromModes(const ModeRange* modes, ShutterVideoMode svm,
                                   ShutterVideoMode modeStyle, uint16_t startDelayMs,
                                   uint16_t stopDelayMs, OpMode opMode) {
    for (int i = 0; i < FUNC_COUNT; i++)
        modes_[i] = modes[i];
    svm_          = svm;
    modeStyle_    = modeStyle;
    startDelayMs_ = startDelayMs;
    stopDelayMs_  = stopDelayMs;
    opMode_       = opMode;
    pendingRec_   = REC_NONE;  // cancel any queued record command on reconfigure
    primed_       = false;
    armPrimed_    = false;
    armRecordingRequested_ = false;
    startAttempted_ = false;
}

void ChannelActions::scheduleRec(bool start) {
    // Every new demand supersedes the old timer, including zero-delay commands.
    pendingRec_  = start ? REC_START : REC_STOP;
    pendingAtMs_ = millis() + (start ? startDelayMs_ : stopDelayMs_);
}

void ChannelActions::processPending(Camera& cam, uint32_t now) {
    if (pendingRec_ == REC_NONE || (int32_t)(now - pendingAtMs_) < 0)
        return;
    if (pendingRec_ == REC_START && opMode_ == OpMode::RecordOnArm) {
        const CameraStatus& s = cam.status();
        // A late connection must not consume the recording request. Never act on
        // stale readiness or send a video start while the camera is in photo mode.
        if (!cam.isConnected() || !s.lastUpdateMs ||
            (int32_t)(now - s.lastUpdateMs) >= 2500 || !s.ready || s.mode == CamMode::Photo)
            return;
        if (!s.isRecording()) {
            cam.startRecord();
            startAttempted_ = true;
            lastStartAttemptMs_ = now;
        }
    } else {
        (pendingRec_ == REC_START) ? cam.startRecord() : cam.stopRecord();
    }
    pendingRec_ = REC_NONE;
}

void ChannelActions::update(const RcState& rc, Camera& cam, bool armed) {
    const uint32_t now = millis();

    // Apply input changes BEFORE expiring timers: a re-arm at the stop deadline
    // must cancel the stop rather than briefly ending the clip.
    if (opMode_ == OpMode::RecordOnArm) {
        if (!armPrimed_ || armed != prevArmed_) {
            const bool initial = !armPrimed_;
            prevArmed_ = armed;
            armPrimed_ = true;
            armRecordingRequested_ = armed;
            startAttempted_ = false;
            if (armed || !initial)
                scheduleRec(armed);
        }
        // Reconcile an ongoing arm demand after reconnect or an unconfirmed start.
        // Rate-limit requests; telemetry, not a successful BLE write, proves recording.
        if (armRecordingRequested_ && pendingRec_ == REC_NONE &&
            !cam.status().isRecording() &&
            (!startAttempted_ || (uint32_t)(now - lastStartAttemptMs_) >= 20000)) {
            pendingRec_ = REC_START;
            pendingAtMs_ = now;
        }
        processPending(cam, now);
        return;
    }

    if (!rc.valid) {
        processPending(cam, now);
        return;
    }

    bool active[FUNC_COUNT];
    for (int i = 0; i < FUNC_COUNT; i++) {
        const ModeRange& m = modes_[i];
        if (m.aux == 0) {
            active[i] = false;
        } else {
            const uint16_t v = rc.aux(m.aux);
            active[i]        = (v >= m.rangeMin && v <= m.rangeMax);
        }
    }

    if (!primed_) {
        for (int i = 0; i < FUNC_COUNT; i++)
            prevActive_[i] = active[i];
        primed_ = true;
        return;
    }

    for (int i = 0; i < FUNC_COUNT; i++) {
        if (active[i] == prevActive_[i])
            continue;
        const Func f      = (Func)i;
        const bool rising = active[i];
        prevActive_[i]    = active[i];

        if (f == Func::Shutter) {
            const bool photo = cam.status().mode == CamMode::Photo;
            if (photo) {
                if (rising)
                    cam.takePhoto();
            } else if (svm_ == ShutterVideoMode::TwoPos) {
                scheduleRec(rising);
            } else if (rising) {
                scheduleRec(pendingRec_ != REC_NONE ? pendingRec_ != REC_START
                                                      : !cam.status().isRecording());
            }
            continue;
        }

        if (f == Func::CameraMode) {
            if (modeStyle_ == ShutterVideoMode::TwoPos) {
                cam.setMode(rising ? CamMode::Photo : CamMode::Video);
            } else if (rising) {
                cam.setMode(cam.status().mode == CamMode::Photo ? CamMode::Video : CamMode::Photo);
            }
            continue;
        }

        if (rising) {
            switch (f) {
                case Func::PresetVideo:
                    cam.loadPresetGroup(PresetGroup::Video);
                    break;
                case Func::PresetPhoto:
                    cam.loadPresetGroup(PresetGroup::Photo);
                    break;
                case Func::PresetTimelapse:
                    cam.loadPresetGroup(PresetGroup::Timelapse);
                    break;
                default:
                    break;
            }
        }
    }
    processPending(cam, now);
}
