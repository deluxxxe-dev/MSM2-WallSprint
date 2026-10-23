#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
#include <exception>
#include "msm2/sdk_bridge.hpp"
#include "msm2/orientation_log.hpp"
#include "msm2/wallrun_hooks.hpp"
#include "version.hpp"
#include "msm2/actor_writer.hpp"
#include "msm2/orientation_trace.hpp"
#include "msm2/orientation_source.hpp"
#include "NativeHooks/native_source_override.hpp"
#include "NativeHooks/immediate_reset_trace.hpp"
#include "NativeHooks/reset_copy_source.hpp"
#include <cstdlib>
#include <limits>
#include <algorithm>
namespace {
HMODULE instance{};
SRWLOCK lifecycle=SRWLOCK_INIT,logLock=SRWLOCK_INIT;
HANDLE stopEvent{},workerHandle{},logFile=INVALID_HANDLE_VALUE;
DWORD workerId{};
std::wstring executablePath(){std::vector<wchar_t> path(32768);DWORD n=GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()));if(!n||n>=path.size())return {};return {path.data(),n};}
std::wstring directory(const std::wstring& path){auto pos=path.find_last_of(L"\\/");return pos==std::wstring::npos?L".":path.substr(0,pos);}
void openLog(){
 auto dir=directory(executablePath())+L"\\logs";CreateDirectoryW(dir.c_str(),nullptr);
 auto path=dir+L"\\WallSprint.log";
 // Keep the previous run, without replacing an existing backup.
 SYSTEMTIME now{};GetSystemTime(&now);wchar_t suffix[96]{};
 swprintf(suffix,96,L".%04u%02u%02u-%02u%02u%02u-%03u-%lu.bak",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,now.wMilliseconds,GetCurrentProcessId());
 if(GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES&&!MoveFileW(path.c_str(),(path+suffix).c_str())){
  // No truncation if preservation fails; use a distinct filename.
  path+=suffix;path+=L".log";
 }
 logFile=CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
 if(logFile==INVALID_HANDLE_VALUE)OutputDebugStringA("WallSprint: cannot create game-directory logs/WallSprint.log; using debug output only\n");
}
void closeLog(){AcquireSRWLockExclusive(&logLock);if(logFile!=INVALID_HANDLE_VALUE){CloseHandle(logFile);logFile=INVALID_HANDLE_VALUE;}ReleaseSRWLockExclusive(&logLock);}
bool buildIdentity(){
 bool versionMatches=false,hashMatches=false;
 auto path=executablePath();auto name=path.substr(path.find_last_of(L"\\/")+1);
 msm2::writeLog("process=%ls expected=Spider-Man2.exe (no identity restriction based only on filename)",name.c_str());
 DWORD unused=0;auto size=GetFileVersionInfoSizeW(path.c_str(),&unused);
 bool gotVersion=false;
 if(size){std::vector<std::uint8_t> data(size);if(GetFileVersionInfoW(path.c_str(),0,size,data.data())){VS_FIXEDFILEINFO* info=nullptr;UINT len=0;if(VerQueryValueW(data.data(),L"\\",reinterpret_cast<void**>(&info),&len)&&len>=sizeof(*info)&&info->dwSignature==0xfeef04bd){msm2::writeLog("exe_version=%u.%u.%u.%u",HIWORD(info->dwFileVersionMS),LOWORD(info->dwFileVersionMS),HIWORD(info->dwFileVersionLS),LOWORD(info->dwFileVersionLS));gotVersion=true;versionMatches=info->dwFileVersionMS==MAKELONG(810,2)&&info->dwFileVersionLS==0;}}}
 if(!gotVersion)msm2::writeLog("exe_version=unavailable Win32Error=%lu",GetLastError());
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));IMAGE_DOS_HEADER dos{};IMAGE_NT_HEADERS64 nt{};
 if(msm2::readMemory(base,&dos,sizeof(dos))&&dos.e_magic==IMAGE_DOS_SIGNATURE&&dos.e_lfanew>0&&dos.e_lfanew<1024*1024&&msm2::readMemory(base+dos.e_lfanew,&nt,sizeof(nt))&&nt.Signature==IMAGE_NT_SIGNATURE)msm2::writeLog("PE timestamp=0x%08lx SizeOfImage=0x%08lx machine=0x%04x",nt.FileHeader.TimeDateStamp,nt.OptionalHeader.SizeOfImage,nt.FileHeader.Machine);
 BCRYPT_ALG_HANDLE algorithm=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;DWORD objectSize=0,returned=0;std::vector<UCHAR> object;
 bool ok=BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
 if(ok)ok=BCryptGetProperty(algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&returned,0)>=0;
 if(ok){object.resize(objectSize);ok=BCryptCreateHash(algorithm,&hash,object.data(),objectSize,nullptr,0,0)>=0;}
 HANDLE file=CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,nullptr,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,nullptr);
 if(file==INVALID_HANDLE_VALUE)ok=false;
 std::vector<UCHAR> buffer(1024*1024);DWORD read=0;
 while(ok){if(WaitForSingleObject(stopEvent,0)==WAIT_OBJECT_0){ok=false;break;}if(!ReadFile(file,buffer.data(),static_cast<DWORD>(buffer.size()),&read,nullptr)){ok=false;break;}if(!read)break;ok=BCryptHashData(hash,buffer.data(),read,0)>=0;}
 UCHAR digest[32]{};if(ok)ok=BCryptFinishHash(hash,digest,sizeof(digest),0)>=0;
 if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
 if(hash)BCryptDestroyHash(hash);
 if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);
 if(ok){char text[65]{};for(unsigned i=0;i<32;++i)snprintf(text+i*2,3,"%02x",digest[i]);msm2::writeLog("exe_file_sha256=%s (on-disk executable, not relocated memory)",text);hashMatches=std::strcmp(text,"c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9")==0;}else msm2::writeLog("exe_file_sha256=unavailable");
 msm2::writeLog("SDK version=SM2ScriptTemplate source commit %s; no external SDK DLL",msm2::templateCommit);
 bool accepted=_wcsicmp(name.c_str(),L"Spider-Man2.exe")==0&&versionMatches&&hashMatches;
 msm2::writeLog("target_build_gate=%s required=Spider-Man2.exe_2.810.0.0_and_previous_user_log_SHA256",accepted?"ACCEPTED":"REJECTED");
 return accepted;
}
bool foreground(){DWORD pid=0;GetWindowThreadProcessId(GetForegroundWindow(),&pid);return pid==GetCurrentProcessId();}
bool pressed(int key,bool& wasDown){bool down=foreground()&&(GetAsyncKeyState(key)&0x8000);bool edge=down&&!wasDown;wasDown=down;return edge;}
struct Settings {bool orientation{},audit=true,surface{};unsigned interval=100;msm2::hooks::Options hooks;ws::planar::Config planar;msm2::trace::Config trace;msm2::input::Config input;msm2::native_source::Config source;};
Settings settings(){
 wchar_t modulePath[32768]{};DWORD length=GetModuleFileNameW(instance,modulePath,32768);
 std::wstring path=directory(length?std::wstring(modulePath,length):executablePath())+L"\\WallSprint.ini";
 Settings result;result.hooks.surfaceTelemetry=false;
 auto number=[&](const wchar_t* key,const wchar_t* fallback){wchar_t text[64]{};GetPrivateProfileStringW(L"WallSprint",key,fallback,text,64,path.c_str());wchar_t* end=nullptr;float value=std::wcstof(text,&end);return end&&end!=text&&!*end?value:std::numeric_limits<float>::quiet_NaN();};
 result.source.debug=GetPrivateProfileIntW(L"WallSprint",L"DEBUG_LOG",1,path.c_str())==1;
 result.source.enabled=GetPrivateProfileIntW(L"WallSprint",L"WALLSPRINT_ENABLE",0,path.c_str())==1;
 result.source.sourceOverride=GetPrivateProfileIntW(L"WallSprint",L"WALLSPRINT_NATIVE_SOURCE_OVERRIDE",0,path.c_str())==1;
 result.source.delay=number(L"WALLSPRINT_ENTRY_DELAY",L"0.70");result.source.rotateTime=number(L"WALLSPRINT_ROTATE_TIME",L"0.25");
 result.input.observe=GetPrivateProfileIntW(L"Diagnostics",L"SOURCE_ARGUMENT_CAPTURE",0,path.c_str())==1;
 result.input.enabled=GetPrivateProfileIntW(L"Diagnostics",L"ORIENTATION_SOURCE_DISCOVERY",1,path.c_str())==1;
 result.input.otherArguments=GetPrivateProfileIntW(L"Diagnostics",L"SOURCE_OTHER_ARGUMENT_SNAPSHOTS",0,path.c_str())==1;
 result.input.windowMs=GetPrivateProfileIntW(L"Diagnostics",L"SOURCE_WINDOW_MS",2000,path.c_str());
 result.trace.enabled=GetPrivateProfileIntW(L"Diagnostics",L"ORIENTATION_WRITER_TRACE",0,path.c_str())==1;
 result.trace.windowMs=GetPrivateProfileIntW(L"Diagnostics",L"WRITER_TRACE_WINDOW_MS",350,path.c_str());
 result.planar.enable=GetPrivateProfileIntW(L"WallSprint",L"WALLSPRINT_ENABLE",1,path.c_str())==1;
 result.planar.orientation=GetPrivateProfileIntW(L"WallSprint",L"WALLSPRINT_ORIENTATION_WRITE",1,path.c_str())==1;
 result.planar.adhesion=GetPrivateProfileIntW(L"WallSprint",L"WALLSPRINT_ADHESION_WRITE",0,path.c_str())==1;
 result.planar.movement=GetPrivateProfileIntW(L"WallSprint",L"WALLSPRINT_MOVEMENT_WRITE",0,path.c_str())==1;
 wchar_t timing[64]{};GetPrivateProfileStringW(L"WallSprint",L"WALLSPRINT_WRITE_TIMING",L"POST_WALLRUN_UPDATE",timing,64,path.c_str());
 result.planar.postUpdate=std::wcscmp(timing,L"POST_WALLRUN_UPDATE")==0;
 result.planar.postDiagnostic=GetPrivateProfileIntW(L"WallSprint",L"WALLSPRINT_POST_UPDATE_DIAGNOSTIC",1,path.c_str())==1;
 result.planar.debug=GetPrivateProfileIntW(L"WallSprint",L"DEBUG_LOG",1,path.c_str())==1;
 result.planar.delay=number(L"WALLSPRINT_ENTRY_DELAY",L"0.70");result.planar.rotateTime=number(L"WALLSPRINT_ROTATE_TIME",L"0.25");result.planar.standoff=number(L"WALLSPRINT_TARGET_STANDOFF",L"0.50");result.planar.maxAdhesion=number(L"WALLSPRINT_MAX_ADHESION_CORRECTION",L"0.05");result.planar.speedScale=number(L"WALLSPRINT_SPEED_SCALE",L"0.70");
 result.orientation=GetPrivateProfileIntW(L"Diagnostics",L"ORIENTATION_DIAGNOSTIC",0,path.c_str())==1;
 result.audit=GetPrivateProfileIntW(L"Diagnostics",L"REFERENCE_AOB_AUDIT",1,path.c_str())==1;
 result.hooks.enabled=GetPrivateProfileIntW(L"Diagnostics",L"WALLRUN_HOOK_DIAGNOSTIC",1,path.c_str())==1;
 const wchar_t* names[]={L"HOOK_WALLRUN_INIT",L"HOOK_WALLRUN_ENTRY",L"HOOK_WALLRUN_UPDATE",L"HOOK_WALLRUN_DEINIT"};
 for(unsigned i=0;i<4;++i)result.hooks.individual[i]=GetPrivateProfileIntW(L"Diagnostics",names[i],1,path.c_str())==1;
 result.hooks.objectDiagnostic=GetPrivateProfileIntW(L"Diagnostics",L"WALLRUN_OBJECT_DIAGNOSTIC",1,path.c_str())==1;
 result.surface=GetPrivateProfileIntW(L"Diagnostics",L"SURFACE_PROBE_DIAGNOSTIC",0,path.c_str())==1;
 result.interval=std::clamp(GetPrivateProfileIntW(L"Diagnostics",L"ORIENTATION_INTERVAL_MS",100,path.c_str()),50u,5000u);
 msm2::writeLog("config=%ls ORIENTATION_DIAGNOSTIC=%u interval_ms=%u REFERENCE_AOB_AUDIT=%u",path.c_str(),unsigned(result.orientation),result.interval,unsigned(result.audit));
 return result;
}
unsigned markerEvents(ws::diag::MarkerKeys& keys){
 unsigned numbers=0;for(unsigned i=0;i<6;++i)if(GetAsyncKeyState('1'+i)&0x8000)numbers|=1u<<i;
 return keys.update(foreground(),numbers);
}
void run(){
 openLog();msm2::writeLog("WallSprint %s package_version=%s Sustained Native Lifecycle; scope=shared_orientation_sources_and_bounded_process_thread_watch path_b_override=1 third_path_override=0 hook_mode=build_scoped_verified_lifecycle",ws::versionLabel,ws::packageVersion);
 msm2::writeLog("marker_keys=1,2,3,4,5,6 modifier=NONE top_row_digits=true edge_triggered=true");
 bool targetConfirmed=buildIdentity();auto config=settings();bool ready=msm2::initialize(targetConfirmed,config.audit,config.hooks.enabled);
 msm2::writer::configure(config.planar,targetConfirmed&&ready);
 msm2::hooks::start(targetConfirmed,config.hooks);
 auto image=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));IMAGE_DOS_HEADER traceDos{};IMAGE_NT_HEADERS64 traceNt{};bool traceImage=msm2::readMemory(image,&traceDos,sizeof(traceDos))&&traceDos.e_magic==IMAGE_DOS_SIGNATURE&&traceDos.e_lfanew>0&&traceDos.e_lfanew<0x100000&&msm2::readMemory(image+traceDos.e_lfanew,&traceNt,sizeof(traceNt))&&traceNt.Signature==IMAGE_NT_SIGNATURE;
 msm2::trace::configure(config.trace,targetConfirmed&&ready&&traceImage&&msm2::hooks::observationPlanAuthenticated(),image,traceImage?traceNt.OptionalHeader.SizeOfImage:0);
 msm2::input::configure(config.input,targetConfirmed&&ready&&traceImage&&msm2::hooks::observationPlanAuthenticated(),image,traceImage?traceNt.OptionalHeader.SizeOfImage:0);
 msm2::reset_copy::configure(targetConfirmed&&ready&&msm2::input::authenticated()&&msm2::hooks::observationPlanAuthenticated(),image,traceImage?traceNt.OptionalHeader.SizeOfImage:0);
 msm2::native_source::configure(config.source,targetConfirmed&&ready&&msm2::input::authenticated()&&msm2::hooks::observationPlanAuthenticated(),image,traceImage?traceNt.OptionalHeader.SizeOfImage:0,msm2::hooks::patchSnapshot());
 auto proofStatus=msm2::reset_copy::proofStatus();
 msm2::writeLog("ORIENTATION_ADMISSION exe_build_verified=%u path_a_writer_verified=%u path_b_writer_verified=%u path_b_caller_verified=%u path_b_map_verified=%u dual_source_admitted=%u",unsigned(targetConfirmed),unsigned(msm2::input::authenticated()),unsigned(proofStatus.writer),unsigned(proofStatus.caller),unsigned(proofStatus.map),unsigned(msm2::native_source::observationGuardsAuthenticated()));
 msm2::reset_trace::configure(targetConfirmed&&ready&&msm2::native_source::observationGuardsAuthenticated(),image,traceImage?traceNt.OptionalHeader.SizeOfImage:0,true,true);
 msm2::writeLog("surface_axis_verified=1 surface_sign_verified=1 plane_reference_verified=1 diagonal_surface_verified=1 source=prior_read_only_v01a9_A_E_validation scope=exact_build_planar_vertical NOT_formal_engine_field_names legacy_crawl_calibration_disabled=1");
 msm2::writeLog("surface_probe requested=%u effective=0 reason=unverified_query_ABI_no_game_call",unsigned(config.surface));
 msm2::writeLog("capabilities position=template rotation=row_axes_validated_with_sanity_check velocity=position_difference automatic_wallrun=complete_hook_set_guarded contact=unavailable");
 msm2::writeLog("WallCrawl=Unknown Ground=Unknown Air=Unknown; WallRun uses verified lifecycle only, user markers DO NOT change automatic state");
 msm2::writeLog("component_diagnostic=legacy_linear_enumeration_REMOVED reason=layout_unverified; reference_AOB_audit provides lookup candidates, no component pointers cached");
 bool debug=config.planar.debug,f8=false,f9=false,f10=false;ws::diag::MarkerKeys keys;ws::diag::History history;
 ULONGLONG lastSample=0,lastLog=0,lastOrientation=0;std::uint64_t id=0;
 msm2::Snapshot recent{};ws::diag::Motion recentMotion{};bool recentAvailable=false;
 // Frequent key polling; game memory reads remain interval-bound or explicit snapshots.
 while(WaitForSingleObject(stopEvent,10)==WAIT_TIMEOUT){
  if(pressed(VK_F8,f8)){msm2::reset_trace::kill();msm2::trace::kill();msm2::input::kill();msm2::native_source::kill();msm2::writer::kill("kill_switch_F8");msm2::writeLog("F8 ORIENTATION_TRACE_STOP=1 ALL_WRITES_DISABLED=1 latched_until_restart=1");}
  if(!msm2::hooks::observationPlanAuthenticated()){msm2::reset_trace::kill();msm2::trace::kill();msm2::input::kill();msm2::native_source::kill();}
  msm2::trace::tick(msm2::hooks::context(GetTickCount64()),msm2::hooks::nativeThreadId(),msm2::hooks::exitReason());
  msm2::trace::drainLogs();
  msm2::input::tick(msm2::hooks::context(GetTickCount64()),msm2::hooks::exitReason(),msm2::hooks::observationPlanAuthenticated());
  msm2::input::drainLogs();
  msm2::native_source::tick(msm2::hooks::context(GetTickCount64()),msm2::hooks::exitReason(),msm2::hooks::observationPlanAuthenticated()&&msm2::input::authenticated());
  msm2::native_source::drainLogs();
  msm2::writer::drainLogs();
  if(pressed(VK_F9,f9)){msm2::writeLog("F9 telemetry stop; all WallSprint writes stopping");break;}
  bool dump=pressed(VK_F10,f10);unsigned markers=markerEvents(keys);auto now=GetTickCount64();
  if(!markers&&!dump&&now-lastSample<100&&(!config.orientation||now-lastOrientation<config.interval)){msm2::hooks::drain(recentAvailable?&recent:nullptr,recentMotion,lastSample);continue;}
  lastSample=now;++id;msm2::Snapshot sample{};bool available=ready&&msm2::sample(sample);
  auto motion=history.update(available,sample.hero,sample.transform,{sample.position[0],sample.position[1],sample.position[2]},now);
  recent=sample;recentMotion=motion;recentAvailable=available;
  msm2::hooks::drain(available?&sample:nullptr,motion,now);
  if(motion.changed){
   msm2::writeLog("Hero/Transform identity changed available=%u hero=0x%llx transform=0x%llx hero_epoch=%llu; cache_reset=(transform,components,wallrun_observation,surface_query,velocity) wallrun_reset=Hero_or_Transform_changed",unsigned(available),static_cast<unsigned long long>(available?sample.hero:0),static_cast<unsigned long long>(available?sample.transform:0),static_cast<unsigned long long>(motion.epoch));
  }
  if(debug&&(now-lastLog>=500||motion.changed)){
   if(available){
    auto axes=ws::diag::actorAxes(sample.prefix);
    msm2::writeLog("Actor axes valid=%u Right=(%.6f,%.6f,%.6f) Up=(%.6f,%.6f,%.6f) Forward=(%.6f,%.6f,%.6f) worldUp=(0,1,0)",unsigned(axes.valid),axes.right.x,axes.right.y,axes.right.z,axes.up.x,axes.up.y,axes.up.z,axes.forward.x,axes.forward.y,axes.forward.z);
    msm2::writeLog("non_wallrun_state=Unknown WallSprint=%s sample_id=%llu hero=0x%llx transform=0x%llx position=(%.5f,%.5f,%.5f) yaw_scalar_template=%.6f",msm2::native_source::active()?"native_source_active":"inactive",static_cast<unsigned long long>(id),static_cast<unsigned long long>(sample.hero),static_cast<unsigned long long>(sample.transform),sample.position[0],sample.position[1],sample.position[2],sample.yaw);
    if(motion.valid)msm2::writeLog("velocity_estimate=(%.5f,%.5f,%.5f) source=position_difference NOT_physics_velocity",motion.velocity.x,motion.velocity.y,motion.velocity.z);else msm2::writeLog("velocity_estimate=unavailable_or_discontinuous NOT_physics_velocity");
   }else msm2::writeLog("Hero unavailable; resolver=%s; state=Unknown; no stale pointer retained",ready?"resolved":"disabled");
   msm2::hooks::logState(now);lastLog=now;
  }
  for(unsigned i=0;i<6;++i)if(markers&(1u<<i))msm2::dumpDiagnostic(available?&sample:nullptr,motion,now,id,ws::diag::markers[i]);
  if(dump)msm2::dumpDiagnostic(available?&sample:nullptr,motion,now,id,"F10_DUMP");
  if(config.orientation&&now-lastOrientation>=config.interval){
   // Reset the schedule even while F8 logging is paused; no busy polling.
   if(debug&&!markers&&!dump)msm2::dumpDiagnostic(available?&sample:nullptr,motion,now,id,"ORIENTATION_INTERVAL");
   lastOrientation=now;
  }
 }
 msm2::reset_trace::stop();msm2::native_source::stop();msm2::reset_copy::stop();msm2::input::stop();msm2::trace::stop();
 msm2::hooks::stop();
 msm2::writeLog("WallSprint exited reason=TelemetryStopped future_writes_disabled=1 no_forced_ground_restore=1");
 closeLog();
}
DWORD WINAPI worker(void*){
 try{run();}catch(const std::exception& e){msm2::reset_trace::stop();msm2::native_source::stop();msm2::reset_copy::stop();msm2::input::stop();msm2::trace::stop();msm2::hooks::stop();msm2::writeLog("exception=%s; telemetry stopped; game overrides=none",e.what());closeLog();}catch(...){msm2::reset_trace::stop();msm2::native_source::stop();msm2::reset_copy::stop();msm2::input::stop();msm2::trace::stop();msm2::hooks::stop();msm2::writeLog("exception=unknown; telemetry stopped; game overrides=none");closeLog();}
 auto self=instance;FreeLibraryAndExitThread(self,0);
}
}
namespace msm2 {
void writeLog(const char* format,...){
 char body[4096]{};va_list args;va_start(args,format);vsnprintf(body,sizeof(body),format,args);va_end(args);
 SYSTEMTIME time{};GetSystemTime(&time);char line[4352]{};
 int n=snprintf(line,sizeof(line),"%04u-%02u-%02uT%02u:%02u:%02u.%03uZ %s\r\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,time.wMilliseconds,body);
 if(n<=0)return;
 DWORD bytes=static_cast<DWORD>(n<static_cast<int>(sizeof(line))?n:sizeof(line)-1);
 AcquireSRWLockExclusive(&logLock);if(logFile!=INVALID_HANDLE_VALUE){DWORD written;WriteFile(logFile,line,bytes,&written,nullptr);}OutputDebugStringA(line);ReleaseSRWLockExclusive(&logLock);
}
}
extern "C" __declspec(dllexport) void script_enable(){
 AcquireSRWLockExclusive(&lifecycle);
 if(workerHandle&&WaitForSingleObject(workerHandle,0)==WAIT_TIMEOUT){ReleaseSRWLockExclusive(&lifecycle);return;}
 if(workerHandle){CloseHandle(workerHandle);workerHandle=nullptr;}if(stopEvent){CloseHandle(stopEvent);stopEvent=nullptr;}
 HMODULE self=nullptr;
 if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,reinterpret_cast<LPCWSTR>(&script_enable),&self)){
  stopEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);
  if(stopEvent)workerHandle=CreateThread(nullptr,0,worker,nullptr,0,&workerId);
  if(!workerHandle){if(stopEvent){CloseHandle(stopEvent);stopEvent=nullptr;}FreeLibrary(self);}
 }
 ReleaseSRWLockExclusive(&lifecycle);
}
extern "C" __declspec(dllexport) void script_disable(){
 msm2::reset_trace::kill();msm2::trace::kill();msm2::input::kill();msm2::native_source::kill();
 msm2::writer::kill("script_disable");
 AcquireSRWLockExclusive(&lifecycle);if(stopEvent)SetEvent(stopEvent);
 if(workerHandle&&GetCurrentThreadId()!=workerId){WaitForSingleObject(workerHandle,INFINITE);CloseHandle(workerHandle);workerHandle=nullptr;if(stopEvent){CloseHandle(stopEvent);stopEvent=nullptr;}}
 ReleaseSRWLockExclusive(&lifecycle);
}
BOOL WINAPI DllMain(HMODULE module,DWORD reason,LPVOID){
 if(reason==DLL_PROCESS_ATTACH)instance=module;
 // No waiting, file I/O, SDK calls, or hook work under the loader lock.
 return TRUE;
}
