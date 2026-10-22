"""Offline AVX-capable decoder. Input: checksum-verified bounded function captures.
Provenance is a conservative CFG must-analysis, not C++ type recovery.
Generated runtime observation plan needs all four native-body hashes to match.
"""
from pathlib import Path
import argparse, collections, hashlib, json
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_GRP_JUMP, CS_GRP_CALL, CS_GRP_RET, CS_AC_READ, CS_AC_WRITE
from capstone.x86 import X86_OP_REG, X86_OP_IMM, X86_OP_MEM
VERSION='capstone 5.0.6'
VOLATILE={'rax','rcx','rdx','r8','r9','r10','r11'}
FAMILIES=[('rax','eax','ax','al','ah'),('rbx','ebx','bx','bl','bh'),('rcx','ecx','cx','cl','ch'),('rdx','edx','dx','dl','dh'),('rsi','esi','si','sil'),('rdi','edi','di','dil'),('rbp','ebp','bp','bpl'),('rsp','esp','sp','spl')]+[(f'r{i}',f'r{i}d',f'r{i}w',f'r{i}b') for i in range(8,16)]
CANON={n:g[0] for g in FAMILIES for n in g}
def reg(ins,n):return CANON.get(ins.reg_name(n),ins.reg_name(n))
def origin(ins,mem,state):
    if mem.index:return None
    base=state.get(reg(ins,mem.base))
    if base:return (base[0],base[1],base[2]+mem.disp)
    return None

def analyze(raw,rva,name):
    md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
    instructions=list(md.disasm(raw,rva)); decoded=sum(i.size for i in instructions)
    if decoded!=len(raw):raise ValueError(f'{name}: undecodable byte at {rva+decoded:x}; no byte skipping permitted')
    by={i.address:i for i in instructions}; states={rva:{'rcx':('this',0,0),'rdx':('arg2',0,0),'rsp':('stack',0,0)}}; todo=collections.deque([rva]); outside=set(); iterations=0
    def transfer(ins,state):
        nxt=state.copy(); ops=ins.operands; assignment=None
        if ins.mnemonic=='mov' and len(ops)==2 and ops[0].type==X86_OP_REG and ops[0].size==8:
            if ops[1].type==X86_OP_REG:assignment=state.get(reg(ins,ops[1].reg))
            elif ops[1].type==X86_OP_MEM:
                o=origin(ins,ops[1].mem,state)
                if o and o[0]=='this':assignment=('child',o[2],0)
        if ins.mnemonic=='lea' and len(ops)==2 and ops[0].type==X86_OP_REG and ops[0].size==8 and ops[1].type==X86_OP_MEM:assignment=origin(ins,ops[1].mem,state)
        for rid in ins.regs_access()[1]:nxt.pop(reg(ins,rid),None)
        if assignment:nxt[reg(ins,ops[0].reg)]=assignment
        if ins.group(CS_GRP_CALL):
            for r in VOLATILE:nxt.pop(r,None)
        # Stack derived only for argument/output-buffer proximity, never followed.
        if 'rsp' in state and (ins.group(CS_GRP_CALL) or ins.mnemonic in ('push','pop')):
            o=state['rsp'];nxt['rsp']=(o[0],o[1],o[2]+(-8 if ins.mnemonic=='push' else 8 if ins.mnemonic=='pop' else 0))
        if ins.mnemonic in ('sub','add') and len(ops)==2 and ops[0].type==X86_OP_REG and reg(ins,ops[0].reg)=='rsp' and ops[1].type==X86_OP_IMM and 'rsp' in state:
            o=state['rsp'];nxt['rsp']=(o[0],o[1],o[2]+ops[1].imm*(1 if ins.mnemonic=='add' else -1))
        return nxt
    while todo:
        pc=todo.popleft();ins=by[pc];state=states[pc];nxt=transfer(ins,state);iterations+=1
        if iterations>100000:raise ValueError('CFG did not converge')
        successors=[]
        if ins.group(CS_GRP_JUMP):
            if ins.operands[0].type==X86_OP_IMM:successors.append(ins.operands[0].imm)
            if ins.mnemonic!='jmp':successors.append(pc+ins.size)
        elif not ins.group(CS_GRP_RET):successors.append(pc+ins.size)
        for target in successors:
            if target not in by:
                if rva<=target<rva+len(raw):raise ValueError('branch into middle of decoded instruction')
                if target!=rva+len(raw):outside.add(target)
                continue
            old=states.get(target)
            merged=nxt.copy() if old is None else {r:v for r,v in old.items() if nxt.get(r)==v}
            if old!=merged:states[target]=merged;todo.append(target)
    accesses=[];calls=[]
    for idx,ins in enumerate(instructions):
        if ins.address not in states:continue
        state=states[ins.address]
        for operand_index,op in enumerate(ins.operands):
            if op.type!=X86_OP_MEM:continue
            o=origin(ins,op.mem,state)
            if not o or o[0] not in ('this','child'):continue
            address_only=ins.mnemonic=='lea'
            operation='ADDRESS' if address_only else ('READ_WRITE' if op.access==(CS_AC_READ|CS_AC_WRITE) else 'WRITE' if op.access&CS_AC_WRITE else 'READ' if op.access&CS_AC_READ else 'UNKNOWN')
            # Capstone 5 AVX vmov store access flags can incorrectly say READ.
            # Intel move operand direction is unambiguous; normalize explicitly.
            if ins.mnemonic.startswith(('vmov','mov')) and not address_only:
                operation='WRITE' if operand_index==0 else 'READ'
            simd=any(ins.reg_name(x.reg).startswith(('xmm','ymm','zmm')) for x in ins.operands if x.type==X86_OP_REG)
            accesses.append(dict(function=name,instruction_rva=ins.address,scope=o[0],parent_field=o[1],offset=o[2],operation=operation,width=op.size,this_alias=ins.reg_name(op.mem.base),register_source=ins.op_str,simd=simd,possible_type='address_only' if address_only else 'float_or_vector' if simd else 'pointer_or_uint64' if op.size==8 else 'scalar_or_unknown',instruction=ins.mnemonic+' '+ins.op_str,instruction_index=idx))
        if ins.group(CS_GRP_CALL):
            calls.append(dict(function=name,instruction_rva=ins.address,target_rva=ins.operands[0].imm if ins.operands[0].type==X86_OP_IMM else None,instruction=ins.mnemonic+' '+ins.op_str,arguments={r:list(state[r]) for r in ('rcx','rdx','r8','r9') if r in state},instruction_index=idx))
    for row in accesses:
        near=sorted(calls,key=lambda c:abs(row['instruction_index']-c['instruction_index']))[:1]
        row['nearest_call_rva']=near[0]['instruction_rva'] if near else None
        row['call_instruction_distance']=row['instruction_index']-near[0]['instruction_index'] if near else None
    for c in calls:
        near=[a for a in accesses if abs(a['instruction_index']-c['instruction_index'])<=12]
        c['fields_before']=[{k:a[k] for k in ('scope','parent_field','offset','operation','width','instruction_rva')} for a in near if a['instruction_index']<c['instruction_index']]
        c['fields_after']=[{k:a[k] for k in ('scope','parent_field','offset','operation','width','instruction_rva')} for a in near if a['instruction_index']>c['instruction_index']]
        c['followed_by_vector_store']=any(a['operation']=='WRITE' and a['simd'] for a in near if 0<a['instruction_index']-c['instruction_index']<=12)
        c['output_buffer_candidate']=any(o[0] in ('stack','this') for r,o in c['arguments'].items() if r in ('rdx','r8','r9'))
        c['uses_0x128_child']=any(o[0]=='child' and o[1]==0x128 for o in c['arguments'].values()) or any(a['scope']=='child' and a['parent_field']==0x128 for a in near)
    return dict(name=name,rva=rva,size=len(raw),sha256=hashlib.sha256(raw).hexdigest(),decoded_instructions=len(instructions),decoded_bytes=decoded,decoded_instruction_coverage=decoded/len(raw),cfg_reachable_instructions=len(states),cfg_iterations=iterations,outside_span_branches=sorted(outside),accesses=accesses,calls=calls)

def vector_fields(functions):
    # A split 8+4 byte copy, a SIMD >=12 byte operand, or three adjacent
    # scalar-float operations in a short instruction neighborhood supply boundaries.
    # Never slide every 4 bytes across the object.
    candidates={}
    for f in functions:
        rows=[a for a in f['accesses'] if a['scope']=='this' and 0<=a['offset']<0x480 and a['operation'] in ('READ','WRITE','READ_WRITE')]
        for a in rows:
            reason=None;support=[a]
            if a['simd'] and a['width']>=12:reason='SIMD_12_or_16byte_operand'
            if a['simd'] and a['width']==8:
                bs=[b for b in rows if b['offset']==a['offset']+8 and b['width']==4 and b['operation']==a['operation'] and abs(b['instruction_index']-a['instruction_index'])<=5]
                if bs:reason='SIMD8_plus_adjacent4_copy12';support+=[bs[0]]
            if a['simd'] and a['width']==4:
                # Only group a short run once, preserving non-overlapping triplets.
                bs=[[b for b in rows if b['simd'] and b['width']==4 and b['offset']==a['offset']+d and b['operation']==a['operation'] and abs(b['instruction_index']-a['instruction_index'])<=12] for d in (4,8)]
                if all(bs):reason=reason or 'three_contiguous_scalar_float_operands';support+=[bs[0][0],bs[1][0]]
            if reason:
                off=a['offset'];entry=candidates.setdefault(off,{'offset':off,'width':12,'reasons':[],'evidence':[]})
                if reason not in entry['reasons']:entry['reasons'].append(reason)
                entry['evidence']+= [{k:b[k] for k in ('function','instruction_rva','offset','width','operation','this_alias')} for b in support]
                if a['simd'] and a['width']==32:
                    lane=candidates.setdefault(off+16,{'offset':off+16,'width':12,'reasons':[],'evidence':[]})
                    lane['reasons'].append('second_16byte_lane_of_YMM_operand_NOT_verified_matrix_row')
                    lane['evidence'].append({k:a[k] for k in ('function','instruction_rva','offset','width','operation','this_alias')})
    selected=[]
    # Strong split-copy/packed boundaries win overlapping scalar hypotheses.
    def priority(e):return (0 if any(x!='three_contiguous_scalar_float_operands' for x in e['reasons']) else 1,e['offset'])
    for e in sorted(candidates.values(),key=priority):
        e['overlaps']=[x['offset'] for x in selected if abs(e['offset']-x['offset'])<12]
        e['selected']=not e['overlaps']
        if e['selected']:selected.append(e)
    return sorted(candidates.values(),key=lambda e:e['offset'])

def generate(input_dir,output_dir,header=None):
    meta=json.loads((input_dir/'SURFACE_SOURCE_REVIEW.json').read_text());output_dir.mkdir(parents=True,exist_ok=True)
    functions=[]
    for m in meta['function_captures']:
        raw=(input_dir/(m['name']+'.bin')).read_bytes()
        assert len(raw)==m['size'] and hashlib.sha256(raw).hexdigest()==m['sha256'],'captured code integrity mismatch'
        functions.append(analyze(raw,int(m['rva'],16),m['name']))
    fields=vector_fields(functions);selected=[f for f in fields if f['selected']]
    report={'decoder':VERSION,'source':'checksum-verified v0.1a7 native runtime captures','functions':functions,'vector_fields':fields,'selected_root_offsets':[f['offset'] for f in selected],'surface_verified':False,'contact_verified':False}
    (output_dir/'STATIC_ACCESS_V2.json').write_text(json.dumps(report,indent=2)+'\n')
    text='# WallRun.this access map V2\n\nOffline Capstone 5.0.6 (VEX/AVX). All captured bytes decode without skip-data or guessed instruction lengths. Decoded coverage is byte coverage of the authenticated RUNTIME_FUNCTION span, not proof of complete semantic recovery or coverage of external/cold chunks. Conservative CFG must-alias merge; all GPR writes invalidate aliases, Windows volatile GPRs clobbered on calls; nonvolatile this-derived aliases retained. No second-level pointer chasing.\n\n'
    text+='|Function|Bytes|Instructions|Decoded coverage|CFG reachable|External branch targets|SHA256|\n|---|---:|---:|---:|---:|---|---|\n'
    for f in functions:text+=f"|{f['name']}|{f['size']}|{f['decoded_instructions']}|{f['decoded_instruction_coverage']:.2%}|{f['cfg_reachable_instructions']}|{', '.join(hex(x) for x in f['outside_span_branches']) or 'none'}|{f['sha256']}|\n"
    text+='\nREAD/WRITE below describes the original game instruction; the MOD only reads. ADDRESSES are separate from memory dereferences. Instruction frequency is static occurrence count, not runtime execution frequency. Calls are proximity only; no interprocedural ABI/result inference.\n\n|Offset|Scope/parent|Function|Instruction RVA|Operation|Width|Alias|Type suspicion|Nearest call / instruction delta|Static field frequency|Instruction / register source|\n|---|---|---|---|---|---:|---|---|---|---:|---|\n'
    counts=collections.Counter((a['scope'],a['parent_field'],a['offset']) for f in functions for a in f['accesses'])
    for f in functions:
        for a in f['accesses']:
            text+=f"|{hex(a['offset'])}|{a['scope']}/{hex(a['parent_field'])}|{f['name']}|{hex(a['instruction_rva'])}|{a['operation']}|{a['width']}|{a['this_alias']}|{a['possible_type']}|{hex(a['nearest_call_rva']) if a['nearest_call_rva'] else '-'} / {a['call_instruction_distance']}|{counts[a['scope'],a['parent_field'],a['offset']]}|`{a['instruction']}`|\n"
    (output_dir/'WALLRUN_THIS_ACCESS_MAP_V2.md').write_text(text)
    text='# WallRun vector-like field observation plan\n\nRoot triplets are selected from actual instruction operands, not a sliding object scan. All four original function SHA256 values must match before activating this build-specific plan. Overlapping hypotheses are suppressed, never counted independently. Float/vector suspicion does not prove world-space coordinates, field semantics, normal sign, or contact.\n\n|Offset|Read bytes|Selected|Static evidence|Overlap with selected field|Evidence instruction RVAs|\n|---|---:|---|---|---|---|\n'
    for e in fields:text+=f"|{hex(e['offset'])}|12|{e['selected']}|{', '.join(e['reasons'])}|{', '.join(hex(x) for x in e['overlaps'])}|{', '.join(sorted({hex(x['instruction_rva']) for x in e['evidence']}))}|\n"
    text+='\nNew root values were not captured by v0.1a7; no four-face or subtype score is invented for them. v0.1a8 records first, latest (relabelled last active sample at Deinit), and every 100ms worker observation. These are asynchronous samples, never claimed as exact Entry/Deinit state. Surface candidates and nearby-point candidates retain full source offset identity.\n'
    (output_dir/'WALLRUN_VECTOR_FIELDS.md').write_text(text)
    grouped=collections.defaultdict(list)
    for f in functions:
        for c in f['calls']:
            if c['target_rva'] is not None:grouped[c['target_rva']].append(c)
    ranks=[]
    for target,cs in grouped.items():
        entry=any(c['function']=='Entry' for c in cs);update=any(c['function']=='Update' for c in cs);store=any(c['followed_by_vector_store'] for c in cs);out=any(c['output_buffer_candidate'] for c in cs);child=any(c['uses_0x128_child'] for c in cs)
        ranks.append(dict(target_rva=target,rank=int(entry)+2*int(update)+3*int(store)+2*int(out)+int(child),called_from_WallRunEntry=entry,called_from_WallRunUpdate=update,output_buffer_candidate=out,followed_by_vector_store=store,uses_0x128_child=child,reference_mod_similarity='not established: wrapper semantics alone do not identify native callee',build_unique='not established: no target AOB capture',runtime_function_boundary='not captured for callee',game_call=0,hook=0,calls=cs))
    ranks.sort(key=lambda x:(-x['rank'],x['target_rva']))
    (output_dir/'SURFACE_FUNCTION_CANDIDATES.json').write_text(json.dumps(ranks,indent=2)+'\n')
    text='# Surface function candidates\n\nRank is static triage only. No callee body/AOB/prefix/boundary or typed ABI was authenticated. Reference wrapper similarity is NOT established. All game_call=0, new hook=0. Output buffers are argument provenance hypotheses, not verified direction or size. Fields within 12 instructions before/after each call may be on different CFG paths; proximity is not causality. See companion JSON for every before/after field access.\n\n|Target RVA|Rank|Entry|Update|Output buffer possible|Vector store follows|Uses child128 nearby|Callsites|\n|---|---:|---|---|---|---|---|---|\n'
    for c in ranks:text+=f"|{hex(c['target_rva'])}|{c['rank']}|{c['called_from_WallRunEntry']}|{c['called_from_WallRunUpdate']}|{c['output_buffer_candidate']}|{c['followed_by_vector_store']}|{c['uses_0x128_child']}|{', '.join(hex(x['instruction_rva']) for x in c['calls'])}|\n"
    for c in ranks:
        text+=f"\n## {hex(c['target_rva'])}\n\n"
        for call in c['calls']:
            def desc(rows):return ', '.join(f"{x['scope']}[{hex(x['parent_field'])}]+{hex(x['offset'])}:{x['operation']}/{x['width']}@{hex(x['instruction_rva'])}" for x in rows) or 'none in window'
            text+=f"- {call['function']} call {hex(call['instruction_rva'])}; arguments {call['arguments']}\n- Before: {desc(call['fields_before'])}\n- After: {desc(call['fields_after'])}\n"
    (output_dir/'SURFACE_FUNCTION_CANDIDATES.md').write_text(text)
    if header:
        s='#pragma once\n// Generated by tools/static_access_v2.py; original bounded code SHA256 gate.\nnamespace ws::static_plan {\nstruct Function {const char* name;const char* hash;unsigned bytes;unsigned instructions;};\ninline constexpr Function functions[]={\n'
        for f in functions:s+=f' {{"{f["name"]}","{f["sha256"]}",{f["size"]},{f["decoded_instructions"]}}},\n'
        s+='};\ninline constexpr unsigned rootOffsets[]={'+','.join(hex(f['offset']) for f in selected)+'};\n}\n';header.write_text(s)
    print('PASS:',[(f['name'],f['decoded_instructions'],f['decoded_instruction_coverage'],len(f['accesses'])) for f in functions]);print('selected fields:',[hex(x['offset']) for x in selected])
    return report
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('input_dir',type=Path);p.add_argument('--output-dir',type=Path,required=True);p.add_argument('--header',type=Path);a=p.parse_args();generate(a.input_dir,a.output_dir,a.header)
