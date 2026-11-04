#!/usr/bin/env python3
"""M1 asset-only experiment: raise WallRun TurnMaxDown, with a speed positive control.

Inputs are owner-extracted vanilla configs (Modding Tool). Nothing proprietary is stored in Git.
Edits (all fail-closed, byte-level, WS-owned code):
  steering Standard: TurnMaxDown u8 12 -> --turn-max-down (default 60)
  speed Standard   : SettleSpeedMax / SettleSpeedMin u8 x2 (cap 255) in every field struct,
                     fields identified by the hashes at alphabetical index 11/12 of the first
                     16-field struct (HYPOTHESIS: runtime speed change confirms/rejects).
Field order in the steering struct is alphabetical (HYPOTHESIS) and is self-checked against the
known TurnSpeedFast/Slow offsets 0x218/0x219 and values 100/75, MoveSpeedFast 16, TurnMaxDown 12.
"""
from __future__ import annotations

import argparse
import hashlib
import io
import json
import struct
import sys
import zipfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import dat1_config_fields as dat1  # noqa: E402

STEER_PATH = "configs/hero/hero_WallRunSteeringSetup_Standard.config"
STEER_SIZE = 544
STEER_SHA = "32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b"
SPEED_PATH = "configs/hero/hero_WallRunSpeedSetup_Standard.config"
SPEED_SIZE = 6380
SPEED_SHA = "cc07f41b09fa7dddaf10c4f2f71f0596749b41ce5a8a81223ca03d790ce0c90c"  # owner extract 2026-11-04
STEER_ORDER = ["InTimeMax", "InTimeMin", "MoveSpeedFast", "TurnDampFast", "TurnDampSlow",
               "TurnGainFast", "TurnGainSlow", "TurnMaxDown", "TurnSpeedDecelScaleMax",
               "TurnSpeedFast", "TurnSpeedSlow", "TurnSpeedSpeedScaleMax"]
STEER_WIDTHS = [4, 4, 1, 1, 1, 4, 4, 1, 4, 1, 1, 4]
STEER_KNOWN = {"MoveSpeedFast": 16, "TurnMaxDown": 12, "TurnSpeedFast": 100, "TurnSpeedSlow": 75}
STEER_KNOWN_OFFSETS = {"TurnSpeedFast": 0x218, "TurnSpeedSlow": 0x219}
SPEED_ANCHOR_COUNT = 16
MIN_SPEED_EDITS = 10
SPEED_MAX_IDX, SPEED_MIN_IDX = 11, 12
MOD_NAME = "WS Experiment M1 - TurnMaxDown + Speed Control"
AUTHOR = "deluxxxe-dev / WallSprint"
ZIP_TS = (2026, 11, 4, 12, 0, 0)


def sha(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


def _need(data: bytes, size: int, digest: str, label: str) -> None:
    if len(data) != size or sha(data) != digest:
        raise ValueError(f"{label}: unexpected size/SHA256 ({len(data)}, {sha(data)})")


def patch_steering(data: bytes, turn_max_down: int, gains: tuple[float, float] | None = None) -> tuple[bytes, dict]:
    _need(data, STEER_SIZE, STEER_SHA, "steering")
    if not 13 <= turn_max_down <= 255:
        raise ValueError("turn_max_down must be 13..255")
    structs = [s for s in dat1.walk(data) if tuple(s.widths) == tuple(STEER_WIDTHS)]
    if len(structs) != 1:
        raise ValueError(f"steering: expected exactly one matching struct, found {len(structs)}")
    s = structs[0]
    off = dict(zip(STEER_ORDER, s.offsets))
    for name, want in STEER_KNOWN_OFFSETS.items():
        if off[name] != want:
            raise ValueError(f"steering layout self-check failed: {name} at 0x{off[name]:X} != 0x{want:X}")
    for name, want in STEER_KNOWN.items():
        if data[off[name]] != want:
            raise ValueError(f"steering value self-check failed: {name}={data[off[name]]} != {want}")
    if struct.unpack_from("<f", data, off["TurnSpeedSpeedScaleMax"])[0] != struct.unpack("<f", struct.pack("<f", 0.72))[0]:
        raise ValueError("steering float self-check failed: TurnSpeedSpeedScaleMax != 0.72")
    out = bytearray(data)
    out[off["TurnMaxDown"]] = turn_max_down
    allowed = {off["TurnMaxDown"]}
    report = {"offset": off["TurnMaxDown"], "from": 12, "to": turn_max_down}
    if gains is not None:
        for name, want, new in (("TurnGainFast", -3.5, gains[0]), ("TurnGainSlow", -2.25, gains[1])):
            if struct.unpack_from("<f", data, off[name])[0] != want:
                raise ValueError(f"steering float self-check failed: {name} != {want}")
            struct.pack_into("<f", out, off[name], new)
            allowed.update(range(off[name], off[name] + 4))
        report["gains"] = {"TurnGainFast": gains[0], "TurnGainSlow": gains[1]}
    changed = [i for i, (a, b) in enumerate(zip(data, out)) if a != b]
    if not changed or not set(changed) <= allowed:
        raise ValueError(f"steering unexpected delta {changed}")
    return bytes(out), report


def patch_speed(data: bytes, factor: int = 2) -> tuple[bytes, list[dict]]:
    _need(data, SPEED_SIZE, SPEED_SHA, "speed")
    structs = dat1.walk(data)
    anchor = next((s for s in structs if len(s.hashes) == SPEED_ANCHOR_COUNT), None)
    if anchor is None:
        raise ValueError("speed: no 16-field anchor struct")
    h_max, h_min = anchor.hashes[SPEED_MAX_IDX], anchor.hashes[SPEED_MIN_IDX]
    out, edits = bytearray(data), []
    for s in structs:
        if h_max not in s.hashes or h_min not in s.hashes:
            continue
        for label, h in (("SettleSpeedMax", h_max), ("SettleSpeedMin", h_min)):
            k = s.hashes.index(h)
            if s.widths[k] != 1:
                raise ValueError(f"speed: {label} is not a u8 in struct 0x{s.start:X}")
            old = data[s.offsets[k]]
            if not 0 < old < 128:
                raise ValueError(f"speed: implausible {label}={old} in struct 0x{s.start:X}")
            out[s.offsets[k]] = min(255, old * factor)
            edits.append({"struct": s.start, "field": label, "offset": s.offsets[k], "from": old, "to": out[s.offsets[k]]})
    if len(edits) < MIN_SPEED_EDITS:
        raise ValueError(f"speed: only {len(edits)} edits, layout hypothesis rejected")
    return bytes(out), edits


def _zi(name: str) -> zipfile.ZipInfo:
    z = zipfile.ZipInfo(name, ZIP_TS)
    z.compress_type = zipfile.ZIP_DEFLATED
    return z


def make_stage(members: dict[str, bytes], mod_name: str) -> bytes:
    info = {"game": "MSM2", "name": mod_name, "author": AUTHOR, "format_version": 2}
    stage = io.BytesIO()
    with zipfile.ZipFile(stage, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        for n, b in members.items():
            z.writestr(_zi(n), b)
        z.writestr(_zi("info.json"), (json.dumps(info, indent=2) + "\n").encode())
    return stage.getvalue()


def build(steering: bytes, speed: bytes | None, turn_max_down: int) -> tuple[bytes, dict]:
    st, st_edit = patch_steering(steering, turn_max_down)
    members = {f"0/{STEER_PATH}": st}
    report: dict = {"steering": st_edit, "steering_sha256": sha(st)}
    if speed is not None:
        sp, sp_edits = patch_speed(speed)
        members[f"0/{SPEED_PATH}"] = sp
        report["speed_edits"] = sp_edits
        report["speed_sha256"] = sha(sp)
    if speed is None:
        mod_name = f"WS Experiment M1b - TurnMaxDown {turn_max_down}"
        stage_name = f"MSM2-WS-M1b-TurnMaxDown{turn_max_down}.stage"
    else:
        mod_name, stage_name = MOD_NAME, "MSM2-WS-M1-DownLimit.stage"
    stage_bytes = make_stage(members, mod_name)
    outer = io.BytesIO()
    with zipfile.ZipFile(outer, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as z:
        z.writestr(_zi(stage_name), stage_bytes)
    report["package_sha256"] = sha(outer.getvalue())
    return outer.getvalue(), report


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--steering", type=Path, required=True)
    ap.add_argument("--speed", type=Path)
    ap.add_argument("--turn-max-down", type=int, default=60)
    ap.add_argument("output", type=Path)
    a = ap.parse_args()
    pkg, rep = build(a.steering.read_bytes(), a.speed.read_bytes() if a.speed else None, a.turn_max_down)
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_bytes(pkg)
    print("WALLRUN_M1_EXPERIMENT_READY")
    print(json.dumps(rep, indent=1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
