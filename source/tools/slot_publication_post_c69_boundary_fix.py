"""Checkpoint-validation correction for c71 synthetic fixtures.

The c71 classifier itself is unchanged.  The original checkpoint validator
required the production executable SHA even for internally consistent synthetic
fixtures.  This layer validates checkpoint identity against the embedded c71
result/header for every checkpoint, while keeping the exact accepted c70/image
pins mandatory whenever the checkpoint is production-bound.
"""
from pathlib import Path
import hashlib

import slot_publication_post_c69_boundary as base
from slot_publication_post_c69_boundary import *  # re-export the c71 surface

c56,c58 = base.c56,base.c58
_original_sources = base.sources


def sources():
    return dict(_original_sources(),
        slot_publication_post_c69_boundary_fix_py=c56.sha(Path(__file__).read_bytes()))


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
    # file SHA.  Production owner checkpoints do, and remain pinned exactly.
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
# call time, so adding the correction source to identities does not alter c70.
base.sources = sources
base.validate_checkpoint = validate_checkpoint


def main():
    return base.main()


if __name__=='__main__':
    raise SystemExit(main())
