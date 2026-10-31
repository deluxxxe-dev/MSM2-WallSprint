"""Pinned original EXE, metadata-only candidate discovery. No code/values exported."""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

EXPECTED_SHA = 'c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9'
MAX_FILE = 512 * 1024 * 1024
MAX_METADATA = 192 * 1024 * 1024
MAX_NAMES = 128
MAX_REFS = 512
MAX_OUTPUT = 128 * 1024
# Search vocabulary, NOT asserted game symbols. Exact returned names remain candidates.
TERMS = ('Sprint', 'GroundLocomotion', 'GroundMovement', 'GroundMove', 'GroundRun',
         'Grounded', 'GroundState')

class Refusal(ValueError):
    pass


def pe_sections(data):
    def get(fmt, at):
        size = struct.calcsize(fmt)
        if at < 0 or at + size > len(data):
            raise Refusal('truncated_PE')
        return struct.unpack_from(fmt, data, at)[0]
    if len(data) < 64 or data[:2] != b'MZ':
        raise Refusal('invalid_DOS')
    pe = get('<I', 60)
    if data[pe:pe+4] != b'PE\0\0' or get('<H', pe+4) != 0x8664:
        raise Refusal('invalid_AMD64_PE')
    count, optional_size = get('<H', pe+6), get('<H', pe+20)
    op = pe + 24
    if not 0 < count <= 96 or optional_size < 240 or get('<H', op) != 0x20b:
        raise Refusal('invalid_PE32plus_sections')
    image_size, header_size = get('<I', op+56), get('<I', op+60)
    base = get('<Q', op+24)
    table_end = op+optional_size+40*count
    if not table_end <= header_size <= len(data) or not header_size <= image_size <= 2**31:
        raise Refusal('invalid_image_headers')
    if base + image_size > 2**64:
        raise Refusal('image_base_overflow')
    sections = []
    for i in range(count):
        at = op+optional_size+40*i
        vs, rva, size, raw, flags = struct.unpack_from('<IIII12xI', data, at+8)
        extent = max(vs, size)
        if not extent or rva < header_size or rva+extent > image_size:
            raise Refusal('invalid_virtual_range')
        if size and (raw < header_size or raw+size > len(data)):
            raise Refusal('invalid_raw_range')
        for s in sections:
            if rva < s['rva']+s['extent'] and s['rva'] < rva+extent:
                raise Refusal('overlapping_virtual_sections')
            if size and s['size'] and raw < s['raw']+s['size'] and s['raw'] < raw+size:
                raise Refusal('overlapping_raw_sections')
        sections.append(dict(rva=rva, raw=raw, size=size, extent=extent, flags=flags))
    return base, image_size, sections


def string_at(blob, hit, wide):
    step = 2 if wide else 1
    def char(at):
        if at < 0 or at+step > len(blob):
            return None
        if wide and blob[at+1] != 0:
            return None
        return blob[at]
    start = hit
    while char(start-step) is not None and 32 <= char(start-step) <= 126:
        start -= step
        if hit-start > 192*step:
            return None
    # Require actual NUL-delimited metadata, not arbitrary interior printable data.
    if start < step or blob[start-step:start] != b'\0'*step:
        return None
    end = start
    while char(end) is not None and 32 <= char(end) <= 126:
        end += step
        if end-start > 192*step:
            return None
    if not 0 < end-start or blob[end:end+step] != b'\0'*step:
        return None
    name = blob[start:end].decode('utf-16le' if wide else 'ascii')
    if not re.fullmatch(r'[A-Za-z0-9_?@$.:<> /()\-]+', name):
        return None
    return start, name


def discover(data, expected_sha=EXPECTED_SHA):
    """Test fixtures inject only their digest here; CLI cannot override build identity."""
    if len(data) > MAX_FILE:
        raise Refusal('file_cap')
    digest = hashlib.sha256(data).hexdigest()
    if digest != expected_sha:
        raise Refusal('unsupported_build_sha256')
    base, image_size, sections = pe_sections(data)
    # Readable, non-writable, non-executable metadata only, no text-section sweep.
    selected = [s for s in sections if s['flags'] & 0x40000000
                and not s['flags'] & 0xa0000000 and s['size']]
    if sum(s['size'] for s in selected) > MAX_METADATA:
        raise Refusal('metadata_scan_cap')
    anchors = {}
    for s in selected:
        blob = data[s['raw']:s['raw']+s['size']]
        for wide in (False, True):
            for term in TERMS:
                needle = term.encode('utf-16le' if wide else 'ascii')
                at = 0
                while True:
                    hit = blob.find(needle, at)
                    if hit < 0:
                        break
                    at = hit+len(needle)
                    found = string_at(blob, hit, wide)
                    if not found:
                        continue
                    start, name = found
                    rva = s['rva']+start
                    if rva in anchors:
                        continue
                    if len(anchors) >= MAX_NAMES:
                        raise Refusal('name_candidate_cap')
                    row = dict(name=name, name_rva=rva,
                               encoding='utf16le' if wide else 'ascii',
                               name_sha256=hashlib.sha256(name.encode()).hexdigest(),
                               type_descriptor_candidate_rva=None)
                    # MSVC x64 RTTI TypeDescriptor layout hypothesis only: 2 qwords then name.
                    if not wide and name.startswith(('.?AV', '.?AU')) and name.endswith('@@') and start >= 16:
                        ptr, spare = struct.unpack_from('<QQ', blob, start-16)
                        if spare == 0 and base <= ptr < base+image_size:
                            row['type_descriptor_candidate_rva'] = rva-16
                    anchors[rva] = row
    rows = sorted(anchors.values(), key=lambda r: r['name_rva'])
    targets = {r['name_rva'] for r in rows}
    targets.update(r['type_descriptor_candidate_rva'] for r in rows
                   if r['type_descriptor_candidate_rva'] is not None)
    refs = []
    for s in selected:
        blob = data[s['raw']:s['raw']+s['size']]
        for target in sorted(targets):
            for fmt, value, alignment, kind in (('<I', target, 4, 'rva32_word'),
                                               ('<Q', base+target, 8, 'va64_word')):
                needle = struct.pack(fmt, value)
                at = 0
                while True:
                    hit = blob.find(needle, at)
                    if hit < 0:
                        break
                    at = hit+1
                    if (s['rva']+hit) % alignment:
                        continue
                    if len(refs) >= MAX_REFS:
                        raise Refusal('metadata_reference_cap')
                    refs.append(dict(source_rva=s['rva']+hit, target_rva=target, kind=kind))
    result = dict(schema=1, result=('NO_ANCHOR_MATCH' if not rows else
                  'AMBIGUOUS_CANDIDATES_ONLY' if len(rows) > 1 else 'CANDIDATE_ONLY'),
                  exe_sha256=digest, build='2.810.0.0_SHA_pinned', image_base=base,
                  metadata_bytes_scanned=sum(s['size'] for s in selected),
                  anchors=rows, metadata_reference_candidates=refs,
                  unique_consumer_authenticated=False, selected_consumer=None,
                  grounded_support_frame='UNKNOWN', gravity_frame='UNKNOWN',
                  ownership_proven=False, code_evidence_exported=False,
                  live_reads=0, original_file_only=True, instruction_xrefs_scanned=False,
                  interpretation='name/RTTI-layout and aligned data-word candidates only; not registry or code-use proof')
    if len((json.dumps(result, indent=2)+"\n").encode()) > MAX_OUTPUT:
        raise Refusal('output_cap')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe', type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    try:
        with args.exe.open('rb') as f:
            data = f.read(MAX_FILE+1)
        result = discover(data)
    except (OSError, ValueError, struct.error) as e:
        # Fixed reason only; never exception strings containing raw input content.
        reason = str(e) if isinstance(e, Refusal) else 'input_unavailable_or_malformed'
        result = dict(schema=1, result='REFUSED', reason=reason, live_reads=0,
                      selected_consumer=None, ownership_proven=False)
    # Exclusive output creation: never clobber earlier acquisition or the EXE.
    with args.output.open('x', encoding='utf-8') as f:
        json.dump(result, f, indent=2)
        f.write('\n')
    print(result['result'], args.output)
    return 0 if result['result'] in ('CANDIDATE_ONLY', 'AMBIGUOUS_CANDIDATES_ONLY') else 2

if __name__ == '__main__':
    raise SystemExit(main())
