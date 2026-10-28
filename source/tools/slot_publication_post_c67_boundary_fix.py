"""c69 checkpoint consistency correction.

The original c69 classifier is unchanged. This layer fixes only checkpoint
creation/validation so owned synthetic fixtures and the authenticated owner path
share the same internally-bound fail-closed contract. A future consumer must
still compare the checkpoint image/artifact identities with its current inputs.
"""
from dataclasses import asdict
import hashlib
from pathlib import Path

import slot_publication_post_c67_boundary as base

c56,c58 = base.c56,base.c58
_original_sources = base.sources


def sources():
    return dict(_original_sources(),
        slot_publication_post_c67_boundary_fix_py=c56.sha(Path(__file__).read_bytes()))


def make_checkpoint(baseline,result,records):
    record_stream_sha = hashlib.sha256()
    for record in records:
        record_stream_sha.update(c56.encoded(record)+b'\n')
    header = dict(
        checkpoint_version=base.CHECKPOINT_VERSION,
        checkpoint_contract=base.CHECKPOINT_CONTRACT,
        image_sha256=baseline.header['image_sha256'],
        accepted_c68_physical_sha256=baseline.header['accepted_c68_physical_sha256'],
        accepted_c68_result_identity=baseline.header['accepted_c68_result_identity'],
        accepted_c68_authentication_identity=baseline.header['accepted_c68_authentication_identity'],
        c69_classifier_identity=baseline.identity,
        c69_result_identity=c56.digest(result),
        record_count=len(records),
        record_stream_sha256=record_stream_sha.hexdigest(),
        limits=baseline.header['c68_limits'],
        tool_sources=sources())
    payload = dict(header=header,result=result,records=records)
    payload['checkpoint_identity'] = c56.digest(payload)
    return payload


def validate_checkpoint(value):
    if type(value)!=dict or set(('header','result','records','checkpoint_identity'))-set(value):
        raise c58.EvidenceError('invalid c69 checkpoint shape')
    identity = value['checkpoint_identity']
    body = {k:v for k,v in value.items() if k!='checkpoint_identity'}
    if c56.digest(body)!=identity:
        raise c58.EvidenceError('corrupt c69 checkpoint identity')

    header,result,records = value['header'],value['result'],value['records']
    classifier = result.get('classifier_header')
    if type(header)!=dict or type(result)!=dict or type(records)!=list or type(classifier)!=dict:
        raise c58.EvidenceError('invalid c69 checkpoint header/result')

    expected_domain = classifier.get('post_c67_uncertified_boundary_refused_roots_expected')
    if (header.get('checkpoint_version')!=base.CHECKPOINT_VERSION
        or header.get('checkpoint_contract')!=base.CHECKPOINT_CONTRACT
        or result.get('repository_result')!=base.READY
        or result.get('tool_version')!=base.VERSION
        or result.get('schema_version')!=base.SCHEMA
        or result.get('contract')!=base.CONTRACT
        or result.get('classification_complete') is not True
        or result.get('new_post_c67_boundaries_admitted')!=0
        or result.get('new_cross_row_candidate_count')!=0
        or result.get('cross_runtime_fact_propagation_unchanged') is not True
        or result.get('c68_candidate_output_unchanged') is not True
        or result.get('c56_through_c68_evidence_read_only') is not True
        or result.get('live_qword_reads')!=0
        or any(result.get(k) is not False for k in c56.FALSE_GATES)
        or header.get('image_sha256')!=result.get('image_sha256')
        or header.get('image_sha256')!=classifier.get('image_sha256')
        or header.get('accepted_c68_physical_sha256')!=classifier.get('accepted_c68_physical_sha256')
        or header.get('accepted_c68_result_identity')!=classifier.get('accepted_c68_result_identity')
        or header.get('accepted_c68_authentication_identity')!=classifier.get('accepted_c68_authentication_identity')
        or header.get('c69_classifier_identity')!=result.get('classifier_identity')
        or header.get('c69_classifier_identity')!=c56.digest(classifier)
        or header.get('tool_sources')!=sources()
        or classifier.get('tool_sources')!=sources()
        or header.get('limits')!=classifier.get('c68_limits')
        or header.get('limits')!=asdict(c58.Limits())
        or c56.digest(result)!=header.get('c69_result_identity')
        or type(expected_domain)!=int or expected_domain<0
        or result.get('post_c67_uncertified_boundary_refused_roots_expected')!=expected_domain
        or result.get('post_c67_uncertified_boundary_refused_roots_classified')!=expected_domain
        or result.get('post_c67_boundary_instances_total')!=expected_domain
        or len(records)!=expected_domain
        or len(records)!=header.get('record_count')):
        raise c58.EvidenceError('stale/wrong c69 checkpoint header/result')

    stream = hashlib.sha256()
    seen = set()
    for record in records:
        if type(record)!=dict:
            raise c58.EvidenceError('stale/wrong c69 checkpoint record')
        record_identity = record.get('record_identity')
        if (not isinstance(record_identity,str) or record_identity in seen
            or c56.digest({k:v for k,v in record.items() if k!='record_identity'})!=record_identity
            or record.get('classifier_identity')!=header.get('c69_classifier_identity')
            or record.get('accepted_c68_authentication_identity')!=header.get('accepted_c68_authentication_identity')
            or record.get('accepted_c68_result_identity')!=header.get('accepted_c68_result_identity')):
            raise c58.EvidenceError('stale/wrong c69 checkpoint record')
        seen.add(record_identity)
        stream.update(c56.encoded(record)+b'\n')
    if (stream.hexdigest()!=header.get('record_stream_sha256')
        or stream.hexdigest()!=result.get('authenticated_record_set_sha256')):
        raise c58.EvidenceError('wrong c69 checkpoint record set identity')
    return value


base.sources = sources
base.make_checkpoint = make_checkpoint
base.validate_checkpoint = validate_checkpoint


def main():
    return base.main()


if __name__=='__main__':
    raise SystemExit(main())
