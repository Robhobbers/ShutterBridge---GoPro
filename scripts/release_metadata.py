"""Write fork identity and checksums beside a USB release's flash images."""
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OFFSETS = {"bootloader.bin": 0, "partitions.bin": 0x8000,
           "boot_app0.bin": 0xE000, "firmware.bin": 0x10000, "littlefs.bin": 0x310000}

def package(dist, tag):
    version = re.search(r'#define FW_VERSION "([^"]+)"',
                        (ROOT / "firmware/src/config.h").read_text()).group(1)
    if tag != "v" + version:
        raise ValueError("Release tag must match FW_VERSION")
    merged = f"ShutterBridge-GoPro-Robhobbers-{version}-merged.bin"
    files = [*OFFSETS, merged]
    for name in files:
        if not (dist / name).is_file() or (dist / name).stat().st_size == 0:
            raise ValueError(f"Missing/empty flash image: {name}")
    revision = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip()
    metadata = {
        "repository": "Robhobbers/ShutterBridge---GoPro",
        "version": version, "tag": tag, "commit": revision,
        "board": "Waveshare ESP32-S3-Zero (ESP32-S3FH4R2, 4MB flash)",
        "transport": "USB-C", "merged_image": merged, "merged_offset": 0,
        "parts": [{"file": name, "offset": offset} for name, offset in OFFSETS.items()],
        "sha256": {name: hashlib.sha256((dist / name).read_bytes()).hexdigest() for name in files},
    }
    (dist / "release.json").write_text(json.dumps(metadata, indent=2) + "\n")
    (dist / "USB_FLASHING.md").write_text((ROOT / "docs/USB_RELEASES.md").read_text())
    checks = [*files, "release.json", "USB_FLASHING.md"]
    (dist / "SHA256SUMS").write_text("".join(
        f"{hashlib.sha256((dist / name).read_bytes()).hexdigest()}  {name}\n" for name in checks))

if __name__ == "__main__":
    package(Path(sys.argv[1]), sys.argv[2])
