"""Minimal read-only walker for MSM2 DAT1 config field structs (WS-owned, no game code).

Layout inferred from owner-supplied vanilla configs (HYPOTHESIS until runtime-confirmed):
  [00 00 00 00] 44 00 15 03 | count:u32 | size:u32 | count*(hash:u32, flags:u32)
  | count*u32 (opaque) | packed data, one slot per field in entry order.
Field width: flags 0x08000010 -> 4 bytes (float), flags 0x10 / 0x04000010 -> 1 byte (u8 / i8).
Anything else is not treated as a field struct. Field NAMES are not decoded: callers
must pin identity by count/width pattern, known values and (optionally) hash equality.
"""
from __future__ import annotations
import struct
from dataclasses import dataclass

TYPE_TAG = bytes.fromhex("44001503")
FLAG_U8 = 0x10
FLAG_I8 = 0x04000010  # signed byte (seen in steering TurnDampFast/Slow)
FLAG_F32 = 0x08000010
MAX_FIELDS = 64


@dataclass(frozen=True)
class FieldStruct:
    start: int
    hashes: tuple[int, ...]
    flags: tuple[int, ...]
    data_offset: int
    offsets: tuple[int, ...]  # absolute file offset of each field's data

    @property
    def widths(self) -> tuple[int, ...]:
        return tuple(4 if f == FLAG_F32 else 1 for f in self.flags)


def walk(data: bytes) -> list[FieldStruct]:
    found: list[FieldStruct] = []
    pos = 0
    while True:
        i = data.find(TYPE_TAG, pos)
        if i < 0:
            return found
        pos = i + 1
        if i < 4 or i + 12 > len(data):
            continue
        count, size = struct.unpack_from("<II", data, i + 4)
        if not 0 < count <= MAX_FIELDS or i + 12 + size > len(data):
            continue
        fixed = 12 * count
        if size < fixed:
            continue
        entries = [struct.unpack_from("<II", data, i + 12 + 8 * k) for k in range(count)]
        flags = tuple(f for _, f in entries)
        if any(f not in (FLAG_U8, FLAG_I8, FLAG_F32) for f in flags):
            continue
        widths = [4 if f == FLAG_F32 else 1 for f in flags]
        data_off = i + 12 + fixed
        pad = size - fixed - sum(widths)
        if not 0 <= pad < 4:  # trailing alignment padding only
            continue
        offs, cur = [], data_off
        for w in widths:
            offs.append(cur)
            cur += w
        found.append(FieldStruct(i, tuple(h for h, _ in entries), flags, data_off, tuple(offs)))
    return found
