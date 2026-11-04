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
TURN_SPEEDS: tuple[int, int] | None = None  # M3a result: only slight diagonal, reversal pulled strongly down -> reverted
MOD_NAME = (f"WallSprint - WallRun TurnMaxDown {TURN_MAX_DOWN}"
            + ("" if TURN_GAINS is None else f" Gain {TURN_GAINS[0]:g}/{TURN_GAINS[1]:g}")
            + ("" if TURN_SPEEDS is None else f" TurnSpeed {TURN_SPEEDS[0]}/{TURN_SPEEDS[1]}"))
# Experiment M2a (2026-11-04): play the ground sprint loop during WallRun by placing the owner-extracted vanilla
# sprint clip at the WallRun loop clip path. Each pair is optional (input file absent -> skipped). HYPOTHESIS: these
# are the sideways WallRun loop clips. See analysis/M2_ANIMATION_PLAN_20261104.md.
CLIP_SWAPS = (
    ("stand_sprint_fwd_spiderman.animclip", "characters/hero/hero_spiderman/animations/wall_run_spiderman.animclip"),
    ("stand_sprint_fwd_spiderman_miles.animclip", "characters/hero/hero_spiderman_milesmorales/animation/wall_run_spiderman_miles.animclip"),
)
CLIP_MIN, CLIP_MAX = 1024, 1024 * 1024


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


def stage_bytes(steering: bytes, clips: dict[str, bytes] | None = None) -> bytes:
    patched, _ = b.patch_steering(steering, TURN_MAX_DOWN, TURN_GAINS, TURN_SPEEDS)
    members = {f"0/{b.STEER_PATH}": patched, **(clips or {})}
    return b.make_stage(members, MOD_NAME + (" + SprintClip" if clips else ""))


def deploy(workspace: Path, game: Path) -> Path | None:
    src = workspace / INPUT_DIR / INPUT_NAME
    if not src.is_file():
        print(f"WallRun steering stage not deployed: place the vanilla {INPUT_NAME} in MSM2-WS\\{INPUT_DIR}\\ to enable it.")
        return None
    if _linked(src) or _linked(src.parent):
        raise ValueError("reparse/symlink input refused")
    stage = stage_bytes(src.read_bytes(), clip_members(src.parent))
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
