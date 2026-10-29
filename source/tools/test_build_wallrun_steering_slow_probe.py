#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import io
import json
import zipfile

import build_wallrun_steering_slow_probe as probe


def main() -> int:
    assert probe.EXPECTED_SIZE == 544
    assert probe.EXPECTED_SHA256 == "32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b"
    assert probe.FIELD_NAME == "TurnSpeedSlow"
    assert probe.FIELD_OFFSET == 0x219
    assert probe.VANILLA_VALUE == 75
    assert probe.PROBE_VALUE == 5
    assert probe.ASSET_PATH == "configs/hero/hero_WallRunSteeringSetup_Standard.config"
    print("PASS: slow probe contract pins current-build vanilla TurnSpeedSlow 75->5")

    data = bytearray(544)
    data[0x219] = 75
    fake = bytes(data)
    old_sha = probe.EXPECTED_SHA256
    try:
        probe.EXPECTED_SHA256 = hashlib.sha256(fake).hexdigest()
        out = probe.patch_one_byte(fake)
    finally:
        probe.EXPECTED_SHA256 = old_sha
    assert out[0x219] == 5
    assert [i for i, (a, b) in enumerate(zip(fake, out)) if a != b] == [0x219]
    print("PASS: slow probe changes exactly one authenticated byte")

    stage_bytes = probe.make_stage(b"asset")
    with zipfile.ZipFile(io.BytesIO(stage_bytes), "r") as zf:
        expected = f"0/{probe.ASSET_PATH}"
        assert zf.namelist() == [expected, "info.json"]
        info = json.loads(zf.read("info.json"))
        assert info == {
            "game": "MSM2",
            "name": "WS Steering Probe - TurnSpeedSlow 5",
            "author": "deluxxxe-dev / WallSprint",
            "format_version": 2,
        }
    print("PASS: slow probe stage uses MSM2 format_version 2 and intended asset path")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
