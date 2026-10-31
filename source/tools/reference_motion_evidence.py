"""Private offline ScriptedFlight dependency-verification evidence, not game offsets."""
import argparse
import hashlib
import io
import json
from pathlib import Path
import zipfile
import ground_support_code_refs as refs

REFERENCE_SHA = '7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662'
LABELS = ('UpdateMotion.call->RequestSimpleMotion',
          'GravityOrientationOwner.call->SetGravityVector')
MAX_ARCHIVES = 64
MAX_CONTAINER = 256*1024*1024
MAX_TOTAL = 512*1024*1024
MAX_MEMBERS = 16384
MAX_DEPTH = 3


def rank_label(name):
    found = [i for i, label in enumerate(LABELS) if label in name]
    return (min(found), 'reference_dependency_verification_label_not_native_method') if found else None


def analyze(data, expected_sha=REFERENCE_SHA):
    result = refs.investigate(data, expected_sha, needles=LABELS, ranker=rank_label)
    result.update(domain='ScriptedFlight_reference_DLL_RVAs_NOT_game_RVAs',
                  native_binding_proven=False, support_frame='UNKNOWN', gravity_frame='UNKNOWN',
                  goal='inspect_reference_dependency_verification_and_resolver_not_execute_or_port',
                  reference_sha256=result.pop('exe_sha256'))
    return result


def acquire(roots, *, expected_sha=REFERENCE_SHA):
    """Bounded named package discovery; no network, game reads, extraction or execution."""
    visited = 0; total = 0; matches = {}; examined = 0
    def inspect(data, depth):
        nonlocal visited, total, examined
        if len(data) > MAX_CONTAINER:
            raise refs.meta.Refusal('reference_container_cap')
        total += len(data)
        if total > MAX_TOTAL:
            raise refs.meta.Refusal('reference_aggregate_byte_cap')
        if data[:2] == b'MZ':
            examined += 1
            digest = hashlib.sha256(data).hexdigest()
            if digest == expected_sha:
                matches[digest] = data
            return
        if depth > MAX_DEPTH:
            raise refs.meta.Refusal('reference_nesting_cap')
        visited += 1
        if visited > MAX_ARCHIVES:
            raise refs.meta.Refusal('reference_archive_cap')
        try:
            with zipfile.ZipFile(io.BytesIO(data)) as z:
                members = z.infolist()
                if len(members) > MAX_MEMBERS:
                    raise refs.meta.Refusal('reference_member_cap')
                for member in members:
                    name = member.filename.lower()
                    if member.is_dir() or not (name.endswith('scriptedflight.dll') or name.endswith(('.script','.zip','.modular'))):
                        continue
                    if member.file_size > MAX_CONTAINER or member.flag_bits & 1:
                        raise refs.meta.Refusal('oversize_or_encrypted_reference_member')
                    with z.open(member) as f:
                        content = f.read(MAX_CONTAINER+1)
                    inspect(content, depth+1)
        except (zipfile.BadZipFile, RuntimeError, NotImplementedError) as e:
            raise refs.meta.Refusal('reference_archive_unreadable') from e
    # Only named packages/DLLs, not unrelated mods. No arbitrary symlink traversal.
    candidates = []
    walked = 0
    for root in roots:
        root = Path(root)
        if root.is_symlink():
            raise refs.meta.Refusal('reference_root_symlink')
        if not root.exists():
            continue
        entries = (root,) if root.is_file() else root.rglob('*')
        for path in entries:
            walked += 1
            if walked > 100000:
                raise refs.meta.Refusal('reference_file_inventory_cap')
            if path.is_symlink() or not path.is_file():
                continue
            low = path.name.lower()
            if low == 'scriptedflight.dll' or ('scripted' in low and 'flight' in low and path.suffix.lower() in ('.zip','.script','.modular')):
                candidates.append(path)
                if len(candidates) > MAX_ARCHIVES:
                    raise refs.meta.Refusal('reference_package_cap')
    for path in sorted(set(candidates)):
        with path.open('rb') as f:
            inspect(f.read(MAX_CONTAINER+1),0)
    if not matches:
        return dict(schema=1,result='REFERENCE_UNAVAILABLE_OR_DIFFERENT_SHA',
                    expected_reference_sha256=expected_sha, named_packages=len(candidates),
                    examined_dlls=examined, live_reads=0, native_binding_proven=False,
                    limitation='only named Scripted Flight ZIP/script/modular or exact DLL; no broad game or WR name scan')
    result = analyze(next(iter(matches.values())), expected_sha)
    result.update(named_packages=len(candidates), examined_dlls=examined,
                  archives_inspected=visited, duplicate_exact_copies_deduplicated=True)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', action='append', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    try:
        result = acquire(args.root)
    except (OSError,ValueError,zipfile.BadZipFile) as e:
        result = dict(schema=1,result='REFUSED',reason=str(e) if isinstance(e,refs.meta.Refusal) else 'reference_input_unavailable',
                      native_binding_proven=False, live_reads=0)
    encoded = json.dumps(result,indent=2)+'\n'
    if len(encoded.encode()) > refs.MAX_OUTPUT:
        encoded = json.dumps(dict(schema=1,result='REFUSED',reason='reference_output_cap'))+'\n'
        result['result']='REFUSED'
    with args.output.open('x',encoding='utf-8') as f:
        f.write(encoded)
    print(result['result'],args.output)
    return 0 if result['result']=='CODE_REFERENCE_CANDIDATES' else 2

if __name__=='__main__':
    raise SystemExit(main())
