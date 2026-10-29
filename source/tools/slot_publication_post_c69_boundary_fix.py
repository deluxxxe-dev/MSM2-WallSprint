"""Corrections for c71 checkpoint validation and owner-observed provenance.

The c71 classifier remains diagnostic-only.  Two corrections live here:

1. Synthetic checkpoint validation binds to the embedded c71 result/header while
   production checkpoints remain pinned to the exact accepted c70/image.
2. Accepted c70's observer-lifetime fix can report the next post-c69 boundary
   after an authenticated c69 terminal even when the current key is neither in
   ``tx.post_c69`` nor ``view.c69_source_keys``.  That is a real c70 provenance
   class, not evidence that the current path itself is post-c69.  Record it
   explicitly as a retired-c69-observer distinct edge without relabeling it.
"""
from pathlib import Path
import hashlib

import slot_publication_post_c69_boundary as base
from slot_publication_post_c69_boundary import *  # re-export the c71 surface

c56,c58 = base.c56,base.c58
_original_sources = base.sources
_original_capture_edge = base._capture_edge


def sources():
    return dict(_original_sources(),
        slot_publication_post_c69_boundary_fix_py=c56.sha(Path(__file__).read_bytes()))


def _capture_edge(tx,view,index,ins,kind,target,architectural):
    """Preserve c70's retired-observer provenance without inventing reachability."""
    key = view.key
    # Existing c71 classes keep their original exact behavior.
    if key in getattr(tx,'post_c69',set()) or key in getattr(view,'c69_source_keys',set()):
        return _original_capture_edge(tx,view,index,ins,kind,target,architectural)

    active = getattr(view,'c69_record',None)
    if active is None:
        return _original_capture_edge(tx,view,index,ins,kind,target,architectural)
    if key is None or key[0]!=index or key not in tx.contexts:
        raise c58.EvidenceError('c70 retired-observer terminal lacks current source context')

    same_authenticated_source = (
        key==getattr(view,'c69_key',None)
        and tx.scan_token==getattr(view,'c69_scan_token',None)
        and ins.address==active.get('source_instruction_rva'))
    if same_authenticated_source:
        raise c58.EvidenceError('c71 retired-observer provenance used for authenticated c69 sibling')

    state,path = tx.contexts[key]
    if key[1]!=c58.state_id(state):
        raise c58.EvidenceError('wrong c70 retired-observer terminal incoming state')
    origin = getattr(tx,'post69_origins',{}).get(key)
    provenance = dict(
        phase='retired_c69_first_terminal_observer_distinct_edge',
        reason='c70_retired_c69_first_terminal_observer_later_distinct_edge',
        underlying_refusal=None,key=list(key),pc=ins.address,size=ins.size,
        scan_token=tx.scan_token,
        c69_authorization_scan_token=getattr(view,'c69_scan_token',None),
        post_c69=False,used_c69_source=False,
        retired_c69_first_terminal_observer=True,
        active_c69_record_identity=active.get('record_identity'),
        active_c69_key=list(view.c69_key) if getattr(view,'c69_key',None) is not None else None,
        post_c69_origin_digest=c56.digest(origin) if origin is not None else None)
    record = base._terminal_common(tx,view,key,state,path,'edge_callback',True,provenance)
    begin,end,code,_ = view.row(index)
    edges = base.c59.classify_edges(view.session,begin,end,ins)
    if not 1<=len(edges)<=2:
        raise c58.EvidenceError('missing exact c70 retired-observer terminal successors')
    if not any(edge.get('edge_kind')==kind and edge.get('edge_target_rva')==target
        and edge.get('architectural_successor')==architectural for edge in edges):
        raise c58.EvidenceError('c70 retired-observer callback not present in classified successor set')
    record.update(source_runtime_row_sha256=view.session.row_identity(index),source_code_sha256=c56.sha(code),
        source_begin_rva=begin,source_end_rva=end,source_instruction_rva=ins.address,source_instruction_size=ins.size,
        source_instruction_sha256=c56.sha(bytes(ins.bytes)),source_mnemonic=ins.mnemonic,edges=edges,
        state_scope='authenticated_entry_seed_retired_c69_observer_distinct_edge_not_current_post_c69_claim')
    base._emit(view,record)


def validate_checkpoint(value):
    if type(value)!=dict or set(('header','result','records','checkpoint_identity'))-set(value):
        raise c58.EvidenceError('invalid c71 checkpoint shape')
    identity=value['checkpoint_identity'];body={k:v for k,v in value.items() if k!='checkpoint_identity'}
    if c56.digest(body)!=identity:
        raise c58.EvidenceError('corrupt c71 checkpoint identity')
    header,result,records=value['header'],value['result'],value['records']
    if (header.get('checkpoint_version')!=base.CHECKPOINT_VERSION
        or header.get('checkpoint_contract')!=base.CHECKPOINT_CONTRACT
        or header.get('tool_sources')!=sources()
        or header.get('limits')!=base.asdict(c58.Limits())
        or header.get('mutable_transaction_resume_snapshot') is not False
        or result.get('repository_result')!=base.READY
        or result.get('tool_version')!=base.VERSION
        or result.get('schema_version')!=base.SCHEMA
        or result.get('contract')!=base.CONTRACT
        or result.get('image_authenticated') is not True
        or header.get('image_sha256')!=result.get('image_sha256')
        or header.get('accepted_c70_physical_sha256')!=result.get('accepted_c70_physical_sha256')
        or header.get('accepted_c70_result_identity')!=result.get('accepted_c70_result_identity')
        or header.get('accepted_c70_authentication_identity')!=result.get('accepted_c70_authentication_identity')
        or header.get('c71_classifier_identity')!=result.get('classifier_identity')
        or c56.digest(result)!=header.get('c71_result_identity')
        or len(records)!=header.get('record_count')
        or result.get('post_c69_boundary_instances_total')!=len(records)):
        raise c58.EvidenceError('stale/wrong c71 checkpoint header/result')
    # Synthetic fixture baselines intentionally have no accepted physical c70
    # file SHA. Production owner checkpoints do, and remain pinned exactly.
    physical=header.get('accepted_c70_physical_sha256')
    if physical is not None and (
        physical!=base.EXPECTED_C70_FILE_SHA
        or header.get('accepted_c70_result_identity')!=base.EXPECTED_C70_RESULT_IDENTITY
        or header.get('image_sha256')!=base.EXPECTED_SHA):
        raise c58.EvidenceError('wrong production c71 checkpoint owner/image identity')
    stream=hashlib.sha256()
    for record in records:
        if type(record)!=dict or c56.digest({k:v for k,v in record.items() if k!='record_identity'})!=record.get('record_identity'):
            raise c58.EvidenceError('stale/wrong c71 checkpoint record')
        stream.update(c56.encoded(record)+b'\n')
    if stream.hexdigest()!=header.get('record_stream_sha256') or stream.hexdigest()!=result.get('authenticated_record_set_sha256'):
        raise c58.EvidenceError('wrong c71 checkpoint record set identity')
    return value


# Existing Baseline/make_checkpoint/main functions resolve these module globals at
# call time, so correction identities remain explicit and accepted c70 is untouched.
base.sources = sources
base._capture_edge = _capture_edge
base.validate_checkpoint = validate_checkpoint


def main():
    return base.main()


if __name__=='__main__':
    raise SystemExit(main())
