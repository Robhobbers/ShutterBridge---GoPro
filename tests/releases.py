import hashlib
import importlib.util
import json
import re
import tempfile
import unittest
from pathlib import Path

spec = importlib.util.spec_from_file_location("release_metadata", "scripts/release_metadata.py")
release = importlib.util.module_from_spec(spec)
spec.loader.exec_module(release)
VERSION = re.search(r'#define FW_VERSION "([^"]+)"',
                    Path("firmware/src/config.h").read_text()).group(1)

class ReleaseTests(unittest.TestCase):
    def test_complete_identified_package_and_checksums(self):
        with tempfile.TemporaryDirectory() as tmp:
            folder = Path(tmp)
            names = [*release.OFFSETS, f"ShutterBridge-GoPro-Robhobbers-{VERSION}-merged.bin"]
            for name in names:
                (folder / name).write_bytes(b"test-image-" + name.encode())
            release.package(folder, "v" + VERSION)
            data = json.loads((folder / "release.json").read_text())
            self.assertEqual(data["repository"], "Robhobbers/ShutterBridge---GoPro")
            self.assertEqual(data["version"], VERSION)
            self.assertEqual(len(data["commit"]), 40)
            self.assertEqual(data["parts"][-1]["offset"], 0x310000)
            for line in (folder / "SHA256SUMS").read_text().splitlines():
                digest, name = line.split("  ")
                self.assertEqual(digest, hashlib.sha256((folder / name).read_bytes()).hexdigest())

    def test_rejects_wrong_version_and_missing_image(self):
        with tempfile.TemporaryDirectory() as tmp:
            with self.assertRaises(ValueError): release.package(Path(tmp), "v999.0.0")
            with self.assertRaises(ValueError): release.package(Path(tmp), "v" + VERSION)

if __name__ == "__main__": unittest.main()
