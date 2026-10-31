"""Stage/run only the WS-owned native fixture; never read or start the game."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import uuid

BRANCH = 'codex/v01c20-edi-value-provenance'
ORIGIN = 'https://github.com/deluxxxe-dev/MSM2-WallSprint.git'
ARTIFACT = 'analysis/native-probes/presence/presence-owned.exe'
MANIFEST = 'analysis/native-probes/presence/manifest.json'


def git(source, *args):
    return subprocess.check_output(['git', '-C', str(source), *args], stderr=subprocess.PIPE)


def run(source, commit, launch=subprocess.run):
    source = Path(source).resolve(strict=True)
    if not re.fullmatch('[0-9a-f]{40}', commit):
        raise ValueError('Expected exact fetched commit SHA')
    if git(source, 'remote', 'get-url', 'origin').decode().strip() != ORIGIN:
        raise ValueError('Wrong WS origin')
    if git(source, 'branch', '--show-current').decode().strip() != BRANCH:
        raise ValueError('Wrong WS branch; no checkout attempted')
    logs = source.parent / 'logs'
    if not logs.is_dir():
        raise ValueError('Existing WS logs directory required')
    manifest = json.loads(git(source, 'show', commit + ':' + MANIFEST))
    binary = git(source, 'show', commit + ':' + ARTIFACT)
    if len(binary) != manifest['size'] or hashlib.sha256(binary).hexdigest() != manifest['sha256']:
        raise ValueError('Owned fixture size/hash mismatch; refused execution')
    if manifest['production_admitted'] is not False:
        raise ValueError('Unexpected production admission claim')
    output = logs / ('MSM2-WS-presence-owned-' + uuid.uuid4().hex + '.json')
    with tempfile.TemporaryDirectory(prefix='WS-owned-presence-') as staging:
        exe = Path(staging) / 'presence-owned.exe'
        exe.write_bytes(binary)
        # Byte-preserving Git extraction and staging outside Mods Library.
        completed = launch([str(exe), str(output)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if not output.is_file():
        raise RuntimeError('Fixture produced no JSON; exit=' + str(completed.returncode) + '; ' + completed.stderr.decode(errors='replace'))
    report = json.loads(output.read_text(encoding='utf-8'))
    if report['schema'] != 'WS-owned-presence-fixture-v1' or report['production_admitted'] is not False:
        raise ValueError('Unexpected report schema/admission')
    assertions = report['assertions']
    if report['pass'] != sum(x['pass'] is True for x in assertions) or report['fail'] != sum(x['pass'] is False for x in assertions):
        raise ValueError('Inconsistent assertion accounting')
    report['fixture_commit'] = commit
    report['fixture_sha256'] = manifest['sha256']
    report['native_result'] = ('UNAVAILABLE' if report['environment'] != 'native Windows' else 'PASS' if report['fail'] == 0 and completed.returncode == 0 else 'FAIL')
    # Keep original assertions even on failure; do not promote Wine to native PASS.
    with output.open('w', encoding='utf-8', newline='\n') as stream:
        json.dump(report, stream, indent=2)
        stream.write('\n')
    print('Owned fixture:', report['native_result'], 'PASS=' + str(report['pass']), 'FAIL=' + str(report['fail']))
    print('Return this JSON:', output)
    print('No game operation. Runtime observer NOT installed/admitted.')
    if completed.returncode:
        raise RuntimeError('Owned fixture assertion/runtime failure; return the JSON above')
    return output


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', required=True)
    parser.add_argument('--commit', required=True)
    args = parser.parse_args()
    if os.name != 'nt':
        parser.error('Run with Windows py -3; no WSL/game fallback')
    run(args.source, args.commit)


if __name__ == '__main__':
    main()
