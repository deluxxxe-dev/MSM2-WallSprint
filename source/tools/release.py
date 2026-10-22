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
for name in ['V01B5_RESULT.md','V01B5_RESET_SUMMARY.json','IMMEDIATE_RESET_TRACE.md','RESET_WRITER_CANDIDATES.md','V01B6_GATE.md','V01B6_RESULT.md','PREARMED_RESET_TRACE.md','COMMON_WRITER_RESET_CALLS.md','RESET_WRITER_VERIFICATION.md','V01B7_GATE.md','V01B7_RUNTIME_RESULT.md','V01B7_RESULT.md','RESET_COPY_WRITER_ANALYSIS.md','DUAL_SOURCE_OWNERSHIP.md','V01B8_GATE.md']:
 shutil.copy2(root/'analysis'/name,out/'analysis'/name)
(out/'logs').mkdir(exist_ok=True)
# Only known independent cloud validation logs; no raw private game logs.
for name in ['v01b8_build.log','v01b8_portable.log','v01b8_smoke.log','v01b8_hooks.log','windows_reset_copy_abi_smoke.log','package_check.log']:
 if (root/'logs'/name).exists():shutil.copy2(root/'logs'/name,out/'logs'/name)
 else:print('SKIP: optional maintainer validation log',name)
if (root.parent/'docs/CODEX_RESULT.md').exists():
 (out/'docs').mkdir(exist_ok=True);shutil.copy2(root.parent/'docs/CODEX_RESULT.md',out/'docs/CODEX_RESULT.md')
# All corresponding tools, fixtures and license copies remain in source/.
result=[]
for name,folder in [('WallSprint_'+label+'_dual_source_ownership.zip',out),('WallSprint_'+label+'_source.zip',out/'source')]:
 path=root.parent/name
 with zipfile.ZipFile(path,'w',zipfile.ZIP_DEFLATED) as z:
  for p in sorted(folder.rglob('*')):
   if p.is_file():z.write(p,p.relative_to(folder.parent))
 with zipfile.ZipFile(path) as z:assert z.testzip() is None
 result.append({'path':str(path),'bytes':path.stat().st_size,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
(root/'logs/v01b8_release.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
