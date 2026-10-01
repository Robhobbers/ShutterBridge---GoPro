#include <cassert>
#include <cstdio>
#include <Preferences.h>
#include "config/Settings.h"
int main() {
    Settings s; s.setDefaults();
    Preferences::available = false; assert(!s.save());
    Preferences::available = true; Preferences::failWrite = true; assert(!s.save());
    Preferences::failWrite = false; Preferences::corruptRead = true; assert(!s.save());
    Preferences::corruptRead = false; assert(s.save());
    Settings read; read.load(); assert(read.version == s.version);
    assert(read.recordStopDelayMs == s.recordStopDelayMs);
    puts("Settings persistence regressions passed (open/write/read-back failures and round trip).");
}
