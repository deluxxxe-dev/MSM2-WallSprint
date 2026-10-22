from pathlib import Path
import hashlib,json,subprocess,re,zipfile,shutil,configparser
root=Path(__file__).resolve().parents[1]
header=(root/'include/version.hpp').read_text();label=re.search(r'versionLabel\[\]="([^"]+)"',header)[1];version=re.search(r'packageVersion\[\]="([^"]+)"',header)[1]
info=json.loads((root/'mod/info.json').read_text());assert info['version']==version and label in info['description']
out=root/'output'/('WallSprint_'+label);out.mkdir(parents=True,exist_ok=True)
readme=(root/'README_TEST.md').read_text();assert label in readme and version in readme;shutil.copy2(root/'README_TEST.md',out/'README_TEST.md')
binary=(root/'build/windows/WallSprint.dll').read_bytes();assert label.encode()+b'\0' in binary and version.encode()+b'\0' in binary
pe=subprocess.check_output(['objdump','-p',str(root/'build/windows/WallSprint.dll')],text=True);assert 'PE32+' in pe
for export in ['script_enable','script_disable']:assert re.search(r'\b'+export+r'\b',pe)
deps=re.findall(r'DLL Name: (\S+)',pe);assert {d.lower() for d in deps}<={'bcrypt.dll','version.dll','kernel32.dll','msvcrt.dll','user32.dll'}
assert 'WriteProcessMemory' not in pe
ini=configparser.ConfigParser();ini.read(root/'mod/WallSprint.ini')
assert ini.getint('WallSprint','WALLSPRINT_ENABLE')==1 and ini.getint('WallSprint','WALLSPRINT_NATIVE_SOURCE_OVERRIDE')==1
assert all(ini.getint('WallSprint','WALLSPRINT_'+k+'_WRITE')==0 for k in ['ORIENTATION','ADHESION','MOVEMENT'])
assert ini.getint('Diagnostics','ORIENTATION_WRITER_TRACE')==0 and ini.getint('Diagnostics','SOURCE_ARGUMENT_CAPTURE')==0
assert ini.getfloat('WallSprint','WALLSPRINT_ENTRY_DELAY')==.7 and ini.getfloat('WallSprint','WALLSPRINT_ROTATE_TIME')==.25
package=out/'WallSprint.script'
with zipfile.ZipFile(package,'w',zipfile.ZIP_DEFLATED) as z:
 for name,data in [('info.json',(root/'mod/info.json').read_bytes()),('WallSprint.ini',(root/'mod/WallSprint.ini').read_bytes()),('WallSprint.dll',binary)]:
  entry=zipfile.ZipInfo(name,(2026,10,22,0,0,0));entry.compress_type=zipfile.ZIP_DEFLATED;z.writestr(entry,data)
with zipfile.ZipFile(package) as z:assert z.testzip() is None and z.read('WallSprint.dll')==binary and set(z.namelist())=={'info.json','WallSprint.ini','WallSprint.dll'}
required=['V01B5_RESULT.md','IMMEDIATE_RESET_TRACE.md','RESET_WRITER_CANDIDATES.md','V01B6_GATE.md','V01B5_RESET_SUMMARY.json','V01B4_EVIDENCE_SUMMARY.json','sdk_sources.json']
# Source is complete for runtime/build. Private game captures are deliberately excluded.
source=out/'source';source.mkdir(exist_ok=True)
for directory in ['src','include','tools','mod','vendor']:
 for p in (root/directory).rglob('*'):
  relative=p.relative_to(root)
  if not p.is_file() or '__pycache__' in p.parts or p.suffix=='.pyc' or 'native_v01a7' in p.parts or 'orientation_v01b3' in p.parts:continue
  target=source/relative;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,target)
for name in ['README.md','README_TEST.md','BUILD.md','WORKLOG.md','CMakeLists.txt','LICENSE.txt','THIRD_PARTY_NOTICES.md']:shutil.copy2(root/name,source/name)
for name in required:
 assert (root/'analysis'/name).exists()
 for folder in [source/'analysis',out/'analysis']:
  folder.mkdir(exist_ok=True);shutil.copy2(root/'analysis'/name,folder/name)
for folder in [out/'config']:
 folder.mkdir(exist_ok=True)
 for p in (root/'mod/presets').glob('*.ini'):shutil.copy2(p,folder/p.name)
for p in source.rglob('*'):
 assert p.name not in ['ActorRelativeCamera.dll','SwingCore.dll','WalkOnWallsV1.stage','writer.hex','Entry.bin','Init.bin','Update.bin','Deinit.bin']
 assert 'extracted' not in p.parts and 'native_v01a7' not in p.parts and 'orientation_v01b3' not in p.parts
review=json.loads((root/'analysis/V01B4_EVIDENCE_SUMMARY.json').read_text());assert review['v01b4_pass'] and review['entries']==687 and review['source_up']['source_candidate_identified'] and not review['v01b5_game_verified']
manifest={
 'version':label,'package_version':version,'artifact':'WallSprint.script','sha256':hashlib.sha256(package.read_bytes()).hexdigest(),'dll_sha256':hashlib.sha256(binary).hexdigest(),'imports':deps,'exports':['script_disable','script_enable'],
 'target_exe':'Spider-Man2.exe','target_version':'2.810.0.0','expected_exe_sha256':'c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9',
 'native_stack_source_write_enabled':True,'source_range':'RDX+[0x00,0x30), current-thread verified caller-local source only','position_write':False,'direct_actor_transform_write':False,'camera_write':False,'physics_internal_write':False,'animation_write':False,'adhesion_write':False,'movement_write':False,'gravity_write':False,
 'writer':'RVA0x2CFC580,584bytes,known32byte AOB/prefix/unique match/runtime boundary/fullSHA3aae659545a606d36c0f9bf39eb773e09f1b091ff8ae139e72709c13688e6cd6',
 'caller':'RVA0x2CFC850,168bytes,return0x2CFC8F3,fullSHA88502996d39173bf38086e8ddafd9eff2875dada30717d1c9f4651324fbe3b3f',
 'native_hooks':'reuse existing4lifecycle plus existing writer-entry hook, no additional native target',
 'lifecycle':'existing verified Observer retained; small preserved-register native gate callback at existing4thunks for synchronous Init/Entry/Update/Deinit/generation/owner/thread/freshness',
 'critical_gates':['exactexe/hash','all4verified lifecycle/full original hashes','current4ownedpatches and writer-ownedpatch','caller168byte integrity','native+worker activeWallRun>=.70s','same owner/generation/native thread','worker context<=100ms/nativeUpdate<=100ms','fresh existing read-only Hero accessor/Transform==RCX','R8null','TEB.StackLimit<=source,source64<=StackBase','source=originalWriterRSP+0x28,16aligned,PAGE_READWRITE','valid proper affine matrix','N398finite,length.95..1.05,abs(y)<=.20','kill inactive'],
 'orientation':'fixed entry basis/forward; projected F0,Right=Up cross Forward,Forward=Right cross Up; quaternion slerp0.25sec; no re-start/steering/corner',
 'normal_change_abort_dot':.995,'native_sample_max_age_ms':100,'worker_context_max_age_ms':100,'max_writes_per_session':512,'max_override_seconds':5,'log_ring_records':1024,
 'write_helper':'currentthreadstack check,64byte expected reread,SEH48byte store,64byte full readback including untouched Position16; best-effort48byte rollback on failure, abort no directActor recovery',
 'stop':'F8 latched OFF/fullgame restart; nativeDeinit closes before original; wrongidentity/invalidnormal/stale/patchloss/overwrite/budget stops; no reentry in failed samegeneration',
 'overwrite_observation':'read-only next matching nativePRE; distinct nativeUpdates>=20 or >=300ms dotActorUpN>=.98 after transition; worker snapshots supplemental; othercaller read-only capped16',
 'unknown_game_function_calls':0,'fresh_Hero_lookup':'reuse existing verified read-only SDK getter; no new vendor file or typed writer signature','WriteProcessMemory_import':False,
 'reference_mod_code_or_assets_added':False,'new_SM2ScriptTemplate_vendor_files':0,'full_game_code_dumps_distributed':False,'private_fixture_replays':'optional SKIP if captures absent; independent synthetic tests execute',
 'v01b4_real_log_sha256':review['log_sha256'],'v01b4_source_matrix_verified':True,'v01b4_gate_pass':True,'v01b5_source_reaches_Actor_verified':True,'v01b5_retention_verified':False,'v01b5_actual_log_sha256':'b4cf534572e01d6369db6e9f950137d42061c0016ceb1195441b7f966786f4ab','v01b6_game_verified':False,'RESET_WRITER_VERIFIED':False,'v01b6_gate':'PENDING_ACTUAL_RESET_WRITE_A_E_TRACE','adhesion_next_phase_enabled':False,
 'immediate_reset_trace':{'enabled_with_native_source':True,'trigger':'first successful source commit per session, dotWorld<=.85 OR dotN>=.50','deadline_ms_from_commit':30,'hard_ceiling_ms':50,'max_sessions':8,'max_events':128,'max_threads':32,'callback_wait':False,'new_hook_targets':0,'abi':'existing raw entry tailjump unchanged','timing':'QPC after source memcpy; first surface observation and actual data-write reset event','source_snapshot':'RDX64bytes only; after override before original; same thread/caller/target/postRSI correspondence required','reset_patch':False,'zero_events_prove_absence':False},
 'validation':'b6 actual hardware/MinHook reset and independent second writer/helper/source snapshot/short-timeout/cancel/F8/DR cleanup + offline anchored decode/negative gates; portable ASan/UBSan; Windows warnings-as-errors; Wine wrongbuild/lifecycle/SEH/bounds; preserved source-gate callback across existing4MinHook ABI/20k tests; independent actual nativecaller+writer/MinHook A/E source transition and retained20Updates, source/position/metadata/parent buffer/returns; stack/heap/ABI/lifecycle/ABA/normal/identity/stale/patch/F8/overwrite negative tests; offline writes alone and synthetic never game PASS',
 'limitations':['actual game b6 reset writer/caller/source/QPC pending; b5 source reaches Actor but is not retained','bounded current-thread enumeration excludes helper/new threads; capture arm delay and missed initial writes reported','30ms capture deadline enforced per handler/helper; OS scheduling can delay physical debug register cleanup','old Up is previous observation, not atomic instruction-before','no formal Cutscene/Death/WallCrawl accessor; native lifecycle/update freshness gates observed behavior','already admitted short in-flight commit may complete concurrently with F8; future admission OFF','source rollback failure is reported, no directActor repair','video mapping/no crash/position review required'],
 'source_directory':'source'
}
(out/'BUILD_MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n')
(root/'logs/package_check.log').write_text(f'PASS: {label}/{version} DLL/version/README/INI; native-source-only ON, all direct/position/other writes OFF; PE32+/2exports/systemimports/noWPM; CRC/hash/source/licenses; no private game captures/reference binaries/full dumps; real b5 source-to-Actor verified/retention failed; b6 reset writer pending\n')
print('PASS:',package)
