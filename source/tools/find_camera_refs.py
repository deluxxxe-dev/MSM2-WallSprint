#!/usr/bin/env python3
"""M5b research-only locator: where does the current Spider-Man2.exe contain the camera-manager code shapes that the
ActorRelativeCamera reference mod targets? Read-only, offline, no game launch, exact-EXE SHA pin.

The six byte shapes below are research hints transcribed from the reference mod's plaintext AOB strings (owner-supplied).
They are used ONLY in this offline tool to locate candidates for independent analysis; production WallSprint code must use
its own exact-build RVA + function-hash identity derived from the game, never these patterns.
Output: JSON with match RVAs, containing .pdata function bounds and a bounded hex window (<=160 bytes) per match for
cloud-side decoding. Do not commit the output (it contains small game code fragments).
"""
from __future__ import annotations
import hashlib, json, struct, sys
from pathlib import Path

EXE_SHA256 = "c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9"
HINTS = {
    "camera_ref_matrix_entry": "40 55 57 41 56 48 8D 6C 24 90 48 81 EC 70 01 00 00 8B 81 ?? ?? ?? ?? 48 8B F9 48 8D 8D 90 00 00 00 89 85 90 00 00 00 E8 ?? ?? ?? ?? 4C 8B F0 48 85 C0",
    "actor_relative_branch": "80 BF ?? ?? ?? ?? 00 48 8B 9C 24 ?? ?? ?? ?? 0F 84 ?? ?? ?? ?? 80 BF ?? ?? ?? ?? 00 75 ?? C7 87 ?? ?? ?? ?? 00 00 00 00",
    "set_new_reference_matrix": "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 48 81 EC E0 00 00 00 C5 FA 10 2A C4 C1 52 59 08 C5 F8 29 70 E8 41 8B E9",
    "reference_setter_call": "45 33 C9 48 8D 54 24 20 E9 ?? ?? ?? ?? 80 BF ?? ?? ?? ?? 00",
    "preserving_setter_call": "41 B9 03 00 00 00 4C 8D 44 24 30 49 8B CF E8 ?? ?? ?? ?? C5 F8 28 B4 24 ?? ?? ?? ??",
    "setter_preservation_flags": "40 F6 C5 01 74 ?? C5 F8 28 CE C4 C1 78 28 C4 E8 ?? ?? ?? ?? C5 FA 11 86 ?? ?? ?? ?? 40 F6 C5 02 74 ??",
}
MAX_MATCHES = 8
BEFORE, AFTER = 32, 128


def parse(p: str):
    toks = p.split()
    return [None if t == "??" else int(t, 16) for t in toks]


def sections(d: bytes):
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    assert d[pe:pe + 4] == b"PE\0\0", "not a PE"
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    optsz = struct.unpack_from("<H", d, pe + 20)[0]
    opt = pe + 24
    assert struct.unpack_from("<H", d, opt)[0] == 0x20B, "not PE32+"
    exc_rva, exc_size = struct.unpack_from("<II", d, opt + 112 + 3 * 8)
    out = []
    for i in range(nsec):
        o = opt + optsz + 40 * i
        name = d[o:o + 8].rstrip(b"\0").decode("latin1")
        vsize, va, rsize, raw = struct.unpack_from("<IIII", d, o + 8)
        chars = struct.unpack_from("<I", d, o + 36)[0]
        out.append((name, va, vsize, raw, rsize, chars))
    return out, exc_rva, exc_size


def rva_to_off(secs, rva):
    for _, va, vs, raw, rs, _ in secs:
        if va <= rva < va + max(vs, rs):
            return raw + (rva - va)
    return None


def find_all(buf: bytes, pat, base_rva: int, limit: int):
    first = next(i for i, b in enumerate(pat) if b is not None)
    anchor = bytes([pat[first]])
    hits, start = [], 0
    n = len(pat)
    while len(hits) <= limit:
        i = buf.find(anchor, start)
        if i < 0:
            break
        s = i - first
        start = i + 1
        if s < 0 or s + n > len(buf):
            continue
        if all(b is None or buf[s + k] == b for k, b in enumerate(pat)):
            hits.append(base_rva + s)
    return hits


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: find_camera_refs.py <Spider-Man2.exe> <out.json>")
        return 2
    exe, out = Path(sys.argv[1]), Path(sys.argv[2])
    d = exe.read_bytes()
    sha = hashlib.sha256(d).hexdigest()
    if sha != EXE_SHA256:
        print(f"REFUSED: unexpected EXE SHA256 {sha}")
        return 3
    secs, exc_rva, exc_size = sections(d)
    funcs = []
    eo = rva_to_off(secs, exc_rva)
    if eo is not None:
        for k in range(exc_size // 12):
            b, e, u = struct.unpack_from("<III", d, eo + 12 * k)
            funcs.append((b, e))
    funcs.sort()
    import bisect
    begins = [f[0] for f in funcs]

    def containing(rva):
        i = bisect.bisect_right(begins, rva) - 1
        if i >= 0 and funcs[i][0] <= rva < funcs[i][1]:
            return {"begin": funcs[i][0], "end": funcs[i][1]}
        return None

    result = {"schema": 1, "tool": "find_camera_refs", "exe_sha256": sha, "research_only": True, "hints": {}}
    for name, text in HINTS.items():
        pat = parse(text)
        hits = []
        for sname, va, vs, raw, rs, chars in secs:
            if not chars & 0x20000000:  # executable
                continue
            hits += find_all(d[raw:raw + rs], pat, va, MAX_MATCHES)
        rows = []
        for rva in hits[:MAX_MATCHES]:
            off = rva_to_off(secs, rva)
            w0 = max(0, off - BEFORE)
            rows.append({"rva": rva, "rva_hex": hex(rva), "function": containing(rva),
                         "window_rva": rva - (off - w0), "window_hex": d[w0:off + AFTER].hex()})
        result["hints"][name] = {"match_count": len(hits), "truncated": len(hits) > MAX_MATCHES, "matches": rows}
        print(f"{name}: {len(hits)} match(es)")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(result, indent=1) + "\n", encoding="utf-8")
    print(f"WROTE {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
