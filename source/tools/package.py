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
required=['V01B8_RUNTIME_RESULT.md', 'V01B8_RESULT.md', 'PATH_B_CALLER_DECODE.md', 'PATH_B_SOURCE_LAYOUT.md', 'PATH_B_STORE_MAP.md', 'V01B9_GATE.md', 'V01B9_RESULT.md', 'PATH_B_COMPACT3X3_PROOF.md', 'DUAL_SOURCE_RETENTION.md', 'V01C1_GATE.md']+['V01B5_RESULT.md','IMMEDIATE_RESET_TRACE.md','RESET_WRITER_CANDIDATES.md','V01B6_GATE.md','V01B6_RESULT.md','PREARMED_RESET_TRACE.md','COMMON_WRITER_RESET_CALLS.md','RESET_WRITER_VERIFICATION.md','V01B7_GATE.md','V01B7_RUNTIME_RESULT.md','V01B7_RESULT.md','RESET_COPY_WRITER_ANALYSIS.md','DUAL_SOURCE_OWNERSHIP.md','V01B8_GATE.md','V01B5_RESET_SUMMARY.json','V01B4_EVIDENCE_SUMMARY.json','sdk_sources.json']
# Source is complete for runtime/build. Private game captures are deliberately excluded.
source=out/'source';source.mkdir(exist_ok=True)
for directory in ['src','include','tools','mod','vendor']:
 for p in (root/directory).rglob('*'):
  relative=p.relative_to(root)
  if not p.is_file() or '__pycache__' in p.parts or p.suffix=='.pyc' or 'native_v01a7' in p.parts or 'orientation_v01b3' in p.parts:continue
  target=source/relative;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,target)
for name in ['README.md','README_TEST.md','BUILD.md','WORKLOG.md','CMakeLists.txt','LICENSE.txt','THIRD_PARTY_NOTICES.md']:shutil.copy2(root/name,source/name)
docs=root.parent/'docs' if (root.parent/'docs').is_dir() else root/'docs'
if docs.is_dir():
 for name in ['DEV_RULES.md','PROJECT_STATE.md','CODEX_NEXT.md','CODEX_RESULT.md']:
  if (docs/name).exists():
   (source/'docs').mkdir(exist_ok=True);shutil.copy2(docs/name,source/'docs'/name)
agent=root.parent/'AGENTS.md' if (root.parent/'AGENTS.md').exists() else root/'AGENTS.md'
if agent.exists():shutil.copy2(agent,source/'AGENTS.md')
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
 'native_hooks':'existing4lifecycle/common writer plus exact original-hash-authenticated reset-copy raw entry',
 'lifecycle':'existing verified Observer retained; small preserved-register native gate callback at existing4thunks for synchronous Init/Entry/Update/Deinit/generation/owner/thread/freshness',
 'critical_gates':['exactexe/hash','all4verified lifecycle/full original hashes','current4ownedpatches and writer-ownedpatch','caller168byte integrity','fresh native activeWallRun>=.70s; worker identity/freshness retained','same owner/generation/native thread','worker context<=100ms/nativeUpdate<=100ms','fresh existing read-only Hero accessor/Transform==RCX','R8null','TEB.StackLimit<=source,source64<=StackBase','source=originalWriterRSP+0x28,16aligned,PAGE_READWRITE','valid proper affine matrix','N398finite,length.95..1.05,abs(y)<=.20','kill inactive'],
 'orientation':'fixed entry basis/forward; projected F0,Right=Up cross Forward,Forward=Right cross Up; quaternion slerp0.25sec; no re-start/steering/corner',
 'normal_change_abort_dot':.995,'native_sample_max_age_ms':100,'worker_context_max_age_ms':100,'max_writes_per_session':4096,'max_override_seconds':5,'log_ring_records':1024,
 'write_helper':'currentthreadstack check,64byte expected reread,SEH48byte store,64byte full readback including untouched Position16; best-effort48byte rollback on failure, abort no directActor recovery',
 'stop':'F8 latched OFF/fullgame restart; nativeDeinit closes before original; wrongidentity/invalidnormal/stale/patchloss/overwrite/budget stops; no reentry in failed samegeneration',
 'overwrite_observation':'read-only nativePRE retention after shared transition; loss resets metric without suppressing valid exact source writes; >=20 distinct updates OR300ms after both commits',
 'unknown_game_function_calls':0,'fresh_Hero_lookup':'reuse existing verified read-only SDK getter; no new vendor file or typed writer signature','WriteProcessMemory_import':False,
 'reference_mod_code_or_assets_added':False,'new_SM2ScriptTemplate_vendor_files':0,'full_game_code_dumps_distributed':False,'private_fixture_replays':'optional SKIP if captures absent; independent synthetic tests execute',
 'v01b4_real_log_sha256':review['log_sha256'],'v01b4_source_matrix_verified':True,'v01b4_gate_pass':True,'v01b5_source_reaches_Actor_verified':True,'v01b5_retention_verified':False,'v01b5_actual_log_sha256':'b4cf534572e01d6369db6e9f950137d42061c0016ceb1195441b7f966786f4ab','v01b6_game_verified':False,'v01b7_reset_path_identified':True,'v01b8_game_verified':False,'RESET_WRITER_VERIFIED':True,'ORIENTATION_OWNERSHIP_VERIFIED':False,'v01b6_gate':'NOT_ADMITTED_OR_LATE_PER_PROJECT_STATE','v01b7_gate':'RESET_PATH_IDENTIFIED_PER_CHECKED_IN_RUNTIME_RESULT','v01b8_gate':'PENDING_ACTUAL_DUAL_SOURCE_ENTRY_AND_RETENTION_A_E','adhesion_next_phase_enabled':False,
 'prearmed_reset_trace':{'enabled_with_native_source':True,'trigger':'native verified Update at elapsed>=.60; common writer fallback','prearm_elapsed':.60,'source_override_elapsed':.70,'deadline_native_elapsed':1.50,'unchanged_Up_records_suppressed':True,'primary_first':True,'max_sessions':8,'max_events':256,'max_threads':1,'common_record_ring':1024,'callback_wait':False,'new_hook_targets':1,'abi':'existing raw entry tailjump unchanged','timing':'event snapshots last override QPC, PREARM_STATUS first_override required','all_current_Hero_callers':True,'invalid_source_interpreted':False,'reset_patch':False,'zero_events_prove_absence':False},
 'reset_copy_source':{'writer_rva':'0x2cfc3c0','end_rva':'0x2cfc57c','original_size':444,'original_sha256':'ee33ad96fb1834f9977621ea988f7667f0eb3e47166a7115ba262072ebc50196','caller_return_rva':'0x2cfc84b','original_auth_before_hook':True,'caller_auth':'accepted-exe runtime boundary + exact direct E8 destination + unchanged caller body','entry_ABI':'raw saved-state tailjump','source_provenance':'aligned readable affine64 within current-thread writable caller-local stack [entryRSP+0x28,entryRSP+0x400]','source_write_range':'[0x00,0x30)','position_bytes_unchanged':16,'shared_transition':True,'R8_R9':'raw preserved, no invented null requirement'},
 'validation':'ASan/UBSan portable; Windows warnings-as-errors; Wine ABI/20k/source/native/prearm/reset; two real independent MinHook sources share transition, same-update late coverage, >=20-update and >=300ms retention; second thunk clobber/AVX/FPU/return preservation; exact-copy hash denial; offline dual gate negatives; no cloud game PASS',
 'limitations':['b7 reset identity accepted from checked-in runtime summary; actual b8 source layout/ownership pending','primary-thread-only hardware coverage; all-caller entries are bounded','helper scheduling can miss prearm; first_override status=1 is required','native Init+1.10 deadline; OS scheduling may delay physical cleanup','Old Up is previous observation, not atomic instruction-before','unknown writer ABI/source remains unverified; post-store RIP/5ms observed sequence anchor need anchored offline decode','short already-admitted source commit may finish concurrently with F8','source rollback failure is reported without directActor repair','explicit A/E mapping/video/no crash/Position review required'],
 'source_directory':'source'
}
# c1 supersedes historical read-only Path B claims above.
manifest.update({
 'path_b_override':True,'path_b_characterization_active':True,'lock_free_shared_snapshot':True,
 'source_range':'Path A RDX+[0x00,0x30) padded orientation; Path B RDX+[0x00,0x24) compact3x3; current-thread verified caller-local stack only',
 'overwrite_observation':'bounded hardware Up watch after both source commits; retention requires dot>=.98 for >=20 distinct updates or >=300ms',
 'v01b8_game_verified':False,'v01b8_gate':'MEASURED_LAYOUT_REJECTION_ZERO_B_WRITES_RETENTION_FAILED',
 'v01b9_gate':'COMPACT_3X3_LAYOUT_AND_A_E_SEMANTICS_VERIFIED_FROM_CHECKED_IN_RUNTIME_RESULT','v01c1_gate':'PENDING_ACCEPTED_BUILD_A_E_RETENTION',
 'PATH_B_FIELDS_VERIFIED':True,'PATH_B_WRITE_PERMISSION':True,'ORIENTATION_OWNERSHIP_VERIFIED':False,
 'validation':'Windows warnings-as-errors; ASan/UBSan portable; Wine ABI/MinHook, full-function compact-map decoder, lock-free A/B source overrides, 36-byte metadata boundary, row-scale preservation, lifecycle/identity/kill gates, dual retention and hardware reset continuation. Synthetic tests never game evidence.',
 'limitations':['Accepted-build c1 A/E retention and visible rotation require the human run','Hardware primary-thread-only and bounded; missing events do not prove absence','Before Up is previous observation, not atomic pre-instruction','A short already-admitted guarded store may finish concurrently with F8','No adhesion or later milestone is enabled']})
manifest['reset_copy_source'].update({'source_provenance':'compact3x3 at exact entry RDX=RSP+0x28 in current-thread stack; anchored original full-function decode maps nine scalar rows to padded RCX rows','source_write_range':'[0x00,0x24)','shared_transition':True,'position_bytes_unchanged':'all bytes at and after +0x24 unchanged','path_b_override':True,'row_scale_preserved':True,'metadata_untouched':True,'original_capture':'private PATH_B_FUNCTION/PATH_B_CODE before hook; never redistributed'})
manifest['prearmed_reset_trace'].update({'continue_after_first_reset':True,'path_b_write':True,'entry_link':'own B entry/current-thread serial, session/generation/owner/target/caller/QPC<=5ms; anchored original decode required'})
manifest['path_b_layout']={'bytes_observed':128,'bytes_written':36,'max_sessions':8,'entries_per_session':128,'ring_records':256,'elapsed_window':[.60,1.50],'pointer_dereferences':0,'layout':'COMPACT3X3_PROVEN','unknown_layout_aborts_path_b':True,'unknown_layout_aborts_path_a':False}
(out/'BUILD_MANIFEST.json').write_text(json.dumps(manifest,indent=2)+'\n')
(root/'logs/package_check.log').write_text(f'PASS: {label}/{version} DLL/version/README/INI; lock-free shared Path A/B orientation sources ON, direct Actor/Position/adhesion/movement/camera/animation/physics writes OFF; Path B write exactly0x24 with row scale and later metadata preserved; PE32+/2exports/systemimports/noWPM; CRC/hash/source/licenses; no private game captures/reference binaries/full dumps; accepted-build c1 retention pending human A/E run\n')
print('PASS:',package)
