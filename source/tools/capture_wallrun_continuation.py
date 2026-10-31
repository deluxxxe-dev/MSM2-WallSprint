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
class Refusal(ValueError):
    pass

def extract(data,start=TARGET,limit=CAP):
    if start!=TARGET:raise Refusal('invalid_rva_only_0x14248f9_allowed')
    if limit!=CAP:raise Refusal('invalid_cap_only_128_allowed')
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
    try:window=read(start,bound-start)
    except ValueError as ex:raise Refusal('target_range_not_file_backed_or_truncated') from ex
    md=pe.decoder();decoded=[];used=0;stop='bounded_range_end'
    while used<len(window):
        ins=next(md.disasm(window[used:],start+used,count=1),None)
        if ins is None:raise Refusal('decode_gap_before_control_boundary')
        decoded.append(dict(rva=ins.address,size=ins.size,mnemonic=ins.mnemonic,operands=ins.op_str))
        used+=ins.size
        if pe.is_boundary(ins) or ins.mnemonic in {'hlt','int1','int3','xabort','xbegin'}:stop='first_control_boundary';break
    if not used:raise Refusal('empty_bounded_window')
    code=window[:used]
    return dict(schema=1,result='CAPTURED_ORIGINAL_OFFLINE_WINDOW',exe_sha256=digest,build='2.810.0.0_SHA_pinned',target_rva=start,cap=CAP,range_end_rva=start+used,bytes=len(code),byte_evidence_hex=code.hex(),block_sha256=hashlib.sha256(code).hexdigest(),stop=stop,section=dict(begin_rva=begin,end_rva=end,raw_offset=raw,raw_size=raw_size,flags=flags,executable=True,file_backed=True,target_file_offset=raw+start-begin),runtime_coverage='covering_entry' if runtime else 'no_covering_entry',runtime=runtime,metadata_bound_rva=bound,instructions=decoded,decoder='Capstone5.0.6',original_file_only=True,live_reads=0,recursive_follow=False,movement_API_identified=False,write_certificate=False)

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('exe',type=Path);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    try:
        if not a.exe.is_file() or not 0<a.exe.stat().st_size<=pe.MAX_FILE:raise Refusal('EXE_missing_or_size_capped')
        with a.exe.open('rb') as source:data=source.read(pe.MAX_FILE+1)
        if len(data)>pe.MAX_FILE:raise Refusal('EXE_size_capped')
        report=extract(data);status=0
    except (OSError,ValueError,struct.error) as ex:
        report=dict(schema=1,result='REFUSED',reason=str(ex),target_rva=TARGET,cap=CAP,write_certificate=False);status=2
    # Never replace another run, the EXE, or any existing user file.
    try:
        with a.output.open('x',encoding='utf-8') as f:json.dump(report,f,indent=2);f.write('\n')
    except OSError as ex:
        print('WALLRUN_OFFLINE REFUSED output_not_created_existing_preserved category='+type(ex).__name__);return 2
    print('WALLRUN_OFFLINE '+report['result']+' output='+str(a.output));return status
if __name__=='__main__':raise SystemExit(main())
