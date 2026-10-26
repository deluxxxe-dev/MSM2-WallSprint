"""Bounded OFFLINE exact-slot reference candidates; never reads a live qword.

Only the pinned exe is accepted by the CLI. Decoding file bytes is not proof of
all instruction entries or indirect writers. Output never grants base/lifetime.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
import capstone as cs
from capstone.x86 import X86_OP_MEM, X86_OP_REG, X86_REG_RIP, X86_REG_INVALID

EXPECTED_SHA = 'c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9'
SLOT = 0xC2D2580
DEFERRED = 0x351EB00
MAX_FILE = 512 * 1024 * 1024
MAX_SCAN = 64 * 1024 * 1024
MAX_INSNS = 5_000_000
MAX_REFS = 256

def scan_code(code, rva, slot=SLOT, image_base=0x140000000,
              max_bytes=MAX_SCAN, max_insns=MAX_INSNS, max_refs=MAX_REFS):
    """Owned-buffer or authenticated-file candidate sweep, no calls followed."""
    md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_64); md.detail = True
    result = dict(bytes_scanned=0, instructions=0, decode_gaps=0, capped=False,
                  references=[], unresolved_memory_operands=0)
    cursor = 0
    limit = min(len(code), max_bytes)
    while cursor < limit and result['instructions'] < max_insns:
        start = cursor
        for ins in md.disasm(code[cursor:min(limit, cursor + 65536 + 15)], rva + cursor):
            if cursor - start >= 65536: break
            result['instructions'] += 1
            for index,op in enumerate(ins.operands):
                if op.type != X86_OP_MEM: continue
                if op.mem.segment != X86_REG_INVALID or op.mem.index != X86_REG_INVALID:
                    result['unresolved_memory_operands'] += 1; continue
                if op.mem.base == X86_REG_RIP:
                    address = ins.address + ins.size + op.mem.disp
                elif op.mem.base == X86_REG_INVALID and ins.addr_size == 8:
                    address = op.mem.disp - image_base
                else:
                    result['unresolved_memory_operands'] += 1; continue
                width = op.size
                if width and address < slot + 8 and slot < address + width:
                    if len(result['references']) >= max_refs:
                        result['capped'] = True; result['bytes_scanned'] = ins.address+ins.size-rva; return result
                    access = op.access
                    # Store caveat: absent/incorrect decoder access never proves a reader.
                    source = ins.operands[-1] if ins.operands else None
                    kind = 'register' if source is not None and source.type == X86_OP_REG else 'immediate_or_other'
                    result['references'].append(dict(site_rva=ins.address, width=width,
                        decoded_access=access, may_write=bool(access & cs.CS_AC_WRITE) or not access or (index==0 and ins.id not in (cs.x86.X86_INS_CMP,cs.x86.X86_INS_TEST,cs.x86.X86_INS_LEA)),
                        source_kind=kind, boundary='linear_sweep_candidate_not_execution_proof'))
            cursor = ins.address + ins.size - rva
            if result['instructions'] >= max_insns: break
        if cursor == start:
            cursor += 1; result['decode_gaps'] += 1
    result['bytes_scanned'] = cursor
    result['capped'] |= cursor < len(code)
    return result

def publication_verdict(scans, coverage_complete=False):
    refs = [x for scan in scans for x in scan['references']]
    writers = [x for x in refs if x['may_write']]
    reason = ('multiple_direct_writer_candidates_ambiguous' if len(writers) > 1 else
              'indirect_writers_and_publication_lifetime_unproven' if coverage_complete else
              'xref_coverage_incomplete_indirect_writers_unexcluded')
    return dict(slot_rva=SLOT, reference_candidates=refs,
        direct_writer_candidates=len(writers), candidate_sweep_complete=coverage_complete,
        writer_enumeration_complete=False, publication_source_kind='unproven',
        allocation_proven=False, range_proven=False, lifetime_proven=False,
        base_proven=False, destination_contained=False, CFG_continuation=False,
        live_qword_reads=0, pointer_chase=False, reason=reason)

def parse_pe(data):
    def u16(at): return struct.unpack_from('<H', data, at)[0]
    def u32(at): return struct.unpack_from('<I', data, at)[0]
    if len(data) < 64 or data[:2] != b'MZ': raise ValueError('invalid DOS header')
    pe = u32(60)
    if pe > len(data)-264 or data[pe:pe+4] != b'PE\0\0' or u16(pe+4) != 0x8664:
        raise ValueError('invalid x64 PE')
    optional = pe+24; count=u16(pe+6); size=u16(pe+20)
    if not 0 < count <= 96 or size < 240 or u16(optional) != 0x20b:
        raise ValueError('invalid sections/PE32+')
    image_size=u32(optional+56); image_base=struct.unpack_from('<Q',data,optional+24)[0]
    sections=[]
    for i in range(count):
        at=optional+size+40*i
        if at+40 > len(data): raise ValueError('truncated sections')
        virtual=u32(at+8); begin=u32(at+12); raw_size=u32(at+16); raw=u32(at+20)
        if begin > image_size or virtual > image_size-begin or raw > len(data) or raw_size > len(data)-raw:
            raise ValueError('section outside image/file')
        end=begin+virtual
        if any(begin < e and b < end for b,e,_,_,_ in sections): raise ValueError('overlapping sections')
        sections.append((begin,end,raw,raw_size,u32(at+36)))
    def file_range(rva,n):
        for begin,end,raw,raw_size,_ in sections:
            if begin <= rva and n <= end-rva and rva-begin+n <= raw_size:
                return data[raw+rva-begin:raw+rva-begin+n]
        raise ValueError('metadata range not file backed')
    excluded=[]
    table=u32(optional+112+3*8); table_size=u32(optional+116+3*8)
    if bool(table) != bool(table_size) or table_size%12 or table_size > 8*1024*1024 or table_size//12 > 600000:
        raise ValueError('runtime table invalid/capped')
    rows=file_range(table,table_size) if table_size else b''
    previous=0
    for at in range(0,len(rows),12):
        begin,end,unwind=struct.unpack_from('<III',rows,at)
        if begin < previous or begin >= end or end > image_size or not 0 < unwind < image_size:
            raise ValueError('runtime table malformed')
        previous=end
        if begin <= DEFERRED < end: excluded.append((begin,end))
    if not excluded:
        excluded=[(b,e) for b,e,_,_,flags in sections if b <= DEFERRED < e and flags & 0x20000000]
    return sections,image_base,excluded,file_range

def analyze_image(data):
    if len(data) > MAX_FILE: raise ValueError('input file cap')
    if hashlib.sha256(data).hexdigest() != EXPECTED_SHA: raise ValueError('exact executable SHA256 mismatch')
    if cs.__version__ != '5.0.6': raise ValueError('requires pinned Capstone5.0.6')
    sections,base,excluded,file_range=parse_pe(data)
    slot_sections=[s for s in sections if s[0] <= SLOT and SLOT+8 <= s[1]]
    if len(slot_sections)!=1: raise ValueError('slot section not unique')
    scans=[]; budget=MAX_SCAN; insns=MAX_INSNS; refs=MAX_REFS
    for begin,end,raw,raw_size,flags in sections:
        if not flags & 0x20000000: continue
        spans=[(begin,min(end,begin+raw_size))]
        for low,high in excluded:
            spans=[part for b,e in spans for part in [(b,min(e,low)),(max(b,high),e)] if part[0]<part[1]]
        for low,high in spans:
            if budget <= 0 or insns <= 0 or refs <= 0: break
            scan=scan_code(data[raw+low-begin:raw+high-begin],low,image_base=base,
                           max_bytes=budget,max_insns=insns,max_refs=refs)
            scans.append(scan);budget-=scan['bytes_scanned'];insns-=scan['instructions'];refs-=len(scan['references'])
    complete=not excluded and budget>0 and insns>0 and refs>0 and all(not s['capped'] and not s['decode_gaps'] for s in scans)
    result=publication_verdict(scans,complete)
    result.update(image_authenticated=True, scan_scope='bounded_offline_direct_candidates_not_all_writes',
        slot_metadata=dict(readable=bool(slot_sections[0][4]&0x40000000),writable=bool(slot_sections[0][4]&0x80000000)),
        excluded_deferred_regions=excluded, cap_bytes=MAX_SCAN, cap_instructions=MAX_INSNS,
        bytes_scanned=sum(s['bytes_scanned'] for s in scans), decode_gaps=sum(s['decode_gaps'] for s in scans),
        indirect_or_unresolved_operands=sum(s['unresolved_memory_operands'] for s in scans))
    return result

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('exe',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    if args.exe.stat().st_size > MAX_FILE: parser.error('input file cap')
    try: result=analyze_image(args.exe.read_bytes())
    except (ValueError,struct.error) as exc: parser.error(str(exc))
    args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print('PASS: authenticated exact-slot candidate metadata written; base/containment remain unproven')
if __name__ == '__main__': main()
