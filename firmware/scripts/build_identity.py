"""Embed the source revision in USB builds, including local dirty builds."""
import json
import subprocess
from pathlib import Path

Import("env")
project = Path(env.subst("$PROJECT_DIR"))
try:
    revision = subprocess.check_output(
        ["git", "rev-parse", "--short=12", "HEAD"], cwd=project, text=True).strip()
    dirty = subprocess.check_output(
        ["git", "status", "--porcelain"], cwd=project, text=True).strip()
    if dirty:
        revision += "-dirty"
except (OSError, subprocess.CalledProcessError):
    revision = "unknown"
output = Path(env.subst("$BUILD_DIR")) / "generated"
output.mkdir(parents=True, exist_ok=True)
header = output / "build_identity.h"
content = '#pragma once\n#define FW_BUILD_SHA ' + json.dumps(revision) + '\n'
if not header.exists() or header.read_text() != content:
    header.write_text(content)
env.Append(CPPPATH=[str(output)])
