"""Read-only exact-EXE check of recovered reference call relations; no ABI certificate."""
import argparse
from bisect import bisect_right
import hashlib
import json
from pathlib import Path
import struct
import capstone as cs
from capstone import x86 as x
import discover_ground_support as pe

TARGETS = {'UpdateMotion': 0x14439d0, 'RequestSimpleMotion': 0x11a1cd0,
           'GravityOrientationOwner': 0x134f000, 'SetGravityVector': 0x3510ea0}
RELATIONS = (('UpdateMotion',0x4e6,'RequestSimpleMotion'),
             ('GravityOrientationOwner',0x67,'SetGravityVector'))
# Hashes of minimal comparison markers independently recovered from reference initializer.
# No raw game/reference function bytes are stored or exported.
MARKERS = [
  [
    "RequestSimpleMotion",
    0,
    7,
    "4409ca3689d39165729542bf9fc76b56ffe59d6e9b79d2ff1082a81be2ba2c37"
  ],
  [
    "RequestSimpleMotion",
    116,
    3,
    "aeb97bf20ac71ce814203f1e75e1d486a6ca18be7047deb90e128941ab43fd4b"
  ],
  [
    "RequestSimpleMotion",
    162,
    3,
    "4a1b59f70929084055d7e3e9d819460ca1f53c63be9a1874d7bad6cfd3001fa3"
  ],
  [
    "SetGravityVector",
    0,
    4,
    "e1e677d1f6937c82562fdff934ba24ac374d4ef7ab57944605b232787ca87834"
  ],
  [
    "SetGravityVector",
    12,
    5,
    "c3fa7ac37d8804c5d97158c0ce6a728cddc84be8f45a3f153edba83627381c99"
  ],
  [
    "SetGravityVector",
    23,
    9,
    "b78f85921ac27365b85d546a1e794806ddd5ae890a4f37986d55ca5d14f69f4a"
  ],
  [
    "SetGravityVector",
    78,
    1,
    "ae3f4619b0413d70d3004b9131c3752153074e45725be13b9a148978895e359e"
  ]
]

def verify(data, *, expected_sha=pe.EXPECTED_SHA, targets=TARGETS):
    if len(data)>pe.MAX_FILE:raise pe.Refusal('file_cap')
    digest=hashlib.sha256(data).hexdigest()
    if digest!=expected_sha:raise pe.Refusal('unsupported_build_sha256')
    if cs.__version__!='5.0.6':raise pe.Refusal('requires_Capstone5.0.6')
    _,image_size,sections=pe.pe_sections(data)
    md=cs.Cs(cs.CS_ARCH_X86,cs.CS_MODE_64);md.detail=True
    def read(rva,n,code=False):
        found=[s for s in sections if s['rva']<=rva and rva+n<=s['rva']+s['size']]
        if len(found)!=1 or (code and (found[0]['flags']&0x60000000!=0x60000000 or found[0]['flags']&0x80000000)):raise pe.Refusal('range_not_file_backed_executable' if code else 'range_not_file_backed')
        s=found[0];at=s['raw']+rva-s['rva'];return data[at:at+n]
    op=struct.unpack_from('<I',data,60)[0]+24
    if struct.unpack_from('<I',data,op+108)[0]<4:raise pe.Refusal('runtime_directory_missing')
    table,size=struct.unpack_from('<II',data,op+112+24)
    if size%12 or size>8*1024*1024 or size//12>600000:raise pe.Refusal('runtime_table_cap_or_alignment')
    entries=list(struct.iter_unpack('<III',read(table,size))) if size else []
    end=0
    for b,e,u in entries:
        if not 0<b<e<=image_size or b<end or not 0<u<image_size or u%4:raise pe.Refusal('invalid_runtime_ranges')
        end=e
    starts=[a[0] for a in entries]
    identities=[]
    for name,rva in targets.items():
        code=read(rva,16,True)
        j=bisect_right(starts,rva)-1
        row=entries[j] if j>=0 and entries[j][0]<=rva<entries[j][1] else None
        identities.append(dict(name=name,rva=rva,entry16_sha256=hashlib.sha256(code).hexdigest(),
                               runtime=list(row) if row else None,
                               exact_runtime_entry=bool(row and row[0]==rva),
                               status='PE_entry_range_only' if row and row[0]==rva else 'method_boundary_UNPROVEN'))
    checks=[]
    for name,offset,n,expected in MARKERS:
        actual=hashlib.sha256(read(targets[name]+offset,n,True)).hexdigest()
        checks.append(dict(role=name,offset=offset,bytes=n,marker_hash_matches=actual==expected))
    relations=[]
    for owner,offset,target in RELATIONS:
        site=targets[owner]+offset;code=read(site,5,True)
        ins=next(md.disasm(code,site,count=1),None)
        valid=bool(ins and ins.size==5 and ins.id==x.X86_INS_CALL and ins.operands[0].type==x.X86_OP_IMM)
        decoded=ins.operands[0].imm if valid else None
        owner_identity=next(i for i in identities if i['name']==owner)
        row=owner_identity['runtime']
        contained=bool(row[0]==targets[owner] and row[0]<=site and site+5<=row[1]) if row else None
        relations.append(dict(owner=owner,site_rva=site,target_role=target,
                              decoded_call=valid,target_rva=decoded,
                              matches_recovered_relation=valid and decoded==targets[target] and contained is not False,
                              owner_runtime_contains_call=contained,
                              boundary='fixed_reference_offset_plus_decode_not_CFG_proof'))
    matches=all(c['marker_hash_matches'] for c in checks) and all(c['matches_recovered_relation'] for c in relations)
    boundaries=all(i['exact_runtime_entry'] for i in identities)
    return dict(schema=1,result=('REFERENCE_RELATIONS_MATCH' if boundaries else 'RELATIONS_MATCH_BOUNDARY_PARTIAL') if matches else 'REFERENCE_RELATIONS_MISMATCH',
                exe_sha256=digest,reference_sha256='7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662',
                selection='four_reference_canonical_RVAs_only_no_shift_or_scan',identities=identities,
                markers=checks,relations=relations,relations_match=matches,
                callable_ABI_proven=False,lifetime_proven=False,ground_contact_proven=False,
                live_reads=0,code_exported=False,production_permission=False)


def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',required=True,type=Path)
    args=p.parse_args()
    try:
        with args.exe.open('rb') as f:result=verify(f.read(pe.MAX_FILE+1))
    except (OSError,ValueError,struct.error) as e:
        result=dict(schema=1,result='REFUSED',reason=str(e) if isinstance(e,pe.Refusal) else 'malformed_or_unavailable',production_permission=False)
    encoded=json.dumps(result,indent=2)+'\n'
    if len(encoded.encode())>32768:raise pe.Refusal('output_cap')
    with args.output.open('x',encoding='utf-8') as f:f.write(encoded)
    print(result['result'],args.output)
    return 0 if result.get('relations_match') else 2

if __name__=='__main__':raise SystemExit(main())
