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
for name in ['V01B5_RESULT.md','V01B5_RESET_SUMMARY.json','IMMEDIATE_RESET_TRACE.md','RESET_WRITER_CANDIDATES.md','V01B6_GATE.md']:
 shutil.copy2(root/'analysis'/name,out/'analysis'/name)
(out/'logs').mkdir(exist_ok=True)
for name in ['v01b6_build.log','v01b6_portable_run.log','v01b6_smoke_run.log','v01b6_hooks_run.log','windows_native_source_override_smoke.log','native_source_review_tests.log','windows_reset_trace_smoke.log','windows_reset_coordinator_smoke.log','reset_writer_review_tests.log','v01b6_scope_check.log','v01b6_final_source_regression.log','v01b6_final_native_regression.log','v01b6_final_reset_regression.log','v01b6_public_source_build.log','v01b6_public_source_native.log','v01b6_public_source_hooks.log','v01b6_public_source_portable.log','package_check.log']:
 if (root/'logs'/name).exists():shutil.copy2(root/'logs'/name,out/'logs'/name)
 else:print('SKIP: optional maintainer validation log',name)
# All corresponding tools, fixtures and license copies remain in source/.
result=[]
for name,folder in [('WallSprint_'+label+'_reset_writer_trace.zip',out),('WallSprint_'+label+'_source.zip',out/'source')]:
 path=root.parent/name
 with zipfile.ZipFile(path,'w',zipfile.ZIP_DEFLATED) as z:
  for p in sorted(folder.rglob('*')):
   if p.is_file():z.write(p,p.relative_to(folder.parent))
 with zipfile.ZipFile(path) as z:assert z.testzip() is None
 result.append({'path':str(path),'bytes':path.stat().st_size,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
(root/'logs/v01b6_release.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
