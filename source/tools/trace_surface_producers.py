"""Targeted xrefs/local backward definitions, not a generic decompiler.
Static-only: never execute a captured instruction or call a native target.
"""
from pathlib import Path
import argparse,hashlib,json
from capstone import Cs,CS_ARCH_X86,CS_MODE_64,CS_GRP_CALL,CS_GRP_JUMP
from capstone.x86 import X86_OP_REG,X86_OP_MEM
from static_access_v2 import analyze,CANON
TARGETS=(0x320,0x338,0x398)
def canon(name):return 'xmm'+name[3:] if name.startswith(('ymm','zmm')) else CANON.get(name,name)
def written(ins,name):
    if not ins.operands or ins.operands[0].type!=X86_OP_REG:return False
    if ins.mnemonic.startswith(('cmp','test','vcom','com','ucom','vucom')):return False
    return canon(ins.reg_name(ins.operands[0].reg))==canon(name)
def run(input_dir,output_dir):
    meta=json.loads((input_dir/'SURFACE_SOURCE_REVIEW.json').read_text());md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True;result=[];capture=[]
    for f in meta['function_captures']:
        raw=(input_dir/(f['name']+'.bin')).read_bytes();assert len(raw)==f['size'] and hashlib.sha256(raw).hexdigest()==f['sha256']
        rva=int(f['rva'],16);ins=list(md.disasm(raw,rva));analysis=analyze(raw,rva,f['name']);idx={x.address:i for i,x in enumerate(ins)};capture.append({**f,'external_branches':analysis['outside_span_branches']})
        for row in analysis['accesses']:
            if row['scope']!='this':continue
            fields=[x for x in TARGETS if row['offset']<x+12 and row['offset']+row['width']>x]
            if not fields:continue
            i=idx[row['instruction_rva']];current=ins[i];priorcalls=[x for x in ins[:i] if x.group(CS_GRP_CALL)];defs=[]
            if row['operation']=='WRITE':
                regs=[current.reg_name(o.reg) for o in current.operands[1:] if o.type==X86_OP_REG]
                for register in regs:
                    definition=None;barrier=None
                    for prev in reversed(ins[max(0,i-80):i]):
                        if written(prev,register):definition=prev;break
                        if prev.group(CS_GRP_CALL):barrier=hex(prev.address);break
                    defs.append({'register':register,'local_definition_rva':hex(definition.address) if definition else None,'local_definition':definition.mnemonic+' '+definition.op_str if definition else 'unknown across local slice / call barrier','source_memory':[definition.op_str] if definition and any(x.type==X86_OP_MEM for x in definition.operands) else [],'stopped_at_call':barrier,'control_flow_paths_proven':False})
            result.append({'fields':[hex(x) for x in fields],**row,'nearest_preceding_call_rva':hex(priorcalls[-1].address) if priorcalls else None,'source_definitions':defs,'bounded_context':[hex(x.address)+': '+x.mnemonic+' '+x.op_str for x in ins[max(0,i-8):i+1]]})
    output_dir.mkdir(parents=True,exist_ok=True);(output_dir/'SURFACE_FIELD_PRODUCERS.json').write_text(json.dumps({'capture_scope':capture,'xrefs':result},indent=2)+'\n')
    text='''# Targeted producers: this+0x320 / +0x338 / +0x398

Capstone5.0.6 on checksum-validated native captures. READ/WRITE below describes the **original game**, not MOD writes. Three12-byte ranges only; aliases from the existing static map are used. Local backward definitions are bounded instruction-level evidence; nearest preceding calls are address proximity, never asserted producers by proximity alone. No unknown native call/hook or memory-range expansion.

|Field|Function|Instruction RVA|R/W|Instruction / source register|Source memory / local definition|Nearest preceding call|
|---|---|---|---|---|---|---|
'''
    for row in result:
        defs='; '.join(f"{d['register']} ← {d['local_definition']} @ {d['local_definition_rva']}" for d in row['source_definitions']) or 'read/address; see source instruction'
        text+=f"|{', '.join(row['fields'])}|{row['function']}|{hex(row['instruction_rva'])}|{row['operation']}/{row['width']}|`{row['instruction']}`|{defs}|{row['nearest_preceding_call_rva']}|\n"
    text+='''
## Confirmed local chains versus hypotheses

- Entry0x142361c sets R14=entry RDX; 0x142361f sets RDI=this. At0x14236dd..0x1423707 the same incoming argument triple `[R14+0x2c..+0x34]` is copied as8+4 bytes into both0x320 and0x338. This is a confirmed **initial copy**; argument type/coordinate semantics are not authenticated.
- P338 has no further direct producer in this captured Entry span. This does not prove it is constant at runtime: other native callees/update chunks can modify the owner. Its current/previous/raw meaning remains unverified.
- N398 at0x1423f20/+0x1423f28 receives XMM0/EAX, the same values just written to0x350 at0x1423ef3/+0x1423efe and0x3e0 at0x1423f12/+0x1423f1a. XMM0/EAX originate in the stack triple `[rsp+0x70..+0x78]` via RCX, not an immediately preceding getter return.
- That stack triple has branch-dependent producers: one path transforms this+0x35c using basis-like this+0x184/+0x194/+0x1a4; a fallback path at0x1423e70..0x1423eee loads a nested/default vector and transforms it with the same basis. 0x35c is built by XZ normalization of0x374 at0x1423d9f..0x1423e08. This establishes mathematical dependence/local copy shape, not a collision-normal getter identity. No nested pointer is followed by the MOD.
- A later P320 write at0x14241c7/+0x14241cf/+0x14241d7 stores XMM2/7/4. Local arithmetic at0x142416b..0x14241c3 subtracts this+0x174/+178/+17c from an adjusted this+0x32c triple then multiplies by the same basis-like fields. The source32c is adjusted along35c after the **unknown** call0x29bd5f0 at0x1424085; the immediate RAX loads at0x142408a/+0x1424095/+0x14240ca are native return-data uses. We do not call it an authenticated Hero getter. Output of this pipeline is copied from P320 to0x3c8 and32c to0x3d4 immediately afterward.
- Captured Update is only its422-byte RUNTIME_FUNCTION span and has **no direct xref** to the three primary ranges. It branches to0x14248f9 outside the capture. Calls and that cold chunk may write these fields. No complete per-frame producer or historical/current/filtered label is claimed.

N398's Entry initialization shares350/3e0 values, but live data need not remain identical. Use SURFACE_DUPLICATE_FIELD_MAP.md for full-sample equality and lag evidence. Numerical surface-pair evidence is strong, while engine-internal field names remain hypotheses. Plane-reference validation does not require proving a true physics collision contact.
'''
    (output_dir/'SURFACE_FIELD_PRODUCERS.md').write_text(text);print('PASS:',len(result),'targeted primary-field xrefs; source/call/local-slice limits explicit')
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('input_dir',type=Path);p.add_argument('--output-dir',type=Path,required=True);a=p.parse_args();run(a.input_dir,a.output_dir)
