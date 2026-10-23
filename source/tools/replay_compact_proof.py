"""Private accepted-build replay. Never print or copy captured game bytes to outputs."""
import argparse
import hashlib
import os
from pathlib import Path
import re
import subprocess
import tempfile

EXPECTED = {
    'writer': (0x2cfc3c0, 444, 'ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196'),
    'caller': (0x2cfc7d0, 128, '1665a7a329a983101673f05ccd95bbf3d738c39099469b4acfd14344ec98a79d'),
}

def captures(text):
    records = {}
    for role, (begin, size, digest) in EXPECTED.items():
        header = [line for line in text.splitlines() if 'PATH_B_FUNCTION role=' + role in line]
        if len(header) != 1:
            raise ValueError('exactly one original function record required: ' + role)
        fields = dict(re.findall(r'(\w+)=(\S+)', header[0]))
        if (int(fields['begin_rva'], 0), int(fields['size']), fields['sha256']) != (begin, size, digest):
            raise ValueError('function identity mismatch: ' + role)
        if fields.get('authenticated') != '1' or fields.get('original_before_hook') != '1':
            raise ValueError('original authenticated capture required: ' + role)
        data = bytearray()
        for line in text.splitlines():
            if 'PATH_B_CODE role=' + role not in line:
                continue
            fields = dict(re.findall(r'(\w+)=(\S+)', line))
            if int(fields['offset'], 0) != len(data):
                raise ValueError('non-contiguous or duplicate chunk: ' + role)
            data.extend(bytes.fromhex(fields['bytes']))
        if len(data) != size or hashlib.sha256(data).hexdigest() != digest:
            raise ValueError('capture length/hash mismatch: ' + role)
        records[role] = bytes(data)
    return records

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture', type=Path, required=True)
    args = parser.parse_args()
    records = captures(args.capture.read_text())
    # The exact same capture parser refuses even a one-bit mutation.
    mutated = args.capture.read_text().replace('bytes=48895c', 'bytes=49895c', 1)
    try:
        captures(mutated)
    except ValueError:
        pass
    else:
        raise AssertionError('mutated private capture was accepted')
    root = Path(__file__).resolve().parents[1]
    env = dict(os.environ, WINEPREFIX='/workspace/toolchains/wine-prefix',
               WINEDLLPATH='/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine',
               WINEDEBUG='-all', WINEDLLOVERRIDES='mscoree,mshtml=')
    with tempfile.TemporaryDirectory(prefix='wallsprint-c2-proof-') as private:
        paths = []
        for role in ('writer', 'caller'):
            data = records[role]
            path = Path(private) / (role + '.bin')
            path.write_bytes(data)
            paths.append('Z:' + path.as_posix())
            print(f'AUTHENTICATED_CAPTURE role={role} size={len(data)} sha256={hashlib.sha256(data).hexdigest()}', flush=True)
        for tool in ('c1_compact_replay', 'compact_proof_replay', 'reset_copy_abi_smoke'):
            subprocess.run(['/workspace/toolchains/wine/usr/lib/wine/wine64',
                            str(root / 'build/windows' / (tool + '.exe')), *paths],
                           cwd=root, env=env, check=True, timeout=60)

if __name__ == '__main__':
    main()
