#!/usr/bin/env python3
from pathlib import Path


def main() -> int:
    text = Path(__file__).with_name("Start-Overstrike.cmd").read_text(encoding="utf-8")

    required = (
        "Overstrike\\winmm.dll",
        "scripts_proxy.dll",
        "winmm.dll",
        "$item.IsReadOnly=$false",
        "AUTO-REPAIRED ReadOnly loader",
        ".MSM2-Mod-Tools.lock",
        "AUTO-REPAIRED stale MSM2 Mod-Tools lock",
        "Overstrike-winmm-",
    )
    for needle in required:
        assert needle in text, needle

    # Scope guard: loader normalization is intentionally limited to the two
    # game-root proxy files; it must not sweep Mods Library or project files.
    assert "foreach($name in @('scripts_proxy.dll','winmm.dll'))" in text
    assert "Mods Library" not in text
    assert "MSM2-SF" not in text
    assert "MSM2-WS" not in text

    print("PASS: Start-Overstrike preflight repairs misplaced loader, stale lock, and ReadOnly root loaders only")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
