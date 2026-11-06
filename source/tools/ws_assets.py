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
# ~9-10 m/s. M6i result: with 9 the WS steady speed was 21-24 m/s (faster), so MoveSpeedFast is NOT the WR speed
# (REJECTED as speed; meaning UNKNOWN). Owner: current speed is fine -> kept at 9 for now.
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
    # M8a (2026-11-06): WS jump motion. HYPOTHESIS: a jump from a sideways WR plays one of these WR jump clips (owner
    # search "wall_run": Peter horiz_jump + traversal jump_spin lf/rt, Miles jump_spin lf/rt + 02 lf/rt; Miles has no
    # horiz_jump). All are replaced by the ground sprint jump in one run; toswing/sling_up/vault/dodge jumps are kept.
    # Shared with vanilla WR (D20).
    ("stand_sprint_jump_fwd_spiderman.animclip", "characters/hero/hero_spiderman/animations/wall_run_horiz_jump_spiderman.animclip"),
    ("stand_sprint_jump_fwd_spiderman.animclip", "characters/hero/hero_spiderman/animations/traversal/wall_run_jump_spin_lf_spiderman.animclip"),
    ("stand_sprint_jump_fwd_spiderman.animclip", "characters/hero/hero_spiderman/animations/traversal/wall_run_jump_spin_rt_spiderman.animclip"),
    ("stand_sprint_jump_fwd_spiderman_miles.animclip", "characters/hero/hero_spiderman_milesmorales/animation/wall_run_jump_spin_lf_spiderman_miles.animclip"),
    ("stand_sprint_jump_fwd_spiderman_miles.animclip", "characters/hero/hero_spiderman_milesmorales/animation/wall_run_jump_spin_rt_spiderman_miles.animclip"),
    ("stand_sprint_jump_fwd_spiderman_miles.animclip", "characters/hero/hero_spiderman_milesmorales/animation/wall_run_jump_spin_02_lf_spiderman_miles.animclip"),
    ("stand_sprint_jump_fwd_spiderman_miles.animclip", "characters/hero/hero_spiderman_milesmorales/animation/wall_run_jump_spin_02_rt_spiderman_miles.animclip"),
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
    # M6j: measured steady distance ground 4.0 m vs WS 5.8-6.4 m with Z -4.5 (camera lag at WS ~2x ground speed).
    # Z -3.0 gave WS 3.9-4.9 m (mean ~4.5) vs ground ~4.0; M6k: Z -2.5.
    # M6n: PivotHeight is added along WORLD up: running up the wall the pivot moves 1.55 m ahead of the hero (camera
    # near, 2.4 m), running down it moves toward the camera (far, 2.8-4.1 m). Pivot 0 removes the direction
    # dependence; height comes only from the camera-frame Y = 1. Z -3.5 targets the ground ~4 m.
    (388, "B", 1, 0), (389, "f", -0.3, 1.0), (393, "f", -2.75, -3.5),     # OffsetDown  (0,1,-3.5)
    (452, "f", 1.25, 0.0), (456, "B", 0, 1), (457, "f", -2.25, -3.5),     # OffsetLevel (0,1,-3.5)
    (516, "f", 0.5, 0.0), (520, "B", 0, 1), (521, "f", -3.25, -3.5),      # OffsetUp    (0,1,-3.5)
    (528, "f", 1.35, 0.0),                                                # PivotHeight 0
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


# M6p (2026-11-06): WS auto-centering seen with WS camera follow OFF (0 writes) -> the vanilla follow camera centers
# by itself. herofollowcamconfig VelocityFollowData.Enabled (bool @2479, vanilla true) is the candidate. Experiment:
# false. NOTE (D17): this is the global hero follow camera, so ground velocity-follow is also off while installed.
# M6p result: no effect on WS centering -> disabled (kept for reference; it would also change the ground camera).
FOLLOW_ENABLED = False
FOLLOW_INPUT = "herofollowcamconfig.config"
FOLLOW_PATH = "configs/herofollowcamconfig/herofollowcamconfig.config"
FOLLOW_SHA = "79b64c0503fa2ee499a26830ad98472e4496a25bb01b45080ecb1fbb17c023c1"
FOLLOW_PATCH = ((2479, "B", 1, 0),)  # VelocityFollowData.Enabled true -> false


def _patch(data: bytes, sha: str, table, label: str) -> bytes:
    import struct
    if b.sha(data) != sha:
        raise ValueError(f"{label} input is not the pinned vanilla file")
    out = bytearray(data)
    for off, kind, old, new in table:
        cur = struct.unpack_from("<" + kind, out, off)[0]
        if abs(cur - old) > 1e-5:
            raise ValueError(f"{label} layout mismatch at {off}: {cur} != {old}")
        struct.pack_into("<" + kind, out, off, new)
    return bytes(out)


def patch_follow(data: bytes) -> bytes:
    return _patch(data, FOLLOW_SHA, FOLLOW_PATCH, "herofollowcamconfig")


# M6u (2026-11-06): the WS camera may use the CLIMB follow camera (wall crawl) rather than the hero follow camera.
# heroclimbfollowcamconfig VelocityFollowData.Enabled (bool @1682, vanilla true) -> false. Experiment; it also turns
# off vanilla wall-crawl camera follow while installed (D19).
CLIMB_ENABLED = True
CLIMB_INPUT = "heroclimbfollowcamconfig.config"
CLIMB_PATH = "configs/heroclimbfollowcamconfig/heroclimbfollowcamconfig.config"
CLIMB_SHA = "bd79fb6f21fd879712fa3672927c109cd74ed227893d673cd91749c4ed07c45e"
CLIMB_PATCH = ((1682, "B", 1, 0),)


def patch_climb(data: bytes) -> bytes:
    return _patch(data, CLIMB_SHA, CLIMB_PATCH, "heroclimbfollowcamconfig")


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
    print(f"WS camera framing: {CAMERA_INPUT} -> uniform 3.5 m behind, 1 m up for every pitch, pivot 0")
    out = {f"0/{CAMERA_PATH}": data}
    fsrc = inputs / FOLLOW_INPUT
    if FOLLOW_ENABLED and fsrc.is_file():
        if _linked(fsrc):
            raise ValueError("reparse/symlink follow input refused")
        out[f"0/{FOLLOW_PATH}"] = patch_follow(fsrc.read_bytes())
        print(f"Follow camera: {FOLLOW_INPUT} -> VelocityFollowData.Enabled false (no auto-centering)")
    csrc = inputs / CLIMB_INPUT
    if CLIMB_ENABLED and csrc.is_file():
        if _linked(csrc):
            raise ValueError("reparse/symlink climb input refused")
        out[f"0/{CLIMB_PATH}"] = patch_climb(csrc.read_bytes())
        print(f"Climb follow camera: {CLIMB_INPUT} -> VelocityFollowData.Enabled false (no auto-centering)")
    return out


# M8b (2026-11-06): WS jump vector. Vanilla hero_wallrunjumpsetup_standard (WallRunJumpSetupConfig, owner-extracted,
# sha pinned) JumpOffData_00/45/90 YBoostMax/YBoostMin (u8) 14/9, 12/6, 2/2. HYPOTHESIS: YBoost is the world-up speed
# added at a WR jump-off; 0 should leave the jump along the wall normal (the WS "up"). Also changes vanilla WR jumps (D21).
JUMP_ENABLED = False  # M8c: replaced by the WS-only in-memory write (camera_ref.cpp); file patch kept for reference
JUMP_INPUT = "hero_wallrunjumpsetup_standard.config"
JUMP_PATH = "configs/hero/hero_wallrunjumpsetup_standard.config"
JUMP_SHA = "a505ea2052fbce42c151550ff2ef4f1bdb3621512b836bd9111f0b7cc17def04"
JUMP_PATCH = ((869, "B", 14, 0), (870, "B", 9, 0),      # JumpOffData_00 YBoostMax/Min
              (1009, "B", 12, 0), (1010, "B", 6, 0),    # JumpOffData_45
              (1149, "B", 2, 0), (1150, "B", 2, 0))     # JumpOffData_90


def patch_jump(data: bytes) -> bytes:
    return _patch(data, JUMP_SHA, JUMP_PATCH, "hero_wallrunjumpsetup_standard")


def jump_members(inputs: Path) -> dict[str, bytes]:
    src = inputs / JUMP_INPUT
    if not JUMP_ENABLED or not src.is_file():
        return {}
    if _linked(src):
        raise ValueError("reparse/symlink jump input refused")
    print(f"WS jump: {JUMP_INPUT} -> JumpOffData YBoost 0 (no world-up boost at WR jump-off)")
    return {f"0/{JUMP_PATH}": patch_jump(src.read_bytes())}


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
    stage = stage_bytes(src.read_bytes(), {**clip_members(src.parent), **jump_members(src.parent)}, camera_members(src.parent))
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
