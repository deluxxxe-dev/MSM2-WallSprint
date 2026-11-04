#!/usr/bin/env python3
"""Synthetic fail-closed tests for the M1 builder (no game assets in Git)."""
import hashlib, io, os, struct, sys, zipfile
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_wallrun_m1_experiment as b
import dat1_config_fields as dat1

F32, U8, I8 = dat1.FLAG_F32, dat1.FLAG_U8, dat1.FLAG_I8

def make_struct(values, widths_flags, data_off_target=None, total=None, start=None):
    n = len(widths_flags)
    fixed = 12 * n
    data = b""
    for v, f in zip(values, widths_flags):
        data += struct.pack("<f", v) if f == F32 else bytes([v & 0xFF])
    size = fixed + len(data)
    hdr = b"\0\0\0\0" + bytes.fromhex("44001503") + struct.pack("<II", n, size)
    ents = b"".join(struct.pack("<II", 0x1000 + k, f) for k, f in enumerate(widths_flags))
    offs = struct.pack("<I", 0) * n
    return hdr + ents + offs + data

def synth_steering():
    flags = [F32 if w == 4 else U8 for w in b.STEER_WIDTHS]
    flags[3] = flags[4] = I8  # as in the real file
    vals = [0.35, 0.15, 16, 0xD8, 0xEC, -3.5, -2.25, 12, 2.5, 100, 75, 0.72]
    blob = make_struct(vals, flags)
    data_off_in_blob = 4 + 12 + 12 * len(flags)
    start = 0x218 - 24 - data_off_in_blob
    assert start > 0
    buf = bytearray(b.STEER_SIZE)
    buf[start:start + len(blob)] = blob
    assert len(blob) + start <= b.STEER_SIZE
    return bytes(buf)

def pin(monkey_data, name):
    setattr(b, name + "_SIZE", len(monkey_data))
    setattr(b, name + "_SHA", hashlib.sha256(monkey_data).hexdigest())

def must_fail(fn, *a):
    try:
        fn(*a)
    except ValueError:
        return
    raise AssertionError("expected ValueError")

steer = synth_steering()
pin(steer, "STEER")
patched, edit = b.patch_steering(steer, 60)
assert edit == {"offset": steer.index(bytes([12]), 0x200), "from": 12, "to": 60} or edit["to"] == 60
assert sum(1 for x, y in zip(steer, patched) if x != y) == 1
assert patched[edit["offset"]] == 60 and steer[edit["offset"]] == 12
must_fail(b.patch_steering, steer, 12)
must_fail(b.patch_steering, steer, 256)
must_fail(b.patch_steering, steer + b"\0", 60)
bad = bytearray(steer); bad[0x218] = 99; bad = bytes(bad); pin(bad, "STEER")
must_fail(b.patch_steering, bad, 60)       # known TurnSpeedFast value mismatch
pin(steer, "STEER")
shifted = bytearray(b.STEER_SIZE)           # layout shift -> offset self-check fails
blob = make_struct([0.35, 0.15, 16, 0xD8, 0xEC, -3.5, -2.25, 12, 2.5, 100, 75, 0.72], [F32 if w == 4 else U8 for w in b.STEER_WIDTHS])
shifted[0x100:0x100 + len(blob)] = blob; shifted = bytes(shifted); pin(shifted, "STEER")
must_fail(b.patch_steering, shifted, 60)
pin(steer, "STEER")

# speed: two synthetic 16-field structs
f16 = [U8] * 7 + [F32] * 4 + [U8] * 5
v_a = [6, 10, 34, 26, 12, 4, 14, 3.5, .35, .1, .35, 24, 20, 40, 30, 3]
v_b = [1, 1, 25, 10, 12, 4, 20, 6, .35, .1, .35, 2, 1, 40, 20, 0]
sp = b"\0" * 8 + make_struct(v_a, f16) + b"\0" * 8 + make_struct(v_b, f16) + b"\0" * 8
pin(sp, "SPEED"); b.MIN_SPEED_EDITS = 4
out, edits = b.patch_speed(sp)
assert [e["to"] for e in edits] == [48, 40, 4, 2], edits
assert sum(1 for x, y in zip(sp, out) if x != y) == 4
b.MIN_SPEED_EDITS = 10
must_fail(b.patch_speed, sp)               # too few edits -> hypothesis rejected
b.MIN_SPEED_EDITS = 4
zero = bytearray(sp); zero[edits[0]["offset"]] = 0; zero = bytes(zero); pin(zero, "SPEED")
must_fail(b.patch_speed, zero)

pin(sp, "SPEED")
pkg, rep = b.build(steer, sp, 60)
outer = zipfile.ZipFile(io.BytesIO(pkg))
assert outer.namelist() == ["MSM2-WS-M1-DownLimit.stage"]
stage = zipfile.ZipFile(io.BytesIO(outer.read("MSM2-WS-M1-DownLimit.stage")))
assert sorted(stage.namelist()) == sorted(["0/" + b.STEER_PATH, "0/" + b.SPEED_PATH, "info.json"])
assert pkg == b.build(steer, sp, 60)[0]    # deterministic
pkg2, _ = b.build(steer, None, 60)
o2 = zipfile.ZipFile(io.BytesIO(pkg2))
assert o2.namelist() == ["MSM2-WS-M1b-TurnMaxDown60.stage"]
assert len(zipfile.ZipFile(io.BytesIO(o2.read(o2.namelist()[0]))).namelist()) == 2

# real owner speed extract, when available locally (not stored in Git)
real = os.environ.get("WS_TEST_SPEED_CONFIG")
if real and Path(real).exists():
    import importlib; importlib.reload(b)
    o, e = b.patch_speed(Path(real).read_bytes())
    assert len(e) == 20
print("test_build_wallrun_m1_experiment: PASS")
