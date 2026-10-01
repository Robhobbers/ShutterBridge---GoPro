#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
TEST_BUILD=$(mktemp -d)
trap 'rm -rf "$TEST_BUILD"' EXIT
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -Itests/stubs -Ifirmware/src \
  tests/recording.cpp firmware/src/control/ChannelActions.cpp -o "$TEST_BUILD/recording"
"$TEST_BUILD/recording"
"${CXX:-c++}" -std=c++17 -Wall -Wextra -Werror -Itests/stubs -Ifirmware/src \
  tests/settings.cpp firmware/src/config/Settings.cpp -o "$TEST_BUILD/settings"
"$TEST_BUILD/settings"
node --test tests/pairing.cjs
python3 -B tests/releases.py
