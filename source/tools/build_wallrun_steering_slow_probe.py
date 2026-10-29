#!/usr/bin/env python3
from __future__ import annotations

import argparse
import hashlib
import io
import json
import zipfile
from pathlib import Path

ASSET_PATH = "configs/hero/hero_WallRunSteeringSetup_Standard.config"
EXPECTED_SIZE = 544
EXPECTED_SHA256 = "32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b"
FIELD_NAME = "TurnSpeedSlow"
FIELD_OFFSET = 0x219
VANILLA_VALUE = 75
PROBE_VALUE = 5
MOD_NAME = "WS Steering Probe - TurnSpeedSlow 5"
AUTHOR = "deluxxxe-dev / WallSprint"
FIXED_ZIP_TIMESTAMP = (2026, 10, 29, 21, 15, 0)


def sha256_hex(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def patch_one_byte(data: bytes) -> bytes:
    if len(data) != EXPECTED_SIZE:
        raise ValueError(f"unexpected input size: {len(data)} != {EXPECTED_SIZE}")
    actual_sha = sha256_hex(data)
    if actual_sha != EXPECTED_SHA256:
        raise ValueError(f"unexpected input SHA256: {actual_sha}")
    if data[FIELD_OFFSET] != VANILLA_VALUE:
        raise ValueError(
            f"unexpected byte at 0x{FIELD_OFFSET:X}: {data[FIELD_OFFSET]} != {VANILLA_VALUE}"
        )
    patched = bytearray(data)
    patched[FIELD_OFFSET] = PROBE_VALUE
    result = bytes(patched)
    changed = [i for i, (a, b) in enumerate(zip(data, result)) if a != b]
    if changed != [FIELD_OFFSET]:
        raise ValueError(f"unexpected changed offsets: {changed}")
    return result


def _zipinfo(name: str) -> zipfile.ZipInfo:
    zi = zipfile.ZipInfo(name, FIXED_ZIP_TIMESTAMP)
    zi.compress_type = zipfile.ZIP_DEFLATED
    return zi


def make_stage(asset_bytes: bytes) -> bytes:
    info = {
        "game": "MSM2",
        "name": MOD_NAME,
        "author": AUTHOR,
        "format_version": 2,
    }
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        zf.writestr(_zipinfo(f"0/{ASSET_PATH}"), asset_bytes)
        zf.writestr(_zipinfo("info.json"), (json.dumps(info, indent=2) + "\n").encode())
    return buffer.getvalue()


def make_outer_zip(stage_bytes: bytes, stage_name: str) -> bytes:
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        zf.writestr(_zipinfo(stage_name), stage_bytes)
    return buffer.getvalue()


def build_probe(input_path: Path, output_zip: Path) -> dict[str, object]:
    vanilla = input_path.read_bytes()
    patched = patch_one_byte(vanilla)
    stage_name = "MSM2-WS-SteeringProbe-TurnSpeedSlow5.stage"
    stage_bytes = make_stage(patched)
    outer_bytes = make_outer_zip(stage_bytes, stage_name)
    output_zip.parent.mkdir(parents=True, exist_ok=True)
    output_zip.write_bytes(outer_bytes)

    with zipfile.ZipFile(io.BytesIO(outer_bytes), "r") as outer:
        if outer.namelist() != [stage_name]:
            raise ValueError(f"unexpected outer archive layout: {outer.namelist()}")
        recovered_stage = outer.read(stage_name)
    with zipfile.ZipFile(io.BytesIO(recovered_stage), "r") as stage:
        expected_member = f"0/{ASSET_PATH}"
        if stage.namelist() != [expected_member, "info.json"]:
            raise ValueError(f"unexpected stage layout: {stage.namelist()}")
        recovered_asset = stage.read(expected_member)
    changed = [i for i, (a, b) in enumerate(zip(vanilla, recovered_asset)) if a != b]
    if changed != [FIELD_OFFSET] or recovered_asset[FIELD_OFFSET] != PROBE_VALUE:
        raise ValueError(f"packaged asset delta mismatch: {changed}")

    return {
        "input_sha256": sha256_hex(vanilla),
        "modified_asset_sha256": sha256_hex(patched),
        "stage_sha256": sha256_hex(recovered_stage),
        "package_sha256": sha256_hex(outer_bytes),
        "field": FIELD_NAME,
        "offset": FIELD_OFFSET,
        "from": VANILLA_VALUE,
        "to": PROBE_VALUE,
        "output": str(output_zip),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Build authenticated TurnSpeedSlow steering probe")
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    result = build_probe(args.input, args.output)
    print("WALLRUN_STEERING_SLOW_PROBE_READY")
    for key, value in result.items():
        if key == "offset":
            value = f"0x{value:X}"
        print(f"{key}={value}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
