#include <initializer_list>

#include <cassert>
#include <cstdio>
#include "control/ChannelActions.h"
uint32_t now = 1;
uint32_t millis() { return now; }
struct CameraMock : Camera {
    CameraStatus s;
    bool online = true;
    bool accepted = true;
    int starts = 0, stops = 0, photos = 0;
    bool begin() override { return true; }
    void poll() override {}
    bool startRecord() override { ++starts; return online && accepted; }
    bool stopRecord() override { ++stops; return online; }
    bool takePhoto() override { ++photos; return online; }
    const CameraStatus& status() const override { return s; }
    bool isConnected() const override { return online; }
};
struct Fixture {
    CameraMock cam;
    ChannelActions actions;
    RcState rc;
    Fixture(uint16_t start = 0, uint16_t stop = 2000, OpMode op = OpMode::RecordOnArm,
            ShutterVideoMode trigger = ShutterVideoMode::TwoPos) {
        ModeRange modes[FUNC_COUNT]{};
        modes[0] = {1, 1800, 2100};
        cam.s.mode = CamMode::Video;
        rc.valid = true; rc.count = 5; rc.ch[4] = 1000;
        actions.loadFromModes(modes, trigger, ShutterVideoMode::TwoPos, start, stop, op);
        tick(1, false);
    }
    void tick(uint32_t time, bool armed, bool fresh = true) {
        now = time;
        if (fresh) cam.s.lastUpdateMs = now;
        actions.update(rc, cam, armed);
    }
};
int main() {
    for (uint32_t rearm : {1000u, 2200u, 2300u}) {
        Fixture f; f.tick(100, true); f.cam.s.recState = RecState::Recording;
        f.tick(200, false); f.tick(rearm, true); f.tick(5000, true);
        assert(f.cam.stops == 0 && f.cam.starts == 1);
    }
    { Fixture f(2000, 0); f.tick(100, true); f.tick(2100, false);
      assert(f.cam.starts == 0 && f.cam.stops == 1); }
    { Fixture f; f.cam.online = false; f.tick(100, true); assert(f.cam.starts == 0);
      f.cam.online = true; f.tick(1000, true); assert(f.cam.starts == 1); }
    { Fixture f; f.tick(100, true); f.cam.s.recState = RecState::Recording;
      f.tick(1000, true); f.cam.online = false; f.cam.s.recState = RecState::Idle;
      f.tick(2000, true); f.cam.online = true; f.tick(20100, true);
      assert(f.cam.starts == 2); }
    { Fixture f; f.cam.accepted = false; f.tick(100, true); f.tick(20099, true);
      assert(f.cam.starts == 1); f.tick(20100, true); assert(f.cam.starts == 2);
      f.tick(20200, false); f.tick(50000, false); assert(f.cam.starts == 2); }
    { Fixture f; f.tick(100, true); f.cam.s.recState = RecState::Recording;
      f.tick(30000, true); assert(f.cam.starts == 1); }
    { Fixture f; f.cam.s.ready = false; f.tick(100, true); assert(f.cam.starts == 0);
      f.cam.s.ready = true; f.tick(5000, true, false); assert(f.cam.starts == 0);
      f.cam.s.mode = CamMode::Photo; f.tick(6000, true); assert(f.cam.starts == 0);
      f.cam.s.mode = CamMode::Video; f.tick(7000, true); assert(f.cam.starts == 1); }
    { Fixture f(1000); f.cam.online = false; f.tick(100, true); f.tick(200, false);
      f.cam.online = true; f.tick(10000, false); assert(f.cam.starts == 0); }
    { Fixture f(1000); f.tick(100, true); f.tick(1099, true); assert(f.cam.starts == 0);
      f.tick(1100, true); assert(f.cam.starts == 1); }
    { Fixture f; f.tick(100, true); f.tick(200, false); f.tick(2199, false);
      assert(f.cam.stops == 0); f.tick(2200, false); assert(f.cam.stops == 1); }
    { Fixture f(0, 2000, OpMode::Manual); f.rc.ch[4] = 1900; f.tick(100, false);
      f.rc.ch[4] = 1000; f.tick(200, false); f.rc.ch[4] = 1900; f.tick(2200, false);
      assert(f.cam.starts == 2 && f.cam.stops == 0); }
    { Fixture f(1000, 0, OpMode::Manual, ShutterVideoMode::Momentary);
      f.rc.ch[4] = 1900; f.tick(100, false); f.rc.ch[4] = 1000; f.tick(200, false);
      f.rc.ch[4] = 1900; f.tick(300, false); f.tick(2000, false);
      assert(f.cam.starts == 0 && f.cam.stops == 1); }
    { Fixture f(0, 0, OpMode::Manual); f.cam.s.mode = CamMode::Photo;
      f.rc.ch[4] = 1900; f.tick(100, false); f.tick(200, false);
      assert(f.cam.photos == 1 && f.cam.starts == 0); }
    { Fixture f; f.tick(0xfffffff0u, true); f.tick(0xfffffff1u, false);
      f.tick(0x7c0u, false); assert(f.cam.stops == 0);
      f.tick(0x7c1u, false); assert(f.cam.stops == 1); }
    puts("Recording regressions passed (including deadline ordering, reconnect, retry, manual/photo, wraparound).");
}
