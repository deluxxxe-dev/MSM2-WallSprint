from pathlib import Path
import json,tempfile,contextlib,io
from static_access_v2 import analyze,generate
root=Path(__file__).resolve().parents[1]
def decode(s):return analyze(bytes.fromhex(s),0x1000,'test')
# mov rdi,rcx; vmovss xmm0,[rdi+0x298]; call; vmovss [rdi+0x304],xmm0; ret
r=decode('48 8b f9 c5 fa 10 87 98 02 00 00 e8 00 00 00 00 c5 fa 11 87 04 03 00 00 c3')
assert [(a['offset'],a['operation']) for a in r['accesses']]==[(0x298,'READ'),(0x304,'WRITE')]
# Partial overwrite must destroy this alias; volatile RCX is killed by call.
assert not decode('48 8b f9 40 b7 00 c5 fa 10 87 98 02 00 00 c3')['accesses']
assert not decode('e8 00 00 00 00 c5 fa 10 81 98 02 00 00 c3')['accesses']
# Conditional path destroys RDI; intersection must not invent alias after merge.
r=decode('48 8b f9 85 c0 74 02 31 ff c5 fa 10 87 98 02 00 00 c3');assert not r['accesses']
# Child provenance only one level; never label grandchild offsets as child memory.
r=decode('48 8b 81 28 01 00 00 48 8b 58 60 48 8b 53 10 c3');assert len(r['accesses'])==2 and r['accesses'][1]['parent_field']==0x128
# Real captures: end-to-end decode and generated header must remain reproducible.
if (root/'tools/fixtures/native_v01a7/Entry.bin').exists():
    with tempfile.TemporaryDirectory() as td,contextlib.redirect_stdout(io.StringIO()):
        p=Path(td);r=generate(root/'tools/fixtures/native_v01a7',p,p/'plan.hpp')
        assert all(f['decoded_instruction_coverage']==1 for f in r['functions'])
        assert next(f for f in r['functions'] if f['name']=='Entry')['decoded_instructions']==744
        assert (p/'plan.hpp').read_bytes()==(root/'include/static_observation_plan.hpp').read_bytes()
        access={a['instruction_rva']:a for f in r['functions'] for a in f['accesses']}
        assert access[0x142385a]['operation']=='WRITE' and access[0x142385a]['offset']==0x2a4
        assert access[0x1424406]['operation']=='WRITE' and access[0x1424406]['width']==32
        assert 0x298 in r['selected_root_offsets'] and 0x240 in r['selected_root_offsets']
        assert all(abs(a-b)>=12 for a in r['selected_root_offsets'] for b in r['selected_root_offsets'] if a!=b)
else:print("SKIP: private native replay files not distributed; synthetic AVX/CFG tests above executed")
print('PASS: AVX load/store direction, alias move/partial-write/call clobber/CFG intersection, one-level provenance, private replay if present, independent synthetic invariants')
