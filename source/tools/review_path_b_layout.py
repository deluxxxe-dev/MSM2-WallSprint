"""Private original-byte Path B review. No input bytes are copied to output.
All numerical matches remain candidates. This tool never grants write permission.
Requires Capstone 5; independent fixtures are not accepted-build evidence.
"""
import argparse, collections, hashlib, json, math, re, struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_GRP_CALL, CS_GRP_JUMP, CS_GRP_RET, CS_AC_WRITE
from capstone.x86 import X86_OP_REG, X86_OP_MEM, X86_OP_IMM
from static_access_v2 import reg, VOLATILE
WRITER=0x2cfc3c0; CALLER=0x2cfc7d0; RETURN=0x2cfc84b
HASH='ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196'
def fields(line):
    return dict(re.findall(r'(\w+)=([^\s]+)',line))
def number(v): return int(v,0)
def decode(raw,rva):
    md=Cs(CS_ARCH_X86,CS_MODE_64);md.detail=True
    ins=list(md.disasm(raw,rva))
    if sum(x.size for x in ins)!=len(raw): raise ValueError('incomplete decode; byte skipping forbidden')
    return ins

def flow(raw,rva):
    """Must-analysis: preserve only identical facts at every CFG predecessor.
    Pointer aliases and exact register copies are tracked. Unsupported writes kill
    facts. Computations retain dependency labels but never become direct copies.
    Stack spills, nested dereferences and call results remain unknown.
    """
    ins=decode(raw,rva);by={i.address:i for i in ins};start={r:('ptr',r,0) for r in ('rcx','rdx','r8','r9','rsp')}
    states={rva:start};todo=collections.deque([rva]);outside=set();indirect=set()
    def ptr(i,op,s):
        if op.type!=X86_OP_MEM or op.mem.index:return None
        p=s.get(reg(i,op.mem.base))
        return ('ptr',p[1],p[2]+op.mem.disp) if p and p[0]=='ptr' else None
    def value(i,op,s):
        if op.type==X86_OP_REG:
            v=s.get(reg(i,op.reg))
            if v and v[0]=='direct' and v[3]!=op.size:return None
            return v
        if op.type==X86_OP_IMM:return ('constant',op.imm)
        p=ptr(i,op,s)
        return ('direct',p[1],p[2],op.size) if p else None
    moves={'mov','movaps','movups','movdqa','movdqu','movss','movsd','movq','movd','vmovaps','vmovups','vmovdqa','vmovdqu','vmovss','vmovsd','vmovq','vmovd'}
    def transfer(i,s):
        n=s.copy();ops=i.operands;assignment=None
        if ops and ops[0].type==X86_OP_REG:
            if i.mnemonic in moves and len(ops)==2 and ops[0].size==ops[1].size:
                assignment=value(i,ops[1],s)
                if assignment and assignment[0]=='ptr' and ops[0].size!=8:assignment=None
            elif i.mnemonic=='lea' and len(ops)==2 and ops[0].size==8:assignment=ptr(i,ops[1],s)
            elif i.mnemonic in ('add','sub') and len(ops)==2 and ops[1].type==X86_OP_IMM and ops[0].size==8:
                p=s.get(reg(i,ops[0].reg))
                if p and p[0]=='ptr':assignment=('ptr',p[1],p[2]+ops[1].imm*(1 if i.mnemonic=='add' else -1))
            # Unsupported computations are deliberately UNKNOWN, not direct RDX.
        for rid in i.regs_access()[1]:n.pop(reg(i,rid),None)
        if assignment:n[reg(i,ops[0].reg)]=assignment
        if i.group(CS_GRP_CALL):
            for r in VOLATILE|{f'xmm{x}' for x in range(6)}|{f'ymm{x}' for x in range(6)}:n.pop(r,None)
            if 'rsp' in s:n['rsp']=s['rsp']
        if i.mnemonic in ('push','pop') and 'rsp' in s:
            p=s['rsp'];n['rsp']=('ptr',p[1],p[2]+(-8 if i.mnemonic=='push' else 8))
        # SIMD aliases overlap. Never retain an old xmm fact across ymm writes.
        for rid in i.regs_access()[1]:
            rn=i.reg_name(rid)
            if rn.startswith(('xmm','ymm','zmm')):
                for prefix in ('xmm','ymm','zmm'):
                    alias=prefix+rn[3:]
                    if alias!=rn:n.pop(alias,None)
        return n
    iterations=0
    while todo:
        pc=todo.popleft();i=by[pc];n=transfer(i,states[pc]);iterations+=1
        if iterations>100000:raise ValueError('CFG convergence limit')
        successors=[]
        if i.group(CS_GRP_JUMP):
            if i.operands[0].type==X86_OP_IMM:successors.append(i.operands[0].imm)
            else:indirect.add(pc)
            if i.mnemonic!='jmp':successors.append(pc+i.size)
        elif not i.group(CS_GRP_RET):successors.append(pc+i.size)
        for target in successors:
            if target not in by:
                if rva<=target<rva+len(raw):raise ValueError('branch into instruction')
                if target!=rva+len(raw):outside.add(target)
                continue
            old=states.get(target);merged=n.copy() if old is None else {k:v for k,v in old.items() if n.get(k)==v}
            if merged!=old:states[target]=merged;todo.append(target)
    stores=[];reads=[];calls=[];instructions=[]
    for i in ins:
        instructions.append(dict(instruction_rva=i.address,instruction=i.mnemonic+' '+i.op_str,reachable=i.address in states))
        if i.address not in states:continue
        s=states[i.address];ops=i.operands
        if i.group(CS_GRP_CALL):calls.append(dict(instruction_rva=i.address,return_rva=i.address+i.size,target=ops[0].imm if ops[0].type==X86_OP_IMM else None,arguments={r:s.get(r) for r in ('rcx','rdx','r8','r9')},rsp=s.get('rsp')))
        for idx,op in enumerate(ops):
            if op.type!=X86_OP_MEM:continue
            p=ptr(i,op,s)
            if p and p[1]=='rdx' and (idx!=0 or i.mnemonic not in moves):reads.append(dict(instruction_rva=i.address,source_rdx_offset=p[2],width=op.size,instruction=i.mnemonic+' '+i.op_str))
        if not ops or ops[0].type!=X86_OP_MEM or not (ops[0].access&CS_AC_WRITE or i.mnemonic in moves):continue
        p=ptr(i,ops[0],s)
        if not p or p[1]!='rcx' or not (p[2]<0x30 and p[2]+ops[0].size>0):continue
        v=value(i,ops[1],s) if i.mnemonic in moves and len(ops)==2 else None
        direct=bool(v and v[0]=='direct' and v[1]=='rdx' and v[3]==ops[0].size)
        kind='RDX_direct' if direct else 'constant' if v and v[0]=='constant' else 'register' if len(ops)>1 and ops[1].type==X86_OP_REG else 'other'
        stores.append(dict(instruction_rva=i.address,instruction=i.mnemonic+' '+i.op_str,destination_rcx_offset=p[2],width=ops[0].size,source_kind=kind,source_rdx_offset=v[2] if direct else None,source_fact=v,semantics='candidate_until_actual_A_E',orientation_up_overlap=p[2]<0x1c and p[2]+ops[0].size>0x10))
    return dict(instructions=instructions,stores=stores,rdx_reads=reads,calls=calls,outside_branches=sorted(outside),indirect_branches=sorted(indirect),coverage_complete=not outside and not indirect,limitations='Unknown register computations, nested loads, stack spills and call results are not assigned semantics; R8/R9 participation only if source_fact proves it.')

def captures(lines):
    headers={};chunks=collections.defaultdict(dict)
    for line in lines:
        f=fields(line)
        if 'PATH_B_FUNCTION ' in line:
            role=f['role']
            if role in headers:raise ValueError('duplicate function capture; review one startup per file')
            headers[role]=f
        elif 'PATH_B_CODE ' in line:
            role=f['role'];off=number(f['offset'])
            if off in chunks[role]:raise ValueError('duplicate code chunk')
            chunks[role][off]=bytes.fromhex(f['bytes'])
    result={}
    for role,rva,size in [('writer',WRITER,444),('caller',CALLER,128)]:
        if role not in headers:continue
        h=headers[role]
        if number(h['begin_rva'])!=rva or number(h['size'])!=size or h['authenticated']!='1' or h['original_before_hook']!='1':raise ValueError('original capture admission mismatch')
        raw=b''
        for offset,data in sorted(chunks[role].items()):
            if offset!=len(raw):raise ValueError('missing/overlapping original chunk')
            raw+=data
        digest=hashlib.sha256(raw).hexdigest()
        if len(raw)!=size or digest!=h['sha256'] or (role=='writer' and digest!=HASH):raise ValueError('original SHA/size mismatch')
        result[role]=dict(header=h,flow=flow(raw,rva),raw=raw)
    if 'caller' in result:
        matches=[c for c in result['caller']['flow']['calls'] if c['return_rva']==RETURN and c['target']==WRITER]
        if len(matches)!=1:raise ValueError('exact call target/return missing')
        result['caller']['exact_call']=matches[0]
    return result

def vec(text):
    try:
        v=[float(x) for x in text.strip('()').split(',')]
        return v if len(v)==3 and all(math.isfinite(x) for x in v) else None
    except ValueError:return None

def candidates(entry,after=None):
    raw=bytes.fromhex(entry['raw_source_0x80'])
    if len(raw)!=128:raise ValueError('source snapshot must be 128 bytes')
    floats=struct.unpack('<32f',raw);refs={'WorldUp':[0,1,0],'N398':vec(entry.get('N398','')),'Hero_before':vec(entry.get('hero_up_before','')),'Hero_after':after}
    if entry.get('path_a_reference_valid')=='1':refs['Path_A']=vec(entry.get('path_a_interpolated_up',''))
    vectors=[]
    for width in (2,3,4):
        for j in range(33-width):
            v=floats[j:j+width]
            if not all(math.isfinite(x) for x in v):continue
            row=dict(offset=j*4,width=width,value=v,label='candidate_only')
            if width==3:
                norm=math.sqrt(sum(x*x for x in v));row['norm']=norm;row['cosines']={}
                for name,target in refs.items():
                    if not target:continue
                    tn=math.sqrt(sum(x*x for x in target))
                    if norm>1e-12 and tn>1e-12:row['cosines'][name]=sum(a*b for a,b in zip(v,target))/(norm*tn)
            vectors.append(row)
    pointers=[]
    for j in range(0,128,8):
        q=struct.unpack_from('<Q',raw,j)[0]
        if 0x10000<=q<=0x7fffffffffff:pointers.append(dict(offset=j,value=hex(q),dereferenced=False))
    return dict(entry_id=entry['entry_id'],session=entry['session'],finite_scalars=[dict(offset=j*4,value=x) for j,x in enumerate(floats) if math.isfinite(x)],vectors=vectors,pointer_looking_qwords=pointers)

def review(text,label):
    lines=text.splitlines();cap=captures(lines);entries={};events=[];links={};regs={}
    for line in lines:
        f=fields(line)
        if 'RESET_COPY_LAYOUT_ENTRY ' in line and f.get('source_readable')=='1':entries[f['entry_id']]=f
        if 'ORIENTATION_WRITER_EVENT ' in line:events.append(f)
        if 'PATH_B_ENTRY_LINK ' in line:links[(f['session'],f['qpc_ms'])]=f
        if 'ORIENTATION_WRITER_REGS ' in line:regs[(f['writer_id'],f['qpc_ms'])]=f
    stores=[];afters={}
    if 'writer' in cap:
        original=decode(cap['writer']['raw'],WRITER);by_end={i.address+i.size:i for i in original};mapping={s['instruction_rva']:s for s in cap['writer']['flow']['stores']}
        for event in events:
            link=links.get((event['session'],event['qpc_ms']));g=regs.get((event['writer_id'],event['qpc_ms']))
            if not link or link.get('entry_link_checks')!='1' or not g:continue
            entry=entries.get(link['entry_id'])
            if not entry or entry['session']!=event['session'] or entry['thread_id']!=event['thread_id'] or entry['thread_id']!=link['thread_id'] or entry['qpc']!=link['entry_qpc'] or number(entry['RCX'])!=number(event['transform']) or number(entry['caller_rva'])!=RETURN:continue
            i=by_end.get(number(event['module_offset_after']))
            if not i or i.address not in mapping:continue
            row=mapping[i.address];o=i.operands[0]
            values=[int(x,16) for x in g['post_gpr_RAX_RCX_RDX_RBX_RSP_RBP_RSI_RDI_R8_R9_R10_R11_R12_R13_R14_R15'].split(',')]
            names=['rax','rcx','rdx','rbx','rsp','rbp','rsi','rdi','r8','r9','r10','r11','r12','r13','r14','r15'];state=dict(zip(names,values))
            # Store operands must use unmodified GPRs, with actual effective address.
            if o.type!=X86_OP_MEM or o.mem.index or reg(i,o.mem.base) not in state:continue
            if any(reg(i,r)==reg(i,o.mem.base) for r in i.regs_access()[1]):continue
            address=state[reg(i,o.mem.base)]+o.mem.disp;target=number(entry['RCX'])
            if address!=target+row['destination_rcx_offset'] or not row['orientation_up_overlap'] or event.get('read_valid')!='1':continue
            before=vec(event['old_observed_up']);after=vec(event['new_observed_up'])
            stores.append(dict(event='PATH_B_STORE',entry_id=entry['entry_id'],session=entry['session'],thread_id=entry['thread_id'],**row,before=before,after=after,before_semantics='previous_observation_not_atomic',authenticated_original=True,post_store_instruction_anchored=True))
            afters[entry['entry_id']]=after
    observations=[candidates(e,afters.get(k)) for k,e in entries.items()]
    functions={role:{k:v for k,v in data.items() if k!='raw'} for role,data in cap.items()}
    return dict(label=label,functions=functions,entries=observations,stores=stores,PATH_B_FIELDS_VERIFIED=False,PATH_B_WRITE_PERMISSION=False,limitations=['No field semantic proof from correlations alone.','Review A and E at identical offsets; no automatic layout or write admission.','Unsupported instructions/dataflow remain unknown.','Missing events or code cannot establish absence.'])

def main():
    p=argparse.ArgumentParser();p.add_argument('log',type=Path);p.add_argument('--label',choices=['A','E'],required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--peer',type=Path,help='Derived JSON for the other face; compare identical offsets');a=p.parse_args()
    report=review(a.log.read_text(errors='replace'),a.label)
    if a.peer:
        peer=json.loads(a.peer.read_text())
        if peer['label']==a.label:raise ValueError('peer must be the other A/E face')
        def scores(r):
            out=collections.defaultdict(lambda:collections.defaultdict(list))
            for e in r['entries']:
                for v in e['vectors']:
                    if v['width']==3:
                        for name,score in v.get('cosines',{}).items():out[v['offset']][name].append(score)
            return {off:{name:sum(xs)/len(xs) for name,xs in targets.items()} for off,targets in out.items()}
        own,other=scores(report),scores(peer)
        report['same_offset_A_E_candidates']=[dict(offset=o,**{a.label:own[o],peer['label']:other[o]},semantics='candidate_only_static_and_runtime_review_required') for o in sorted(own.keys()&other.keys())]
    a.output.write_text(json.dumps(report,indent=2,allow_nan=False)+'\n');print('Path B:',len(report['entries']),'entries,',len(report['stores']),'anchored stores; write_permission=0')
if __name__=='__main__':main()
