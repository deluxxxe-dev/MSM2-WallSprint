"""Package existing validated build, with sources, analyses, licenses and selected logs."""
from pathlib import Path
import subprocess,hashlib,zipfile,shutil,re,json
root=Path(__file__).resolve().parents[1]
subprocess.run(['python',str(root/'tools/package.py')],cwd=root,check=True)
label=re.search(r'versionLabel\[\]="([^"]+)"',(root/'include/version.hpp').read_text())[1]
out=root/'output'/('WallSprint_'+label)
for name in ['LICENSE.txt','THIRD_PARTY_NOTICES.md']:
 shutil.copy2(root/name,out/name)
(out/'analysis').mkdir(exist_ok=True)
for name in ['V01C12_RUNTIME_RESULT.md', 'MOVEMENT_WRITER_ENTRY_SNAPSHOT.md', 'MOVEMENT_ENTRY_SOURCE_CANDIDATES.md', 'V01C13_GATE.md', 'V01C11_RUNTIME_RESULT.md', 'MOVEMENT_WRITER_2CFC900.md', 'MOVEMENT_CALLER_351EEB0.md', 'MOVEMENT_SOURCE_LINEAGE.md', 'V01C12_GATE.md', 'V01C10_RUNTIME_RESULT.md', 'POSITION_WRITER_ATTRIBUTION.md', 'PATH_A_POSITION_CAUSALITY.md', 'MOVEMENT_SOURCE_OWNERSHIP.md', 'V01C11_GATE.md', 'V01C9_RUNTIME_RESULT.md', 'V01C9_E_RUNTIME_RESULT.md', 'V01C9_E_RESULT.md', 'SECONDARY_PUBLICATION_RACE.md', 'V01C10_GATE.md', 'V01C8_RUNTIME_RESULT.md', 'V01C8_RESULT.md', 'SECONDARY_PATH_B_THREAD.md', 'V01C9_GATE.md', 'V01C7_RUNTIME_RESULT.md', 'V01C7_RESULT.md', 'HARD_KILL_ATTRIBUTION.md', 'V01C8_GATE.md', 'V01C6_RUNTIME_RESULT.md', 'V01C6_RESULT.md', 'TRANSIENT_FRESHNESS_RECOVERY.md', 'V01C7_GATE.md', 'V01B8_RUNTIME_RESULT.md', 'V01B8_RESULT.md', 'PATH_B_CALLER_DECODE.md', 'PATH_B_SOURCE_LAYOUT.md', 'PATH_B_STORE_MAP.md', 'V01B9_GATE.md', 'V01B9_RESULT.md', 'PATH_B_COMPACT3X3_PROOF.md', 'DUAL_SOURCE_RETENTION.md', 'V01C1_GATE.md', 'V01C1_RUNTIME_RESULT.md', 'V01C1_RESULT.md', 'PATH_B_PROOF_DECODER_FIX.md', 'V01C2_GATE.md', 'V01C2_RUNTIME_RESULT.md', 'V01C2_RESULT.md', 'THIRD_WRITER_TRACE.md', 'V01C3_GATE.md', 'V01C3_RUNTIME_RESULT.md', 'V01C3_RESULT.md', 'REAL_THREAD_ENUM_FIX.md', 'V01C4_GATE.md', 'V01C4_RUNTIME_RESULT.md', 'V01C4_RESULT.md', 'SOURCE_LIFECYCLE_CONTINUITY.md', 'V01C5_GATE.md', 'V01C5_RUNTIME_RESULT.md', 'V01C5_RESULT.md', 'PATH_A_POSITION_PROOF.md', 'PLANAR_ADHESION.md', 'V01C6_GATE.md']+['V01B5_RESULT.md','V01B5_RESET_SUMMARY.json','IMMEDIATE_RESET_TRACE.md','RESET_WRITER_CANDIDATES.md','V01B6_GATE.md','V01B6_RESULT.md','PREARMED_RESET_TRACE.md','COMMON_WRITER_RESET_CALLS.md','RESET_WRITER_VERIFICATION.md','V01B7_GATE.md','V01B7_RUNTIME_RESULT.md','V01B7_RESULT.md','RESET_COPY_WRITER_ANALYSIS.md','DUAL_SOURCE_OWNERSHIP.md','V01B8_GATE.md']:
 shutil.copy2(root/'analysis'/name,out/'analysis'/name)
(out/'logs').mkdir(exist_ok=True)
# Only known independent cloud validation logs; no raw private game logs.
for name in ['v01c13_build.log','v01c13_portable.log','v01c13_smoke.log','v01c13_hooks.log','v01c13_public_build.log','v01c13_entry.log','package_check.log']:
 if (root/'logs'/name).exists():shutil.copy2(root/'logs'/name,out/'logs'/name)
 else:print('SKIP: optional maintainer validation log',name)
if (root.parent/'docs/CODEX_RESULT.md').exists():
 (out/'docs').mkdir(exist_ok=True);shutil.copy2(root.parent/'docs/CODEX_RESULT.md',out/'docs/CODEX_RESULT.md')
shutil.copy2(out/'WallSprint.script',root.parent/'WallSprint.script')
shutil.copy2(out/'BUILD_MANIFEST.json',root.parent/'BUILD_MANIFEST.json')
# All corresponding tools, fixtures and license copies remain in source/.
result=[]
for name,folder in [('WallSprint_'+label+'_writer_entry_snapshot.zip',out),('WallSprint_'+label+'_source.zip',out/'source')]:
 path=root.parent/name
 with zipfile.ZipFile(path,'w',zipfile.ZIP_DEFLATED) as z:
  for p in sorted(folder.rglob('*')):
   if p.is_file():z.write(p,p.relative_to(folder.parent))
 with zipfile.ZipFile(path) as z:assert z.testzip() is None
 result.append({'path':str(path),'bytes':path.stat().st_size,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
(root/'logs/v01c13_release.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
