"""One authenticated original-file window; no image scan, live process or write permission."""
import argparse
import hashlib
import json
import struct
from pathlib import Path
import slot_publication as pe

EXPECTED_SHA=pe.EXPECTED_SHA
TARGET=0x14248F9
CAP=128
FRAGMENT_END=0x1424946
FIRST_END=0x1424922
FIRST_SHA="c5f5f80a08fe76ed7251ec5abc4be1ed6d3ce1479eafc2b5e32038437caed0b3"
CHAIN=(0x1424530,0x14246d6,0x94bdc14)
SUCCESSOR=0x14246D6
PREFIX_SHA="e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f"
class Refusal(ValueError):
    pass

def extract(data,start=TARGET,limit=CAP,complete=False,successor=False):
    if start!=TARGET:raise Refusal('invalid_rva_only_0x14248f9_allowed')
    if limit!=CAP:raise Refusal('invalid_cap_only_128_allowed')
    if complete and successor:raise Refusal('mutually_exclusive_fixed_modes')
    if successor:start=SUCCESSOR
    digest=hashlib.sha256(data).hexdigest()
    if digest!=EXPECTED_SHA:raise Refusal('unsupported_build_sha256')
    try:
        sections,base,_,read,image_size,_=pe.parse_pe(data)
    except (ValueError,struct.error,IndexError) as e:raise Refusal('invalid_PE_or_runtime_metadata: '+str(e)) from e
    region=[s for s in sections if s[0]<=start<s[1]]
    if len(region)!=1:raise Refusal('target_section_missing_or_conflicting')
    begin,end,raw,raw_size,flags=region[0]
    if not flags&0x20000000:raise Refusal('target_section_not_executable')
    optional=struct.unpack_from('<I',data,60)[0]+24
    if struct.unpack_from('<I',data,optional+108)[0]<4:raise Refusal('exception_directory_not_declared')
    table,size=struct.unpack_from('<II',data,optional+112+3*8)
    table_bytes=read(table,size) if size else b''
    rows=[struct.unpack_from('<III',table_bytes,i) for i in range(0,size,12)] if size else []
    covering=[r for r in rows if r[0]<=start<r[1]]
    if len(covering)>1:raise Refusal('conflicting_runtime_coverage')
    bound=min(start+limit,end)
    future=[r[0] for r in rows if r[0]>start]
    if future:bound=min(bound,min(future))
    runtime=None
    if covering:
        b,e,u=covering[0];bound=min(bound,e)
        try:
            header=read(u,4);version=header[0]&7;uf=header[0]>>3;count=header[2]
            if version!=1 or uf&~7 or (uf&4 and uf&3):raise Refusal('unsupported_unwind_version_or_flags')
            n=4+((count+1)//2)*4
            tail=12 if uf&4 else 4 if uf&3 else 0
            block=read(u,n+tail)
            runtime=dict(begin_rva=b,end_rva=e,unwind_rva=u,version=version,flags=uf,prolog_size=header[1],code_slots=count,frame_register=header[3]&15,frame_offset=header[3]>>4,metadata_bytes=len(block),metadata_sha256=hashlib.sha256(block).hexdigest(),unwind_hex=block.hex())
            if uf&4:
                cb,ce,cu=struct.unpack_from('<III',block,n)
                if not 0<=cb<ce<=image_size or not 0<cu<image_size:raise Refusal('invalid_chained_runtime_metadata')
                runtime['chained_entry']=dict(begin_rva=cb,end_rva=ce,unwind_rva=cu,followed=False)
            elif uf&3:
                handler=struct.unpack_from('<I',block,n)[0]
                if not 0<handler<image_size:raise Refusal('invalid_unwind_handler_rva')
                runtime['handler_rva']=handler
        except (ValueError,struct.error) as ex:
            if isinstance(ex,Refusal):raise
            raise Refusal('unwind_metadata_not_file_backed_or_truncated') from ex
    if complete:
        if not runtime or (runtime['begin_rva'],runtime['end_rva'])!=(TARGET,FRAGMENT_END) or runtime['flags']!=4:raise Refusal('fixed_fragment_runtime_identity_mismatch')
        chain=runtime.get('chained_entry',{})
        if tuple(chain.get(k) for k in ['begin_rva','end_rva','unwind_rva'])!=CHAIN:raise Refusal('fixed_fragment_chain_identity_mismatch')
        if bound<FRAGMENT_END:raise Refusal('fixed_fragment_section_or_metadata_truncated')
        bound=FRAGMENT_END
    if successor:
        if CHAIN not in rows:raise Refusal('authenticated_prefix_runtime_identity_missing')
        try:prefix=read(CHAIN[0],CHAIN[1]-CHAIN[0])
        except ValueError as ex:raise Refusal('authenticated_prefix_not_file_backed') from ex
        if hashlib.sha256(prefix).hexdigest()!=PREFIX_SHA:raise Refusal('authenticated_prefix_sha_mismatch')
        if not runtime:raise Refusal('successor_covering_runtime_metadata_absent')
        if runtime['begin_rva']!=SUCCESSOR:raise Refusal('successor_runtime_not_contiguous_with_prefix')
        if runtime['flags']&4:
            chain=runtime.get('chained_entry',{})
            if tuple(chain.get(k) for k in ['begin_rva','end_rva','unwind_rva'])!=CHAIN:raise Refusal('successor_chain_identity_mismatch')
    try:window=read(start,bound-start)
    except ValueError as ex:raise Refusal('target_range_not_file_backed_or_truncated') from ex
    if complete and hashlib.sha256(window[:FIRST_END-TARGET]).hexdigest()!=FIRST_SHA:raise Refusal('previous_41byte_capture_identity_mismatch')
    md=pe.decoder();decoded=[];edges=[];uses=[];used=0;stop='bounded_range_end'
    while used<len(window):
        ins=next(md.disasm(window[used:],start+used,count=1),None)
        if ins is None:raise Refusal('decode_gap_before_control_boundary')
        decoded.append(dict(rva=ins.address,size=ins.size,mnemonic=ins.mnemonic,operands=ins.op_str))
        if successor:
            try:reads,writes=ins.regs_access()
            except pe.cs.CsError as ex:raise Refusal('successor_register_access_unsupported') from ex
            aliases={pe.x.X86_REG_RAX,pe.x.X86_REG_EAX,pe.x.X86_REG_AX,pe.x.X86_REG_AL,pe.x.X86_REG_RDX,pe.x.X86_REG_EDX,pe.x.X86_REG_DX,pe.x.X86_REG_DL,pe.x.X86_REG_DH}
            overlap_read=[ins.reg_name(r) for r in reads if r in aliases]
            overlap_write=[ins.reg_name(r) for r in writes if r in aliases]
            if overlap_read or overlap_write:uses.append(dict(site_rva=ins.address,reads=overlap_read,writes=overlap_write,evidence='decoded_register_overlap_not_value_flow_or_ABI'))
        used+=ins.size
        boundary=pe.is_boundary(ins) or ins.mnemonic in {'hlt','int1','int3','xabort','xbegin'}
        if complete and boundary:
            is_ret=ins.group(pe.cs.CS_GRP_RET)
            is_jump=ins.group(pe.cs.CS_GRP_JUMP);is_call=ins.group(pe.cs.CS_GRP_CALL)
            if is_ret:raise Refusal('RET_in_fixed_fragment_requires_review')
            if not (is_ret or is_jump or is_call):raise Refusal('unsupported_control_in_fixed_fragment')
            direct=bool(ins.operands and ins.operands[0].type==pe.x.X86_OP_IMM)
            edges.append(dict(site_rva=ins.address,kind='ret' if is_ret else 'call' if is_call else 'jump',target_rva=int(ins.operands[0].imm) if direct else None,fallthrough_rva=ins.address+ins.size if is_call or (is_jump and ins.mnemonic!='jmp') else None,followed=False))
        elif boundary:
            if successor:
                direct=bool(ins.operands and ins.operands[0].type==pe.x.X86_OP_IMM)
                edges.append(dict(site_rva=ins.address,target_rva=int(ins.operands[0].imm) if direct else None,followed=False))
            stop='first_local_control_boundary' if successor else 'first_control_boundary';break
    if not used:raise Refusal('empty_bounded_window')
    if complete and used!=FRAGMENT_END-TARGET:raise Refusal('incomplete_fixed_fragment_decode')
    if complete:stop='fixed77_metadata_end_not_full_logical_function'
    code=window[:used]
    return dict(schema=1,result='CAPTURED_ORIGINAL_OFFLINE_WINDOW',exe_sha256=digest,build='2.810.0.0_SHA_pinned',target_rva=start,cap=FRAGMENT_END-TARGET if complete else CAP,mode='shared_successor' if successor else 'complete_fixed77' if complete else 'first_control',register_overlap=uses,authenticated_prefix=dict(begin_rva=CHAIN[0],end_rva=CHAIN[1],unwind_rva=CHAIN[2],sha256=PREFIX_SHA) if successor else None,continuity='authenticated_prefix_end_plus_actual_runtime' if successor else None,edges=edges,previous_window_sha_verified=complete,range_end_rva=start+used,bytes=len(code),byte_evidence_hex=code.hex(),block_sha256=hashlib.sha256(code).hexdigest(),stop=stop,section=dict(begin_rva=begin,end_rva=end,raw_offset=raw,raw_size=raw_size,flags=flags,executable=True,file_backed=True,target_file_offset=raw+start-begin),runtime_coverage='covering_entry' if runtime else 'no_covering_entry',runtime=runtime,metadata_bound_rva=bound,instructions=decoded,decoder='Capstone5.0.6',original_file_only=True,live_reads=0,recursive_follow=False,movement_API_identified=False,write_certificate=False)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);p.add_argument('--complete-fragment',action='store_true',help='only authenticated fixed77-byte chained fragment');p.add_argument('--shared-successor',action='store_true',help='only original shared successor0x14246d6,max128');a=p.parse_args()
    try:
        if not a.exe.is_file() or not 0<a.exe.stat().st_size<=pe.MAX_FILE:raise Refusal('EXE_missing_or_size_capped')
        with a.exe.open('rb') as source:data=source.read(pe.MAX_FILE+1)
        if len(data)>pe.MAX_FILE:raise Refusal('EXE_size_capped')
        report=extract(data,complete=a.complete_fragment,successor=a.shared_successor);status=0
    except (OSError,ValueError,struct.error) as ex:
        report=dict(schema=1,result='REFUSED',reason=str(ex),target_rva=SUCCESSOR if a.shared_successor else TARGET,cap=FRAGMENT_END-TARGET if a.complete_fragment else CAP,mode='shared_successor' if a.shared_successor else 'complete_fixed77' if a.complete_fragment else 'first_control',write_certificate=False);status=2
    # Never replace another run, the EXE, or any existing user file.
    try:
        with a.output.open('x',encoding='utf-8') as f:json.dump(report,f,indent=2);f.write('\n')
    except OSError as ex:
        print('WALLRUN_OFFLINE REFUSED output_not_created_existing_preserved category='+type(ex).__name__);return 2
    print('WALLRUN_OFFLINE '+report['result']+' output='+str(a.output));return status
if __name__=='__main__':raise SystemExit(main())
