#!/usr/bin/env python3
"""M6 research-only: dump the exact-build bytes of the WS camera functions for offline (cloud) disassembly.
Read-only, offline, exact-EXE SHA pin. Writes JSON with the .pdata bounds and the raw bytes (capped) of:
  0x1065680 HeroCameraManager::UpdateCameraRefMatrix (actor-relative branch, setter call sites 0x1065c44/0x1065d81)
  0x3350220 LookCameraMover reference setter
Do not commit or distribute the output (it contains game code). Usage: dump_camera_code.py <Spider-Man2.exe> <out.json>
"""
from __future__ import annotations
import bisect, hashlib, json, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import find_camera_refs as f

TARGETS = {"update_camera_ref_matrix": 0x1065680, "reference_setter": 0x3350220}
CAP = 12288


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: dump_camera_code.py <Spider-Man2.exe> <out.json>"); return 2
    exe, out = Path(sys.argv[1]), Path(sys.argv[2])
    d = exe.read_bytes(); sha = hashlib.sha256(d).hexdigest()
    if sha != f.EXE_SHA256:
        print(f"REFUSED: unexpected EXE SHA256 {sha}"); return 3
    secs, exc_rva, exc_size = f.sections(d)
    eo = f.rva_to_off(secs, exc_rva); funcs = []
    for k in range(exc_size // 12):
        b, e, u = struct.unpack_from("<III", d, eo + 12 * k); funcs.append((b, e))
    funcs.sort(); begins = [x[0] for x in funcs]
    result = {"schema": 1, "tool": "dump_camera_code", "exe_sha256": sha, "research_only": True, "functions": {}}
    for name, rva in TARGETS.items():
        i = bisect.bisect_right(begins, rva) - 1
        if i < 0 or not funcs[i][0] <= rva < funcs[i][1]:
            result["functions"][name] = {"rva": hex(rva), "error": "no_pdata"}; continue
        b, e = funcs[i]; n = min(e - b, CAP); off = f.rva_to_off(secs, b)
        result["functions"][name] = {"rva": hex(rva), "begin": hex(b), "end": hex(e), "dumped": n, "hex": d[off:off + n].hex()}
        print(f"{name}: 0x{b:x}-0x{e:x} ({e - b} bytes, dumped {n})")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(result, indent=1) + "\n", encoding="utf-8"); print(f"WROTE {out}"); return 0


if __name__ == "__main__":
    raise SystemExit(main())
