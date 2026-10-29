#!/usr/bin/env python3
from __future__ import annotations

import argparse
from dataclasses import dataclass
import hashlib
import io
import json
import zipfile
from pathlib import Path

ASSET_PATH = "configs/hero/hero_WallRunSteeringSetup_Standard.config"
EXPECTED_SIZE = 544
EXPECTED_SHA256 = "32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b"
AUTHOR = "deluxxxe-dev / WallSprint"
FIXED_ZIP_TIMESTAMP = (2026, 10, 29, 20, 30, 0)


@dataclass(frozen=True)
class ProbeSpec:
    key: str
    field: str
    offset: int
    vanilla: int
    probe: int
    mod_name: str
    stage_name: str


PROBES = {
    "fast10": ProbeSpec(
        key="fast10",
        field="TurnSpeedFast",
        offset=0x218,
        vanilla=100,
        probe=10,
        mod_name="WS Steering Probe - TurnSpeedFast 10",
        stage_name="MSM2-WS-SteeringProbe-TurnSpeedFast10.stage",
    ),
    "slow5": ProbeSpec(
        key="slow5",
        field="TurnSpeedSlow",
        offset=0x219,
        vanilla=75,
        probe=5,
        mod_name="WS Steering Probe - TurnSpeedSlow 5",
        stage_name="MSM2-WS-SteeringProbe-TurnSpeedSlow5.stage",
    ),
}

# Backward-compatible constants used by older focused checks/importers.
FIELD_NAME = PROBES["fast10"].field
FIELD_OFFSET = PROBES["fast10"].offset
VANILLA_VALUE = PROBES["fast10"].vanilla
PROBE_VALUE = PROBES["fast10"].probe
MOD_NAME = PROBES["fast10"].mod_name


def sha256_hex(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def patch_one_byte(
    data: bytes,
    *,
    expected_sha256: str,
    expected_size: int,
    offset: int,
    expected_old: int,
    new_value: int,
) -> bytes:
    if len(data) != expected_size:
        raise ValueError(f"unexpected input size: {len(data)} != {expected_size}")
    actual_sha = sha256_hex(data)
    if actual_sha != expected_sha256:
        raise ValueError(f"unexpected input SHA256: {actual_sha}")
    if not 0 <= offset < len(data):
        raise ValueError("patch offset outside input")
    if data[offset] != expected_old:
        raise ValueError(
            f"unexpected byte at 0x{offset:X}: {data[offset]} != {expected_old}"
        )
    if not 0 <= new_value <= 0xFF:
        raise ValueError("new value must fit one byte")

    patched = bytearray(data)
    patched[offset] = new_value
    result = bytes(patched)
    changed = [i for i, (a, b) in enumerate(zip(data, result)) if a != b]
    if changed != [offset]:
        raise ValueError(f"unexpected changed offsets: {changed}")
    return result


def _zipinfo(name: str) -> zipfile.ZipInfo:
    zi = zipfile.ZipInfo(name, FIXED_ZIP_TIMESTAMP)
    zi.compress_type = zipfile.ZIP_DEFLATED
    return zi


def make_stage(asset_bytes: bytes, spec: ProbeSpec | None = None) -> bytes:
    spec = spec or PROBES["fast10"]
    info = {
        "game": "MSM2",
        "name": spec.mod_name,
        "author": AUTHOR,
        "format_version": 2,
    }
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        zf.writestr(_zipinfo(f"0/{ASSET_PATH}"), asset_bytes)
        info_bytes = (json.dumps(info, indent=2, ensure_ascii=False) + "\n").encode("utf-8")
        zf.writestr(_zipinfo("info.json"), info_bytes)
    return buffer.getvalue()


def make_outer_zip(stage_bytes: bytes, stage_name: str) -> bytes:
    buffer = io.BytesIO()
    with zipfile.ZipFile(buffer, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
        zf.writestr(_zipinfo(stage_name), stage_bytes)
    return buffer.getvalue()


def build_probe(input_path: Path, output_zip: Path, spec: ProbeSpec | None = None) -> dict[str, object]:
    spec = spec or PROBES["fast10"]
    vanilla = input_path.read_bytes()
    patched = patch_one_byte(
        vanilla,
        expected_sha256=EXPECTED_SHA256,
        expected_size=EXPECTED_SIZE,
        offset=spec.offset,
        expected_old=spec.vanilla,
        new_value=spec.probe,
    )

    stage_bytes = make_stage(patched, spec)
    outer_bytes = make_outer_zip(stage_bytes, spec.stage_name)
    output_zip.parent.mkdir(parents=True, exist_ok=True)
    output_zip.write_bytes(outer_bytes)

    # Verify the written package before reporting success.
    with zipfile.ZipFile(io.BytesIO(outer_bytes), "r") as outer:
        if outer.namelist() != [spec.stage_name]:
            raise ValueError(f"unexpected outer archive layout: {outer.namelist()}")
        recovered_stage = outer.read(spec.stage_name)

    with zipfile.ZipFile(io.BytesIO(recovered_stage), "r") as stage:
        expected_member = f"0/{ASSET_PATH}"
        if stage.namelist() != [expected_member, "info.json"]:
            raise ValueError(f"unexpected stage layout: {stage.namelist()}")
        recovered_asset = stage.read(expected_member)

    changed = [i for i, (a, b) in enumerate(zip(vanilla, recovered_asset)) if a != b]
    if changed != [spec.offset]:
        raise ValueError(f"packaged asset changed unexpected offsets: {changed}")
    if recovered_asset[spec.offset] != spec.probe:
        raise ValueError("packaged asset does not contain requested probe value")

    return {
        "input_sha256": sha256_hex(vanilla),
        "modified_asset_sha256": sha256_hex(patched),
        "stage_sha256": sha256_hex(recovered_stage),
        "package_sha256": sha256_hex(outer_bytes),
        "probe": spec.key,
        "field": spec.field,
        "offset": spec.offset,
        "from": spec.vanilla,
        "to": spec.probe,
        "output": str(output_zip),
    }


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Build an authenticated MSM2 WallRun steering one-field probe package."
    )
    parser.add_argument("input", type=Path, help="extracted vanilla Standard steering config")
    parser.add_argument("output", type=Path, help="output .zip path")
    parser.add_argument("--probe", choices=sorted(PROBES), default="fast10")
    args = parser.parse_args()

    result = build_probe(args.input, args.output, PROBES[args.probe])
    print("WALLRUN_STEERING_PROBE_READY")
    for key, value in result.items():
        if key == "offset":
            value = f"0x{value:X}"
        print(f"{key}={value}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
