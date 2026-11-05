"""WS-owned deployment of the WallRun steering stage by Update-WS (owner decision 2026-11-04).

Reads the owner-extracted vanilla steering config from <MSM2-WS>/_inputs/, byte-patches TurnMaxDown
with pinned-hash/layout self-checks, and writes ONE loose stage into <game>/Mods Library/Mods-Developing/
MSM2-WS-Assets/ (WS-owned, outside MSM2-WS where stages are refused). Only a file that is already this
stage is ever replaced. No input -> skip. Anything unexpected -> ValueError (update aborts before deploy).
"""
from __future__ import annotations
import io, json, os, zipfile
from pathlib import Path
import build_wallrun_m1_experiment as b

TURN_MAX_DOWN = 90  # M1b runtime result: straight-down WallRun (analysis/C48_M1_RUNTIME_RESULT_20261104.md)
INPUT_DIR = "_inputs"
INPUT_NAME = "hero_WallRunSteeringSetup_Standard.config"
ASSET_DIR = "MSM2-WS-Assets"
STAGE_NAME = "MSM2-WS-WallRunSteering.stage"
# Experiment M1c (2026-11-04): sideways WR drifts to straight down after a while at TurnMaxDown 90.
# HYPOTHESIS: negative TurnGainFast/Slow drive that automatic downward pull. None = vanilla gains.
TURN_GAINS: tuple[float, float] | None = (0.0, 0.0)
# Experiment M3a (2026-11-05): diagonal stick snaps to the nearest axis once WS is active. HYPOTHESIS: the WallRun
# heading turn rate (vanilla TurnSpeedFast/Slow 100/75) limits turning toward a diagonal stick. None = vanilla.
# M6h (2026-11-06): WS speed parity. Measured WS travel ~16 m/s mean (vanilla MoveSpeedFast 16) vs ground sprint
# ~9-10 m/s; the camera lags at the higher speed (uneven distance). HYPOTHESIS: MoveSpeedFast is the WR speed.
MOVE_SPEED_FAST: int | None = 9
TURN_SPEEDS: tuple[int, int] | None = None  # M3a result: only slight diagonal, reversal pulled strongly down -> reverted
MOD_NAME = (f"WallSprint - WallRun TurnMaxDown {TURN_MAX_DOWN}"
            + ("" if TURN_GAINS is None else f" Gain {TURN_GAINS[0]:g}/{TURN_GAINS[1]:g}")
            + ("" if TURN_SPEEDS is None else f" TurnSpeed {TURN_SPEEDS[0]}/{TURN_SPEEDS[1]}")
            + ("" if MOVE_SPEED_FAST is None else f" Speed {MOVE_SPEED_FAST}"))
# Experiment M2a (2026-11-04): play the ground sprint loop during WallRun by placing the owner-extracted vanilla
# sprint clip at the WallRun loop clip path. Each pair is optional (input file absent -> skipped). HYPOTHESIS: these
# are the sideways WallRun loop clips. See analysis/M2_ANIMATION_PLAN_20261104.md.
CLIP_SWAPS = (
    ("stand_sprint_fwd_spiderman.animclip", "characters/hero/hero_spiderman/animations/wall_run_spiderman.animclip"),
    ("stand_sprint_fwd_spiderman_miles.animclip", "characters/hero/hero_spiderman_milesmorales/animation/wall_run_spiderman_miles.animclip"),
)
CLIP_MIN, CLIP_MAX = 1024, 1024 * 1024
# M6d (2026-11-05): frame the WS camera like the ground camera (M6f: standard Offset* values, see CAMERA_PATCH). Vanilla camera_wallrun_aimcontext
# (CameraAimContextConfig, owner-extracted, sha pinned) gets the TraversalOffset*/TraversalPivotHeight values of
# vanilla cameraaimcontextstandard (Down (0,0,-2.5), Level (0,0,-2), Up (0,0,-2.5), pivot 1.55). FOV (72) unchanged.
# Each slot keeps its stored width (u8 or float); every original value is checked before writing.
CAMERA_INPUT = "camera_wallrun_aimcontext.config"
CAMERA_PATH = "configs/camera/camera_wallrun_aimcontext.config"
CAMERA_SHA = "2c21f777386943996e6f1418ca46699fe355f1205c60fa772d66a989c8e6919c"
CAMERA_PATCH = (  # (offset, kind, vanilla, new)  kind B=u8 f=float32
    # M6f: standard Offset* (ground) values. M6e measured the ground camera 4.4-7.8 m away and ~2.3 m high, the
    # WS camera (with Traversal* 2-2.5 m) 2.9-3.8 m and ~1 m. OffsetUp Y is a u8 slot: 0.5 cannot be stored, kept 0.
    # M6g: the Down/Level/Up offsets are chosen by camera pitch. On a wall, running up makes the camera "look up"
    # (2 m), sideways is "level" (4.5 m), so the distance changed with travel direction. All three = Level.
    # M6h: camera height. Ground camera measured ~2.3 m above the ground, WS ~0.9-1.6 m above the wall. Y is
    # a u8 slot for Level/Up (integers only): Y = 1 m on all three, pivot 1.55 (standard Traversal pivot).
    (388, "B", 1, 0), (389, "f", -0.3, 1.0), (393, "f", -2.75, -4.5),     # OffsetDown  (0,1,-4.5)
    (452, "f", 1.25, 0.0), (456, "B", 0, 1), (457, "f", -2.25, -4.5),     # OffsetLevel (0,1,-4.5)
    (516, "f", 0.5, 0.0), (520, "B", 0, 1), (521, "f", -3.25, -4.5),      # OffsetUp    (0,1,-4.5)
    (528, "f", 1.35, 1.55),                                               # PivotHeight 1.55
)


def _linked(p: Path) -> bool:
    return p.is_symlink() or bool(getattr(p.lstat(), "st_file_attributes", 0) & 0x400)


def _is_ours(path: Path) -> bool:
    try:
        with zipfile.ZipFile(io.BytesIO(path.read_bytes())) as z:
            if "info.json" not in z.namelist():
                return False
            info = json.loads(z.read("info.json"))
            return info.get("name", "").startswith("WallSprint - WallRun TurnMaxDown") and info.get("game") == "MSM2"
    except (OSError, ValueError, zipfile.BadZipFile):
        return False


def clip_members(inputs: Path) -> dict[str, bytes]:
    members = {}
    for name, target in CLIP_SWAPS:
        src = inputs / name
        if not src.is_file():
            continue
        if _linked(src):
            raise ValueError(f"reparse/symlink clip input refused: {name}")
        data = src.read_bytes()
        if not CLIP_MIN <= len(data) <= CLIP_MAX:
            raise ValueError(f"clip input {name} has implausible size {len(data)}")
        members[f"0/{target}"] = data
        print(f"WallRun clip swap: {name} ({len(data)} bytes, sha256 {b.sha(data)[:16]}) -> {target}")
    return members


def patch_camera(data: bytes) -> bytes:
    import struct
    if b.sha(data) != CAMERA_SHA:
        raise ValueError("camera_wallrun_aimcontext input is not the pinned vanilla file")
    out = bytearray(data)
    for off, kind, old, new in CAMERA_PATCH:
        cur = struct.unpack_from("<" + kind, out, off)[0]
        if abs(cur - old) > 1e-5:
            raise ValueError(f"camera config layout mismatch at {off}: {cur} != {old}")
        struct.pack_into("<" + kind, out, off, new)
    return bytes(out)


def camera_members(inputs: Path) -> dict[str, bytes]:
    src = inputs / CAMERA_INPUT
    if not src.is_file():
        return {}
    if _linked(src):
        raise ValueError("reparse/symlink camera input refused")
    data = patch_camera(src.read_bytes())
    print(f"WS camera framing: {CAMERA_INPUT} -> uniform 4.5 m behind, 1 m up for every pitch, pivot 1.55")
    return {f"0/{CAMERA_PATH}": data}


def stage_bytes(steering: bytes, clips: dict[str, bytes] | None = None, camera: dict[str, bytes] | None = None) -> bytes:
    patched, _ = b.patch_steering(steering, TURN_MAX_DOWN, TURN_GAINS, TURN_SPEEDS, MOVE_SPEED_FAST)
    members = {f"0/{b.STEER_PATH}": patched, **(clips or {}), **(camera or {})}
    return b.make_stage(members, MOD_NAME + (" + SprintClip" if clips else "") + (" + GroundCam" if camera else ""))


def deploy(workspace: Path, game: Path) -> Path | None:
    src = workspace / INPUT_DIR / INPUT_NAME
    if not src.is_file():
        print(f"WallRun steering stage not deployed: place the vanilla {INPUT_NAME} in MSM2-WS\\{INPUT_DIR}\\ to enable it.")
        return None
    if _linked(src) or _linked(src.parent):
        raise ValueError("reparse/symlink input refused")
    stage = stage_bytes(src.read_bytes(), clip_members(src.parent), camera_members(src.parent))
    parent = game / "Mods Library" / "Mods-Developing"
    if not parent.is_dir() or _linked(parent):
        raise ValueError("Mods-Developing missing or reparse")
    folder = parent / ASSET_DIR
    if folder.exists() or folder.is_symlink():
        if not folder.is_dir() or _linked(folder):
            raise ValueError("asset folder is not a plain directory")
    else:
        folder.mkdir()
    target = folder / STAGE_NAME
    if target.exists():
        if target.read_bytes() == stage:
            print(f"WallRun steering stage up to date: {target.name} (TurnMaxDown {TURN_MAX_DOWN}).")
            return target
        if not _is_ours(target):
            raise ValueError("existing file is not a WallSprint steering stage; refusing to replace it")
    tmp = folder / (STAGE_NAME + ".tmp")
    with tmp.open("wb") as f:
        f.write(stage)
        f.flush()
        os.fsync(f.fileno())
    os.replace(tmp, target)
    print(f"WallRun steering stage deployed: {target.name} (TurnMaxDown {TURN_MAX_DOWN}); install via Overstrike.")
    return target
