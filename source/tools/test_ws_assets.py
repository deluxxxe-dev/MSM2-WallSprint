#!/usr/bin/env python3
"""Fail-closed tests for ws_assets (synthetic steering file, temp dirs)."""
import hashlib, struct, sys, tempfile, zipfile, io, os
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_wallrun_m1_experiment as b
import dat1_config_fields as dat1
import ws_assets as a

F32, U8, I8 = dat1.FLAG_F32, dat1.FLAG_U8, dat1.FLAG_I8
flags = [F32 if w == 4 else U8 for w in b.STEER_WIDTHS]; flags[3] = flags[4] = I8
vals = [0.35, 0.15, 16, 0xD8, 0xEC, -3.5, -2.25, 12, 2.5, 100, 75, 0.72]
data = b"".join(struct.pack("<f", v) if f == F32 else bytes([v & 255]) for v, f in zip(vals, flags))
n = len(flags)
blob = b"\0\0\0\0" + bytes.fromhex("44001503") + struct.pack("<II", n, 12 * n + len(data))
blob += b"".join(struct.pack("<II", 0x1000 + k, f) for k, f in enumerate(flags)) + b"\0" * (4 * n) + data
steer = bytearray(b.STEER_SIZE)
start = 0x218 - 24 - (16 + 12 * n)
steer[start:start + len(blob)] = blob
steer = bytes(steer)
b.STEER_SHA = hashlib.sha256(steer).hexdigest()

a.TURN_GAINS = None; a.TURN_SPEEDS = None; a.MOVE_SPEED_FAST = None; a.MOD_NAME = f"WallSprint - WallRun TurnMaxDown {a.TURN_MAX_DOWN}"

def fails(fn):
    try: fn()
    except ValueError: return
    raise AssertionError("expected ValueError")

with tempfile.TemporaryDirectory() as t:
    game = Path(t) / "game"; ws = game / "Mods Library" / "Mods-Developing" / "MSM2-WS"
    ws.mkdir(parents=True)
    assert a.deploy(ws, game) is None                         # no input -> skip, nothing created
    assert not (ws.parent / a.ASSET_DIR).exists()
    (ws / a.INPUT_DIR).mkdir(); (ws / a.INPUT_DIR / a.INPUT_NAME).write_bytes(steer)
    out = a.deploy(ws, game)
    assert out == ws.parent / a.ASSET_DIR / a.STAGE_NAME and out.is_file()
    z = zipfile.ZipFile(io.BytesIO(out.read_bytes()))
    assert sorted(z.namelist()) == ["0/" + b.STEER_PATH, "info.json"]
    patched = z.read("0/" + b.STEER_PATH)
    assert sum(x != y for x, y in zip(steer, patched)) == 1 and patched[0x213] == a.TURN_MAX_DOWN
    first = out.read_bytes(); assert a.deploy(ws, game) == out and out.read_bytes() == first   # idempotent
    assert not list(out.parent.glob("*.tmp")) and [p.name for p in out.parent.iterdir()] == [a.STAGE_NAME]
    out.write_bytes(b"foreign")                                # foreign file never overwritten
    fails(lambda: a.deploy(ws, game)); assert out.read_bytes() == b"foreign"
    out.write_bytes(first); a.TURN_MAX_DOWN, saved = 80, a.TURN_MAX_DOWN   # our older stage -> replaced
    a.MOD_NAME = "WallSprint - WallRun TurnMaxDown 80"; a.deploy(ws, game)
    assert out.read_bytes() != first and zipfile.ZipFile(io.BytesIO(out.read_bytes())).read("0/" + b.STEER_PATH)[0x213] == 80
    a.TURN_GAINS = (0.0, 0.0); a.MOD_NAME += " Gain 0/0"; a.deploy(ws, game)   # gain variant replaces our stage
    pg = zipfile.ZipFile(io.BytesIO(out.read_bytes())).read("0/" + b.STEER_PATH)
    assert sum(x != y for x, y in zip(steer, pg)) == 5 and pg[0x213] == 80   # 1 TurnMaxDown byte + 2+2 gain float bytes
    (ws / a.INPUT_DIR / a.INPUT_NAME).write_bytes(steer[:-1] + b"\1")   # wrong SHA -> refuse
    fails(lambda: a.deploy(ws, game))

with tempfile.TemporaryDirectory() as t:                       # M2a clip swaps
    a.TURN_GAINS = None; a.TURN_MAX_DOWN = 90; a.MOD_NAME = "WallSprint - WallRun TurnMaxDown 90"
    b.STEER_SHA = hashlib.sha256(steer).hexdigest()
    game = Path(t) / "game"; ws = game / "Mods Library" / "Mods-Developing" / "MSM2-WS"; inp = ws / a.INPUT_DIR
    inp.mkdir(parents=True); (inp / a.INPUT_NAME).write_bytes(steer)
    clip = os.urandom(4096); (inp / a.CLIP_SWAPS[0][0]).write_bytes(clip)
    out = a.deploy(ws, game); z = zipfile.ZipFile(io.BytesIO(out.read_bytes()))
    assert z.read("0/" + a.CLIP_SWAPS[0][1]) == clip and ("0/" + a.CLIP_SWAPS[1][1]) not in z.namelist()
    assert "SprintClip" in z.read("info.json").decode()
    jump = os.urandom(8192); (inp / "stand_sprint_jump_fwd_spiderman.animclip").write_bytes(jump)   # M8a fan-out
    jm = a.clip_members(inp); jt = [t for n, t in a.CLIP_SWAPS if n == "stand_sprint_jump_fwd_spiderman.animclip"]
    assert len(jt) == 3 and all(jm["0/" + t] == jump for t in jt); (inp / "stand_sprint_jump_fwd_spiderman.animclip").unlink()
    (inp / a.CLIP_SWAPS[1][0]).write_bytes(b"x" * 10)          # implausible size -> refuse, previous stage kept
    before = out.read_bytes(); fails(lambda: a.deploy(ws, game)); assert out.read_bytes() == before
sp, rep = b.patch_steering(steer, 90, (0.0, 0.0), (255, 255))
assert sp[0x218] == 255 and sp[0x219] == 255 and rep["turn_speeds"] == {"TurnSpeedFast": 255, "TurnSpeedSlow": 255}
fails(lambda: b.patch_steering(steer, 90, None, (0, 255)))
ms, rep = b.patch_steering(steer, 90, None, None, 9)              # M6h MoveSpeedFast
assert rep["move_speed_fast"] == 9 and sum(x != y for x, y in zip(steer, ms)) == 2 and 9 in ms[0x200:0x213]
fails(lambda: b.patch_steering(steer, 90, None, None, 0))
# M6d camera framing patch (synthetic file with the vanilla values at the pinned offsets)
cam = bytearray(532)
for off, kind, old, new in a.CAMERA_PATCH: struct.pack_into("<" + kind, cam, off, old)
cam = bytes(cam); a.CAMERA_SHA = hashlib.sha256(cam).hexdigest()
pc = a.patch_camera(cam)
for off, kind, old, new in a.CAMERA_PATCH: assert abs(struct.unpack_from("<" + kind, pc, off)[0] - new) < 1e-6
assert len(pc) == len(cam) and sum(x != y for x, y in zip(cam, pc)) > 0
fails(lambda: a.patch_camera(cam[:-1] + b"\1"))                  # wrong SHA -> refuse
with tempfile.TemporaryDirectory() as t:
    game = Path(t) / "game"; ws = game / "Mods Library" / "Mods-Developing" / "MSM2-WS"; inp = ws / a.INPUT_DIR
    inp.mkdir(parents=True); (inp / a.INPUT_NAME).write_bytes(steer); (inp / a.CAMERA_INPUT).write_bytes(cam)
    out = a.deploy(ws, game); z = zipfile.ZipFile(io.BytesIO(out.read_bytes()))
    assert z.read("0/" + a.CAMERA_PATH) == pc and "GroundCam" in z.read("info.json").decode()
fol = bytearray(2532); fol[2479] = 1; fol = bytes(fol); a.FOLLOW_SHA = hashlib.sha256(fol).hexdigest()
assert a.patch_follow(fol)[2479] == 0 and sum(x != y for x, y in zip(fol, a.patch_follow(fol))) == 1
fails(lambda: a.patch_follow(fol[:-1] + b"\1"))
with tempfile.TemporaryDirectory() as t:
    game = Path(t) / "game"; ws = game / "Mods Library" / "Mods-Developing" / "MSM2-WS"; inp = ws / a.INPUT_DIR
    inp.mkdir(parents=True); (inp / a.INPUT_NAME).write_bytes(steer); (inp / a.CAMERA_INPUT).write_bytes(cam); (inp / a.FOLLOW_INPUT).write_bytes(fol)
    z = zipfile.ZipFile(io.BytesIO(a.deploy(ws, game).read_bytes())); assert ("0/" + a.FOLLOW_PATH) not in z.namelist()  # disabled
    a.FOLLOW_ENABLED = True; z = zipfile.ZipFile(io.BytesIO(a.deploy(ws, game).read_bytes())); assert z.read("0/" + a.FOLLOW_PATH)[2479] == 0; a.FOLLOW_ENABLED = False
clb = bytearray(1712); clb[1682] = 1; clb = bytes(clb); a.CLIMB_SHA = hashlib.sha256(clb).hexdigest()
assert a.patch_climb(clb)[1682] == 0; fails(lambda: a.patch_climb(clb[:-1] + b"\1"))
print("test_ws_assets: PASS")
