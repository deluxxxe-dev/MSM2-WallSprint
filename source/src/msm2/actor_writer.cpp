// Direct Actor writes remain disabled. The separate b5 module may change only
// a verified caller-local source; it does not use this legacy Actor adapter.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xinput.h>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "msm2/actor_writer.hpp"
#include "msm2/drive_command.hpp"
#include "msm2/update_bridge.hpp"
#include "msm2/sdk_bridge.hpp"

extern "C" {
// M5b: set by camera_ref::configure; called on the game thread from the verified WallRun Update pre-original callback.
std::atomic<void(*)()> ws_wallsprint_update_listener{nullptr};
// Storage for camera_ref.cpp (kept here so smokes that include only this file link): manager `this` recorded by the
// camera thunk, and the proven camera handedness (0 = unknown).
std::uintptr_t ws_camref_this{}; // selected live camera manager (camera_ref worker); 0 = none
std::uintptr_t ws_camref_raw{},ws_camref_seq{},ws_camref_ring[8]{}; // written by the camera thunk
// M5c accepted values (owner run 2026-11-05: forward_dot 0.985, side_cross 0.991). Used from the start; the ground scan no longer runs.
std::atomic<int> ws_camera_handedness{1};
std::atomic<unsigned> ws_camera_forward_offset{0xd34}; // +0x1E0 rejected: it follows hero travel
std::atomic<int> ws_camera_forward_sign{1};
std::atomic<std::uint64_t> ws_camera_engage_ms{0}; // set by camera_ref when the WS camera reference engages
std::atomic<unsigned> ws_write_dispatch_enabled{0};
std::atomic<unsigned> ws_update_probe_enabled{0};
}

namespace {
void logInputStack();
constexpr std::uintptr_t kWallRun250Offset=0x250;
constexpr std::uintptr_t kConsumerThreshold0Rva=0x5303DE8;
constexpr std::uintptr_t kConsumerThreshold1Rva=0x5337098;
constexpr std::uint32_t kV8Threshold0Bits=0x3F000000u; // 0.5f
constexpr std::uint32_t kV8Threshold1Bits=0x3F35C28Fu; // 0.7099999785f

std::atomic<unsigned> thresholdProbeEnabled{0},finalSnapshotPending{0};
// M3c (2026-11-05, owner-authorized bounded write experiment): WallRun.this+0x240 and +0x24C hold the same unit
// travel-direction vector (M3b scan, OBSERVED-CORRELATION). When it is horizontal, tilt it 20deg toward world up in
// the existing pre-original Update callback to test whether WallRun travel follows a non-axis direction.
// Bounds: verified WallRun Update only, same gate as the c48 probe (F8/script disable stop it), per-process write cap,
// only an exact matching unit horizontal pair is ever replaced; the game recomputes its own value once writes stop.
constexpr bool kM3cHeadingTilt=false; // M3c accepted 2026-11-05 (travel followed the tilt); superseded by M3d
constexpr std::uintptr_t kHeadingOffsetA=0x240,kHeadingOffsetB=0x24c;
constexpr float kTiltSin=0.342020143f,kTiltCos=0.939692621f; // 20deg
constexpr std::uint64_t kTiltWriteCap=200000;
std::atomic<std::uint64_t> tiltWrites{0},tiltSkippedShape{0},tiltSkippedVertical{0},tiltRefusedWritable{0};
// M3d (2026-11-05): drive the same direction pair from the player's stick/WASD in wall-relative space.
// up = world up projected on the wall (normal at WallRun.this+0x398, |y|<=0.2), lateral sign calibrated once per WallRun
// owner from vanilla's own first horizontal choice for a clearly sideways stick. Neutral stick: no write (game keeps value).
constexpr bool kM3dStickDrive=true;
constexpr std::uintptr_t kWallNormalOffset=0x398;
constexpr float kStickDeadzone=0.35f;
std::atomic<std::uint64_t> driveWrites{0},driveNoInput{0},driveUncalibrated{0},driveBadNormal{0},driveCalibrations{0},driveCameraMapped{0},driveHeldForward{0},driveSnapSeq{0},driveUpAssist{0};
constexpr bool kCameraUpAssist=true,kCameraSideAxis=true;
constexpr bool kSmoothTurn=true;// M6e: ground sprint measured max ~470, large turns 270-360 deg/s
constexpr double kTurnRateDegPerSec=400.;std::atomic<std::uint64_t> followWrites{0},followManualSkips{0},followNoHero{0},aimWrites{0},holdWrites{0};double turnLastT{};std::atomic<std::uint64_t> turnLimited{0};
std::atomic<std::uint64_t> driveSideAxis{0};
std::atomic<float> driveSnap[16]{};std::uint64_t driveSnapLogged{};
// M5c-r6 diagnostics: vanilla reversal events (heading read back opposite to our previous write) and camera axis scan.
std::atomic<std::uint64_t> driveFlips{0};std::atomic<float> driveFlipSnap[17]{};std::uint64_t driveFlipsLogged{};
std::atomic<std::uint64_t> driveOverrides{0};std::atomic<float> driveOverrideSnap[17]{};std::uint64_t driveOverridesLogged{};
float driveLastOut[3]{};bool driveLastOutValid{};
constexpr unsigned kStateSpan=0x600;float statePrev[kStateSpan/4]{},stateCur[kStateSpan/4]{};bool statePrevValid{};std::uintptr_t stateOwner{};
// M5d-2 diagnostic: which WallRun slots carry the game's left/right run direction (sign follows the game's lateral target).
std::atomic<unsigned> slotSame[0x600/4]{},slotOpp[0x600/4]{},slotPrevSame[0x600/4]{};std::atomic<unsigned> slotEventsPos{0},slotEventsNeg{0};unsigned slotEventsLogged{};
std::atomic<unsigned> flipDiffPending{0};unsigned flipDiffsLogged{};char flipDiffText[1400]{};
unsigned camAxisScans{};std::uint64_t camAxisLastMs{};
using XInputGetStateFn=DWORD (WINAPI*)(DWORD,XINPUT_STATE*);
std::atomic<XInputGetStateFn> xinputGetState{nullptr};
std::uintptr_t driveOwner{};int driveLateralSign{};
bool driveLatched{};float driveLatchP[3]{}; // last valid wall-plane screen-forward (used only when the camera looks straight at the wall)
std::uint64_t driveOwnerWrites{},driveLastCallMs{}; // cap is per WallRun session (gap >500 ms), not per process
std::atomic<std::uint32_t> threshold0Bits{kV8Threshold0Bits},threshold1Bits{kV8Threshold1Bits};
std::atomic<std::uint64_t> samples{0},readOk{0},readFail{0},fieldNonNanSamples{0},fieldNanSamples{0};
std::atomic<std::uint64_t> consumer0True{0},consumer1Fallthrough{0};
std::atomic<std::uint32_t> firstBits{0},lastBits{0};
std::atomic<unsigned> firstCaptured{0};
std::uint64_t lastLoggedSamples{},lastThresholdCheck{};

float asFloat(std::uint32_t bits){float value{};std::memcpy(&value,&bits,sizeof(value));return value;}

void resetProbe(){
 ws_update_probe_enabled.store(0,std::memory_order_release);
 thresholdProbeEnabled.store(0,std::memory_order_release);finalSnapshotPending.store(0,std::memory_order_release);
 threshold0Bits.store(kV8Threshold0Bits,std::memory_order_relaxed);
 threshold1Bits.store(kV8Threshold1Bits,std::memory_order_relaxed);
 samples.store(0,std::memory_order_relaxed);readOk.store(0,std::memory_order_relaxed);readFail.store(0,std::memory_order_relaxed);
 fieldNonNanSamples.store(0,std::memory_order_relaxed);fieldNanSamples.store(0,std::memory_order_relaxed);
 consumer0True.store(0,std::memory_order_relaxed);consumer1Fallthrough.store(0,std::memory_order_relaxed);
 firstBits.store(0,std::memory_order_relaxed);lastBits.store(0,std::memory_order_relaxed);firstCaptured.store(0,std::memory_order_relaxed);
 lastLoggedSamples=lastThresholdCheck=0;
}

bool readLiveThresholds(std::uint32_t& a,std::uint32_t& b){
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
 if(!base||base>UINTPTR_MAX-kConsumerThreshold1Rva)return false;
 return msm2::readMemory(base+kConsumerThreshold0Rva,&a,sizeof(a))&&msm2::readMemory(base+kConsumerThreshold1Rva,&b,sizeof(b));
}

void logProbeSnapshot(const char* phase){
 auto n=samples.load(std::memory_order_acquire);
 auto ok=readOk.load(std::memory_order_acquire),bad=readFail.load(std::memory_order_acquire);
 auto nonNan=fieldNonNanSamples.load(std::memory_order_acquire),nan=fieldNanSamples.load(std::memory_order_acquire);
 auto aTrue=consumer0True.load(std::memory_order_acquire),bFallthrough=consumer1Fallthrough.load(std::memory_order_acquire);
 auto t0=threshold0Bits.load(std::memory_order_acquire),t1=threshold1Bits.load(std::memory_order_acquire);
 std::uint32_t live0=0,live1=0;bool liveOk=readLiveThresholds(live0,live1);
 bool liveUnchanged=liveOk&&live0==t0&&live1==t1;
 msm2::writeLog("wallrun_0x250_threshold_probe phase=%s enabled=%u update_probe_gate=%u write_dispatch_gate=%u exact_update_pre_original_samples=%llu read_ok=%llu read_fail=%llu field_non_nan=%llu field_nan=%llu consumer_gt_threshold0_or_unordered=%llu consumer_gt_threshold1_or_unordered=%llu threshold0_bits=0x%08x threshold0=%.9g threshold1_bits=0x%08x threshold1=%.9g live_check_ok=%u live_threshold0_bits=0x%08x live_threshold1_bits=0x%08x live_thresholds_unchanged_from_admission=%u v8_expected_threshold_bits_match=%u first_bits=0x%08x last_bits=0x%08x source=preserved_Update_thunk_pre_original_callback field=WallRun.this+0x250 pointer_dereference=0 game_call=0 game_write=0",
  phase,unsigned(thresholdProbeEnabled.load(std::memory_order_acquire)),unsigned(ws_update_probe_enabled.load(std::memory_order_acquire)),unsigned(ws_write_dispatch_enabled.load(std::memory_order_acquire)),static_cast<unsigned long long>(n),static_cast<unsigned long long>(ok),static_cast<unsigned long long>(bad),static_cast<unsigned long long>(nonNan),static_cast<unsigned long long>(nan),static_cast<unsigned long long>(aTrue),static_cast<unsigned long long>(bFallthrough),
  t0,asFloat(t0),t1,asFloat(t1),unsigned(liveOk),live0,live1,unsigned(liveUnchanged),unsigned(t0==kV8Threshold0Bits&&t1==kV8Threshold1Bits),firstBits.load(std::memory_order_acquire),lastBits.load(std::memory_order_acquire));
}
}

namespace msm2::writer {
void configure(ws::planar::Config c,bool exactBuild){
 ws_write_dispatch_enabled.store(0,std::memory_order_release);resetProbe();
 writeLog("writer_config requested_orientation=%u exact_build=%u Actor_writes=0 reason=direct_Actor_adapter_hard_disabled old_configuration_cannot_enable_writes=1",unsigned(c.orientation),unsigned(exactBuild));
}

void admit(bool completeVerified,const std::array<Patch,4>&){
 ws_write_dispatch_enabled.store(0,std::memory_order_release);
 ws_update_probe_enabled.store(0,std::memory_order_release);
 thresholdProbeEnabled.store(0,std::memory_order_release);
 std::uint32_t a=0,b=0;bool thresholdsReadable=completeVerified&&readLiveThresholds(a,b);
 // Load XInput only inside the real game process; test hosts (Wine smokes in Update-WS) must not load input stacks.
 if(kM3dStickDrive&&!xinputGetState.load()&&GetModuleHandleW(L"Spider-Man2.exe")){
  logInputStack();
  HMODULE x=LoadLibraryW(L"xinput1_4.dll");if(!x)x=LoadLibraryW(L"xinput9_1_0.dll");
  if(x)xinputGetState.store(reinterpret_cast<XInputGetStateFn>(reinterpret_cast<void*>(GetProcAddress(x,"XInputGetState"))));
  writeLog("wallrun_m3d_stick_drive_admission xinput=%u keyboard=WASD offsets=0x240,0x24c normal=0x398 deadzone=%.2f",unsigned(xinputGetState.load()!=nullptr),kStickDeadzone);
 }
 if(thresholdsReadable){
  threshold0Bits.store(a,std::memory_order_relaxed);threshold1Bits.store(b,std::memory_order_relaxed);
  thresholdProbeEnabled.store(1,std::memory_order_release);
  ws_update_probe_enabled.store(1,std::memory_order_release);
 }
 writeLog("direct_actor_admission=DISABLED reason=legacy_direct_Actor_adapter_unavailable orientation_pulse=0");
 writeLog("wallrun_0x250_threshold_probe_admission complete_verified=%u admitted=%u update_probe_gate=%u write_dispatch_gate=%u live_thresholds_readable=%u threshold0_bits=0x%08x threshold0=%.9g threshold1_bits=0x%08x threshold1=%.9g threshold0_nan=%u threshold1_nan=%u v8_expected_threshold_bits_match=%u exact_update_pre_original=1 direct_update_thunk_callback=1 bridge_unwind_or_argument_clone=0 field_read_width=4 pointer_dereference=0 game_call=0 game_write=0",
  unsigned(completeVerified),unsigned(thresholdsReadable),unsigned(ws_update_probe_enabled.load(std::memory_order_acquire)),unsigned(ws_write_dispatch_enabled.load(std::memory_order_acquire)),unsigned(thresholdsReadable),a,asFloat(a),b,asFloat(b),unsigned(std::isnan(asFloat(a))),unsigned(std::isnan(asFloat(b))),unsigned(thresholdsReadable&&a==kV8Threshold0Bits&&b==kV8Threshold1Bits));
}

void kill(const char*) noexcept{
 ws_write_dispatch_enabled.store(0,std::memory_order_release);
 ws_update_probe_enabled.store(0,std::memory_order_release);
 if(thresholdProbeEnabled.exchange(0,std::memory_order_acq_rel))finalSnapshotPending.store(1,std::memory_order_release);
}

void drainLogs(){
 auto n=samples.load(std::memory_order_acquire);bool final=finalSnapshotPending.exchange(0,std::memory_order_acq_rel)!=0;
 if(!n)return;
 if(!final&&n==lastLoggedSamples)return;
 auto now=GetTickCount64();if(!final&&lastLoggedSamples&&now-lastThresholdCheck<500)return;
 lastThresholdCheck=now;lastLoggedSamples=n;logProbeSnapshot(final?"final":"cumulative");
 writeLog("wallrun_m3d_stick_drive phase=%s enabled=%u writes=%llu cap=%llu no_input=%llu uncalibrated=%llu calibrations=%llu bad_normal=%llu lateral_sign=%d camera_mapped=%llu handedness=%d game_write=1",final?"final":"cumulative",unsigned(kM3dStickDrive),static_cast<unsigned long long>(driveWrites.load()),static_cast<unsigned long long>(kTiltWriteCap),static_cast<unsigned long long>(driveNoInput.load()),static_cast<unsigned long long>(driveUncalibrated.load()),static_cast<unsigned long long>(driveCalibrations.load()),static_cast<unsigned long long>(driveBadNormal.load()),driveLateralSign,static_cast<unsigned long long>(driveCameraMapped.load()),ws_camera_handedness.load());
 if(auto q=driveSnapSeq.load(std::memory_order_acquire);q!=driveSnapLogged){driveSnapLogged=q;float v[16];for(int i=0;i<16;++i){v[i]=driveSnap[i].load(std::memory_order_relaxed);}
  writeLog("wallrun_m5c_map cam=%.3f,%.3f,%.3f normal=%.3f,%.3f,%.3f stick=%.2f,%.2f out=%.3f,%.3f,%.3f heading=%.3f,%.3f,%.3f proj=%.3f mode=%d held=%llu up_assist=%llu side_axis=%llu turn_limited=%llu turn_rate=%.0f cam_follow_writes=%llu cam_follow_manual_skips=%llu cam_follow_no_hero=%llu cam_aim_writes=%llu cam_yaw_hold_writes=%llu",v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],int(v[15]),static_cast<unsigned long long>(driveHeldForward.load()),static_cast<unsigned long long>(driveUpAssist.load()),static_cast<unsigned long long>(driveSideAxis.load()),static_cast<unsigned long long>(turnLimited.load()),kTurnRateDegPerSec,static_cast<unsigned long long>(followWrites.load()),static_cast<unsigned long long>(followManualSkips.load()),static_cast<unsigned long long>(followNoHero.load()),static_cast<unsigned long long>(aimWrites.load()),static_cast<unsigned long long>(holdWrites.load()));}
 if(auto q=driveFlips.load(std::memory_order_acquire);q!=driveFlipsLogged){driveFlipsLogged=q;float v[17];for(int i=0;i<17;++i){v[i]=driveFlipSnap[i].load(std::memory_order_relaxed);}
  writeLog("wallrun_m5c_vanilla_reversal count=%llu stick=%.2f,%.2f our_last=%.3f,%.3f,%.3f vanilla=%.3f,%.3f,%.3f normal=%.3f,%.3f,%.3f cam_f=%.3f,%.3f,%.3f cam_u=%.3f,%.3f,%.3f",static_cast<unsigned long long>(q),v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16]);}
 if(auto q=driveOverrides.load(std::memory_order_acquire);q!=driveOverridesLogged){driveOverridesLogged=q;float v[17];for(int i=0;i<17;++i){v[i]=driveOverrideSnap[i].load(std::memory_order_relaxed);}
  writeLog("wallrun_m5d_vanilla_override count=%llu stick=%.2f,%.2f our_last=%.3f,%.3f,%.3f vanilla=%.3f,%.3f,%.3f normal=%.3f,%.3f,%.3f cam_f=%.3f,%.3f,%.3f cam_u=%.3f,%.3f,%.3f",static_cast<unsigned long long>(q),v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15],v[16]);}
 // Camera axis scan (read-only): unit vectors orthogonal to the camera forward near +0xD34, at most 6 scans, >=3 s apart, only while mapping is live.
 if(auto cam=ws_camref_this;cam&&cam<=UINTPTR_MAX-0x1000&&driveSnap[15].load(std::memory_order_relaxed)>0.5f&&camAxisScans<6&&GetTickCount64()-camAxisLastMs>3000){
  float blk[0x400/4];float f[3];
  if(msm2::readMemory(cam+0xC00,blk,sizeof(blk))){std::memcpy(f,&blk[(0xD34-0xC00)/4],sizeof(f));
   float fl=std::sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
   if(std::isfinite(fl)&&fl>0.95f&&fl<1.05f){camAxisLastMs=GetTickCount64();++camAxisScans;
    float nx=driveSnap[3].load(),ny=driveSnap[4].load(),nz=driveSnap[5].load();
    char line[1600];int len=std::snprintf(line,sizeof(line),"wallrun_m5c_camera_axes scan=%u cam=%.3f,%.3f,%.3f normal=%.3f,%.3f,%.3f ortho_units=",camAxisScans,f[0],f[1],f[2],nx,ny,nz);
    int found=0;
    for(unsigned o=0;o+12<=sizeof(blk)&&found<16;o+=4){float* v=&blk[o/4];
     if(!std::isfinite(v[0])||!std::isfinite(v[1])||!std::isfinite(v[2]))continue;
     float l=v[0]*v[0]+v[1]*v[1]+v[2]*v[2];if(l<0.96f||l>1.04f)continue;
     float df=(v[0]*f[0]+v[1]*f[1]+v[2]*f[2])/fl;if(std::fabs(df)>0.05f)continue;
     float dn=v[0]*nx+v[1]*ny+v[2]*nz;
     if(len>0&&len<int(sizeof(line))-80)len+=std::snprintf(line+len,sizeof(line)-len,"%s0x%x:%.3f,%.3f,%.3f/n%.2f",found?";":"",0xC00+o,v[0],v[1],v[2],dn);
     ++found;}
    writeLog("%s count=%d",line,found);}}}
 if(auto ev=slotEventsPos.load()+slotEventsNeg.load();ev!=slotEventsLogged){slotEventsLogged=ev;char t[1500];int len=std::snprintf(t,sizeof(t),"events_pos=%u events_neg=%u slots=",slotEventsPos.load(),slotEventsNeg.load());int k=0;
  for(unsigned i=0;i<0x600/4&&k<40;++i){unsigned a=slotSame[i].load(),b=slotOpp[i].load(),c=slotPrevSame[i].load();if(a+b<3)continue;
   if(len<int(sizeof(t))-40){len+=std::snprintf(t+len,sizeof(t)-len,"%s0x%x:s%u/o%u/p%u",k?";":"",i*4,a,b,c);}++k;}
  writeLog("wallrun_m5d_direction_slots %s",k?t:"none");}
 if(flipDiffPending.load(std::memory_order_acquire)){writeLog("wallrun_m5c_reversal_state %s",flipDiffText);flipDiffPending.store(0,std::memory_order_release);}
 writeLog("wallrun_m3c_heading_tilt phase=%s enabled=%u offsets=0x240,0x24c tilt_deg=20 writes=%llu cap=%llu skipped_shape=%llu skipped_not_horizontal=%llu refused_not_writable=%llu game_write=1 restore=game_recomputes_after_stop",final?"final":"cumulative",unsigned(kM3cHeadingTilt),static_cast<unsigned long long>(tiltWrites.load()),static_cast<unsigned long long>(kTiltWriteCap),static_cast<unsigned long long>(tiltSkippedShape.load()),static_cast<unsigned long long>(tiltSkippedVertical.load()),static_cast<unsigned long long>(tiltRefusedWritable.load()));
}

bool active(){return false;}
}

namespace {
bool writableRange(std::uintptr_t p,std::size_t n){
 MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(p),&m,sizeof(m)))return false;
 auto base=reinterpret_cast<std::uintptr_t>(m.BaseAddress);
 return m.State==MEM_COMMIT&&!(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&((m.Protect&0xff)==PAGE_READWRITE)&&p>=base&&p-base+n<=m.RegionSize;
}
void tiltHeading(std::uintptr_t owner){
 if(!kM3cHeadingTilt||tiltWrites.load(std::memory_order_relaxed)>=kTiltWriteCap||owner>UINTPTR_MAX-0x258)return;
 float h[6]{};if(!msm2::readMemory(owner+kHeadingOffsetA,h,sizeof(h))){tiltSkippedShape.fetch_add(1,std::memory_order_relaxed);return;}
 for(float v:h)if(!std::isfinite(v)){tiltSkippedShape.fetch_add(1,std::memory_order_relaxed);return;}
 float n=std::sqrt(h[0]*h[0]+h[1]*h[1]+h[2]*h[2]);
 float same=h[0]*h[3]+h[1]*h[4]+h[2]*h[5];
 if(n<0.98f||n>1.02f||same<0.999f){tiltSkippedShape.fetch_add(1,std::memory_order_relaxed);return;}
 if(std::fabs(h[1])>0.1f){tiltSkippedVertical.fetch_add(1,std::memory_order_relaxed);return;}
 float hx=h[0],hz=h[2],hn=std::sqrt(hx*hx+hz*hz);if(hn<0.9f){tiltSkippedShape.fetch_add(1,std::memory_order_relaxed);return;}
 float v[3]{hx/hn*kTiltCos,kTiltSin,hz/hn*kTiltCos};
 if(!writableRange(owner+kHeadingOffsetA,24)){tiltRefusedWritable.fetch_add(1,std::memory_order_relaxed);return;}
 std::memcpy(reinterpret_cast<void*>(owner+kHeadingOffsetA),v,sizeof(v));
 std::memcpy(reinterpret_cast<void*>(owner+kHeadingOffsetB),v,sizeof(v));
 tiltWrites.fetch_add(1,std::memory_order_relaxed);
}

// M5d read-only diagnostic: which input libraries the game itself uses (before WS loads XInput).
void logInputStack(){
 const wchar_t* mods[]={L"xinput1_4.dll",L"xinput1_3.dll",L"xinput9_1_0.dll",L"xinputuap.dll",L"GameInput.dll",L"dinput8.dll",L"hid.dll",L"SDL2.dll",L"SDL3.dll",L"libScePad.dll",L"Windows.Gaming.Input.dll",L"setupapi.dll"};
 char loaded[512]{};int len=0;
 for(auto m:mods){if(GetModuleHandleW(m)){len+=std::snprintf(loaded+len,sizeof(loaded)-len,"%s%ls",len?",":"",m);if(len>=int(sizeof(loaded))-40)break;}}
 char imports[1400]{};int il=0;
 auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
 auto addName=[&](std::uintptr_t rva,const char* tag){if(!rva)return;const char* name=reinterpret_cast<const char*>(base+rva);char low[64]{};
  for(int i=0;i<63&&name[i];++i){char c=name[i];low[i]=char(c>='A'&&c<='Z'?c+32:c);}
  if(std::strstr(low,"xinput")||std::strstr(low,"gameinput")||std::strstr(low,"dinput")||std::strstr(low,"hid")||std::strstr(low,"sdl")||std::strstr(low,"pad")||std::strstr(low,"gaming")||std::strstr(low,"input"))
   if(il<int(sizeof(imports))-80)il+=std::snprintf(imports+il,sizeof(imports)-il,"%s%s:%s",il?",":"",tag,name);};
 if(base){auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
  if(dos->e_magic==IMAGE_DOS_SIGNATURE){auto nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
   if(nt->Signature==IMAGE_NT_SIGNATURE){
    auto& id=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if(id.VirtualAddress&&id.Size){for(auto d=reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base+id.VirtualAddress);d->Name;++d)addName(d->Name,"import");}
    auto& dd=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];
    if(dd.VirtualAddress&&dd.Size){for(auto d=reinterpret_cast<const IMAGE_DELAYLOAD_DESCRIPTOR*>(base+dd.VirtualAddress);d->DllNameRVA;++d)addName(d->DllNameRVA,"delay");}}}}
 msm2::writeLog("wallrun_m5d_input_stack loaded=%s exe_input_imports=%s read_only=1",len?loaded:"none",il?imports:"none");
}
bool readRightStick(float& rx,float& ry){
 rx=ry=0;if(auto get=xinputGetState.load()){XINPUT_STATE st{};if(get(0,&st)==ERROR_SUCCESS){rx=st.Gamepad.sThumbRX/32767.f;ry=st.Gamepad.sThumbRY/32767.f;return true;}}
 return false;
}
bool readStick(float& sx,float& sy){
 sx=sy=0;
 if(auto get=xinputGetState.load()){XINPUT_STATE st{};if(get(0,&st)==ERROR_SUCCESS){
  float x=st.Gamepad.sThumbLX/32767.f,y=st.Gamepad.sThumbLY/32767.f;
  if(x*x+y*y>=kStickDeadzone*kStickDeadzone){sx=std::fmax(-1.f,std::fmin(1.f,x));sy=std::fmax(-1.f,std::fmin(1.f,y));return true;}}}
 float kx=float((GetAsyncKeyState('D')<0)-(GetAsyncKeyState('A')<0)),ky=float((GetAsyncKeyState('W')<0)-(GetAsyncKeyState('S')<0));
 if(kx!=0||ky!=0){sx=kx;sy=ky;return true;}
 return false;
}

// M6g experiment: WS-only camera yaw follow. The ground camera turns behind the travel direction; on the wall it does
// not (M6e: travel vs view 60-130 deg). Rotate the selected camera manager's rows (+0xD24 right, +0xD34 up, +0xD44
// forward) and position (+0xD54) about the wall normal through the hero toward the commanded travel, <=90 deg/s,
// 10 deg deadband. Never while the right stick moves the camera or for 1 s after. If the game recomputes these
// rows from its own state the write has no effect (observable in WS_CAMERA_FRAMING travel_vs_view).
constexpr bool kCameraFollow=true,kCameraAim=true;constexpr float kCameraAimPivot=1.55f;constexpr double kCameraAimDeadbandDeg=5.;constexpr double kCameraFollowDegPerSec=90.,kCameraFollowDeadbandDeg=10.;
double followLastT{};std::uint64_t followManualMs{};
void rotN(const float* n,float* v,float ca,float sa){float nv=n[0]*v[0]+n[1]*v[1]+n[2]*v[2];float c[3]{n[1]*v[2]-n[2]*v[1],n[2]*v[0]-n[0]*v[2],n[0]*v[1]-n[1]*v[0]};
 for(int i=0;i<3;++i){v[i]=v[i]*ca+c[i]*sa+n[i]*nv*(1-ca);}}

// M6q: auto-centering OFF = yaw hold. The actor-relative WS camera turns its reference with the hero's facing, which
// reads as automatic centering. Each WallRun update, undo the camera's yaw change about the wall normal (rows +0xD24/
// +0xD34/+0xD44 and position +0xD54 rotated about n through the hero), unless the right stick turns the camera.
// Pitch, roll onto the wall and position follow stay with the game.
float holdPrev[3]{};bool holdValid{};std::uint64_t holdLastMs{};
void cameraYawHold(const float* n){
 auto now=GetTickCount64();if(now-holdLastMs>150)holdValid=false;holdLastMs=now;
 auto m=ws_camref_this;if(!m||m>UINTPTR_MAX-0x1000){holdValid=false;return;}
 auto hm=ws::drive::heroMs.load(std::memory_order_acquire);if(!hm||now<hm||now-hm>200){holdValid=false;return;}
 float R[16];if(!msm2::readMemory(m+0xD24,R,sizeof(R))){holdValid=false;return;}for(float v:R)if(!std::isfinite(v)){holdValid=false;return;}
 float* f=R+8;float fn=f[0]*n[0]+f[1]*n[1]+f[2]*n[2];float fp[3]{f[0]-fn*n[0],f[1]-fn*n[1],f[2]-fn*n[2]};
 float fpl=std::sqrt(fp[0]*fp[0]+fp[1]*fp[1]+fp[2]*fp[2]);if(fpl<0.3f){holdValid=false;return;}for(float& c:fp)c/=fpl;
 float rx,ry;bool manual=readRightStick(rx,ry)&&rx*rx+ry*ry>0.25f*0.25f;
 if(!holdValid||manual){std::memcpy(holdPrev,fp,sizeof(fp));holdValid=true;return;}
 float pn=holdPrev[0]*n[0]+holdPrev[1]*n[1]+holdPrev[2]*n[2];float pp[3]{holdPrev[0]-pn*n[0],holdPrev[1]-pn*n[1],holdPrev[2]-pn*n[2]};
 float ppl=std::sqrt(pp[0]*pp[0]+pp[1]*pp[1]+pp[2]*pp[2]);if(ppl<0.3f){std::memcpy(holdPrev,fp,sizeof(fp));return;}for(float& c:pp)c/=ppl;
 float x[3]{fp[1]*pp[2]-fp[2]*pp[1],fp[2]*pp[0]-fp[0]*pp[2],fp[0]*pp[1]-fp[1]*pp[0]};
 float a=std::atan2(x[0]*n[0]+x[1]*n[1]+x[2]*n[2],fp[0]*pp[0]+fp[1]*pp[1]+fp[2]*pp[2]); // rotation that takes current back to previous
 if(std::fabs(a)<0.0009f){return;}
 if(std::fabs(a)>0.6f){std::memcpy(holdPrev,fp,sizeof(fp));return;} // large jump (cut/teleport): adopt
 float hp[3]{ws::drive::heroPos[0].load(std::memory_order_relaxed),ws::drive::heroPos[1].load(std::memory_order_relaxed),ws::drive::heroPos[2].load(std::memory_order_relaxed)};
 float rel[3]{R[12]-hp[0],R[13]-hp[1],R[14]-hp[2]};float rl=std::sqrt(rel[0]*rel[0]+rel[1]*rel[1]+rel[2]*rel[2]);if(!std::isfinite(rl)||rl>30.f)return;
 float ca=std::cos(a),sa=std::sin(a);rotN(n,R,ca,sa);rotN(n,R+4,ca,sa);rotN(n,R+8,ca,sa);rotN(n,rel,ca,sa);for(int i=0;i<3;++i){R[12+i]=hp[i]+rel[i];}
 if(!writableRange(m+0xD24,sizeof(R)))return;
 for(int row=0;row<4;++row){std::memcpy(reinterpret_cast<void*>(m+0xD24+0x10*row),R+4*row,12);}
 holdWrites.fetch_add(1,std::memory_order_relaxed);
}
void cameraFollow(const float* n,const float* d){
 LARGE_INTEGER q{},fr{};QueryPerformanceCounter(&q);QueryPerformanceFrequency(&fr);double t=double(q.QuadPart)/double(fr.QuadPart),dt=followLastT>0?t-followLastT:0.;followLastT=t;
 if(!ws::drive::cameraAutoCenter.load(std::memory_order_relaxed)){cameraYawHold(n);return;}
 if(!kCameraFollow||!(dt>0.)||dt>0.1)return;
 auto now=GetTickCount64();float rx,ry;if(readRightStick(rx,ry)&&rx*rx+ry*ry>0.25f*0.25f)followManualMs=now;
 if(followManualMs&&now-followManualMs<1000){followManualSkips.fetch_add(1,std::memory_order_relaxed);return;}
 auto m=ws_camref_this;if(!m||m>UINTPTR_MAX-0x1000)return;
 auto hm=ws::drive::heroMs.load(std::memory_order_acquire);if(!hm||now<hm||now-hm>200){followNoHero.fetch_add(1,std::memory_order_relaxed);return;}
 float R[16];if(!msm2::readMemory(m+0xD24,R,sizeof(R)))return;for(float v:R)if(!std::isfinite(v))return;
 float* f=R+8;float fl=std::sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);if(fl<0.9f||fl>1.1f)return;
 float fn=f[0]*n[0]+f[1]*n[1]+f[2]*n[2];float fp[3]{f[0]-fn*n[0],f[1]-fn*n[1],f[2]-fn*n[2]};float fpl=std::sqrt(fp[0]*fp[0]+fp[1]*fp[1]+fp[2]*fp[2]);if(fpl<0.3f)return;
 float x[3]{fp[1]*d[2]-fp[2]*d[1],fp[2]*d[0]-fp[0]*d[2],fp[0]*d[1]-fp[1]*d[0]};
 float err=std::atan2(x[0]*n[0]+x[1]*n[1]+x[2]*n[2],fp[0]*d[0]+fp[1]*d[1]+fp[2]*d[2]);
 if(std::fabs(err)<float(kCameraFollowDeadbandDeg*3.14159265358979/180.))err=0;
 float mx=float(kCameraFollowDegPerSec*3.14159265358979/180.*dt),a=err>0?std::fmin(err,mx):std::fmax(err,-mx),ca=std::cos(a),sa=std::sin(a);
 float hp[3]{ws::drive::heroPos[0].load(std::memory_order_relaxed),ws::drive::heroPos[1].load(std::memory_order_relaxed),ws::drive::heroPos[2].load(std::memory_order_relaxed)};
 float rel[3]{R[12]-hp[0],R[13]-hp[1],R[14]-hp[2]};float rl=std::sqrt(rel[0]*rel[0]+rel[1]*rel[1]+rel[2]*rel[2]);if(!std::isfinite(rl)||rl>30.f)return;
 bool changed=a!=0;
 if(changed){rotN(n,R,ca,sa);rotN(n,R+4,ca,sa);rotN(n,R+8,ca,sa);rotN(n,rel,ca,sa);for(int i=0;i<3;++i){R[12+i]=hp[i]+rel[i];}}
 // M6m: aim correction. Turn the view (rows only, camera position kept) toward the hero pivot point
 // (hero + wall normal * 1.55 m, the ground pivot height) at <= 90 deg/s with a 5 deg deadband. M6l measured the hero
 // 36-44 deg off center while running down the wall (up/sideways 14-19 deg).
 if(kCameraAim){float tg[3]{hp[0]+n[0]*kCameraAimPivot-R[12],hp[1]+n[1]*kCameraAimPivot-R[13],hp[2]+n[2]*kCameraAimPivot-R[14]};
  float tl=std::sqrt(tg[0]*tg[0]+tg[1]*tg[1]+tg[2]*tg[2]);
  if(tl>0.3f&&tl<30.f){for(float& c:tg)c/=tl;float* ff=R+8;float fl2=std::sqrt(ff[0]*ff[0]+ff[1]*ff[1]+ff[2]*ff[2]);
   float cd=(ff[0]*tg[0]+ff[1]*tg[1]+ff[2]*tg[2])/fl2;float ang=std::acos(std::fmax(-1.f,std::fmin(1.f,cd)));
   if(ang>float(kCameraAimDeadbandDeg*3.14159265358979/180.)){float ax[3]{ff[1]*tg[2]-ff[2]*tg[1],ff[2]*tg[0]-ff[0]*tg[2],ff[0]*tg[1]-ff[1]*tg[0]};
    float al=std::sqrt(ax[0]*ax[0]+ax[1]*ax[1]+ax[2]*ax[2]);
    if(al>1e-4f){for(float& c:ax)c/=al;float st=std::fmin(ang,mx),c2=std::cos(st),s2=std::sin(st);rotN(ax,R,c2,s2);rotN(ax,R+4,c2,s2);rotN(ax,R+8,c2,s2);changed=true;aimWrites.fetch_add(1,std::memory_order_relaxed);}}}}
 if(!changed)return;
 if(!writableRange(m+0xD24,sizeof(R)))return;
 for(int row=0;row<4;++row){std::memcpy(reinterpret_cast<void*>(m+0xD24+0x10*row),R+4*row,12);}
 followWrites.fetch_add(1,std::memory_order_relaxed);
}
void driveHeading(std::uintptr_t owner){
 if(!kM3dStickDrive||owner>UINTPTR_MAX-0x3a4)return;
 if(owner!=driveOwner){driveOwner=owner;driveLateralSign=0;driveOwnerWrites=0;driveLatched=false;}
 {auto now=GetTickCount64();if(now-driveLastCallMs>500)driveOwnerWrites=0;driveLastCallMs=now;}
 if(driveOwnerWrites>=kTiltWriteCap)return;
 float sx,sy;if(!readStick(sx,sy)){driveLatched=false;driveLastOutValid=false;statePrevValid=false;driveNoInput.fetch_add(1,std::memory_order_relaxed);return;}
 float h[6]{},n[3]{};
 if(!msm2::readMemory(owner+kHeadingOffsetA,h,sizeof(h))||!msm2::readMemory(owner+kWallNormalOffset,n,sizeof(n))){driveBadNormal.fetch_add(1,std::memory_order_relaxed);return;}
 for(float v:h)if(!std::isfinite(v))return;
 // M5c-r7 diagnostic: keep the previous frame's WallRun state so a reversal frame can be compared with the frame before.
 {bool okState=owner<=UINTPTR_MAX-kStateSpan&&msm2::readMemory(owner,stateCur,sizeof(stateCur));
  if(okState&&statePrevValid&&stateOwner==owner&&driveLastOutValid&&flipDiffsLogged<8&&!flipDiffPending.load(std::memory_order_acquire)
     &&h[0]*driveLastOut[0]+h[1]*driveLastOut[1]+h[2]*driveLastOut[2]<-0.5f){
   // list unit vec3 slots aligned with the vanilla reversal target now (cur) and one frame earlier (prev)
   int len=std::snprintf(flipDiffText,sizeof(flipDiffText),"target=%.3f,%.3f,%.3f",h[0],h[1],h[2]);
   for(int pass=0;pass<2;++pass){const float* b=pass?statePrev:stateCur;len+=std::snprintf(flipDiffText+len,sizeof(flipDiffText)-len,pass?" prev_frame=":" this_frame=");int k=0;
    for(unsigned o=0;o+12<=kStateSpan&&k<20;o+=4){const float* v=&b[o/4];if(!std::isfinite(v[0])||!std::isfinite(v[1])||!std::isfinite(v[2]))continue;
     float l=v[0]*v[0]+v[1]*v[1]+v[2]*v[2];if(l<0.9f||l>1.1f)continue;float dt=(v[0]*h[0]+v[1]*h[1]+v[2]*h[2])/std::sqrt(l);if(std::fabs(dt)<0.97f)continue;
     if(len<int(sizeof(flipDiffText))-40){len+=std::snprintf(flipDiffText+len,sizeof(flipDiffText)-len,"%s0x%x:%+.2f",k?";":"",o,dt);}++k;}}
   ++flipDiffsLogged;flipDiffPending.store(1,std::memory_order_release);}
  if(okState&&statePrevValid&&stateOwner==owner&&driveLastOutValid&&h[0]*driveLastOut[0]+h[1]*driveLastOut[1]+h[2]*driveLastOut[2]<0.98f){
   float lat[3]{n[2],0.f,-n[0]};float ll=std::sqrt(lat[0]*lat[0]+lat[2]*lat[2]); // world-up x normal, horizontal wall axis
   if(ll>0.5f){float hl=(h[0]*lat[0]+h[2]*lat[2])/ll;
    if(std::fabs(hl)>0.97f){(hl>0?slotEventsPos:slotEventsNeg).fetch_add(1,std::memory_order_relaxed);
     for(unsigned o=0;o+12<=kStateSpan;o+=4){for(int pass=0;pass<2;++pass){const float* v=&(pass?statePrev:stateCur)[o/4];
       if(!std::isfinite(v[0])||!std::isfinite(v[1])||!std::isfinite(v[2])){continue;}
       float l=v[0]*v[0]+v[1]*v[1]+v[2]*v[2];if(l<0.9f||l>1.1f){continue;}
       float dt=(v[0]*h[0]+v[1]*h[1]+v[2]*h[2])/std::sqrt(l);if(std::fabs(dt)<0.97f){continue;}
       if(pass){if(dt>0)slotPrevSame[o/4].fetch_add(1,std::memory_order_relaxed);}else (dt>0?slotSame:slotOpp)[o/4].fetch_add(1,std::memory_order_relaxed);}}}}}
  if(okState){std::memcpy(statePrev,stateCur,sizeof(statePrev));statePrevValid=true;stateOwner=owner;}else statePrevValid=false;}
 // M5d diagnostic: whenever the game's own update replaced our previous direction, record stick, camera
 // forward/up, our value and the game's value, to fit the game's stick->direction mapping offline.
 if(driveLastOutValid){float dl=h[0]*driveLastOut[0]+h[1]*driveLastOut[1]+h[2]*driveLastOut[2];
  if(dl<0.98f){float cam[8]{};bool camOk=ws_camref_this&&ws_camref_this<=UINTPTR_MAX-0x1000&&msm2::readMemory(ws_camref_this+0xD34,cam,sizeof(cam));
   float e[17]{sx,sy,driveLastOut[0],driveLastOut[1],driveLastOut[2],h[0],h[1],h[2],n[0],n[1],n[2],camOk?cam[0]:0.f,camOk?cam[1]:0.f,camOk?cam[2]:0.f,camOk?cam[4]:0.f,camOk?cam[5]:0.f,camOk?cam[6]:0.f};
   auto& snap=dl<-0.5f?driveFlipSnap:driveOverrideSnap;for(int i=0;i<17;++i){snap[i].store(e[i],std::memory_order_relaxed);}
   (dl<-0.5f?driveFlips:driveOverrides).fetch_add(1,std::memory_order_release);}}
 float nn=n[0]*n[0]+n[1]*n[1]+n[2]*n[2];
 if(!std::isfinite(nn)||nn<0.9f||nn>1.1f||std::fabs(n[1])>0.2f){driveBadNormal.fetch_add(1,std::memory_order_relaxed);return;}
 float u[3]{-n[0]*n[1],1.f-n[1]*n[1],-n[2]*n[1]};float un=std::sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);for(float& c:u)c/=un;
 float l[3]{u[1]*n[2]-u[2]*n[1],u[2]*n[0]-u[0]*n[2],u[0]*n[1]-u[1]*n[0]};float ln=std::sqrt(l[0]*l[0]+l[1]*l[1]+l[2]*l[2]);if(ln<0.5f)return;for(float& c:l)c/=ln;

 // M5c: camera-relative mapping once the camera forward (+0x1E0 of the camera manager) and handedness are proven.
 bool cameraMapped=false;float d[3]{};
 unsigned camOff=ws_camera_forward_offset.load(std::memory_order_relaxed);
 if(int hand=ws_camera_handedness.load(std::memory_order_relaxed);hand&&camOff&&ws_camref_this&&ws_camref_this<=UINTPTR_MAX-0x1010){
  // M5c-r5: live camera forward every frame (no latch). Near-singular view (camera looking straight at the wall)
  // keeps the last valid screen-forward instead of falling back to the world-up mapping.
  float f[3]{};
  if(msm2::readMemory(ws_camref_this+camOff,f,sizeof(f))&&std::isfinite(f[0])&&std::isfinite(f[1])&&std::isfinite(f[2])){
   float fl=std::sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
   if(fl>0.95f&&fl<1.05f){
    float fs=float(ws_camera_forward_sign.load(std::memory_order_relaxed));for(float& c:f)c*=fs/fl;
    // M5c-r7: camera up is the next matrix row (+0x10 after forward; runtime scan, orthonormal in 5/5 scans).
    // Projections of forward and up onto the wall point the same screen-forward way, so their sum stays large
    // when the camera looks straight at the wall.
    bool haveSide=false;float sideP[3]{},sideLen=0;(void)sideLen;
    {float u[3]{};if(kCameraUpAssist&&msm2::readMemory(ws_camref_this+camOff+0x10,u,sizeof(u))&&std::isfinite(u[0])&&std::isfinite(u[1])&&std::isfinite(u[2])){
      float ul=u[0]*u[0]+u[1]*u[1]+u[2]*u[2],fu=f[0]*u[0]+f[1]*u[1]+f[2]*u[2];
      if(ul>0.9f&&ul<1.1f&&std::fabs(fu)<0.08f){
       // M5c-r8: camera side axis c=f x u. Its wall projection is the screen-horizontal line on the wall, so
       // screen-forward = proj(c) x n. Stays defined while the camera rolls onto the wall (f and u both tilted
       // 45 deg into the wall make proj(f+u) vanish). Sign: equals the proj(f+u) direction on unrolled frames.
       float c[3]{f[1]*u[2]-f[2]*u[1],f[2]*u[0]-f[0]*u[2],f[0]*u[1]-f[1]*u[0]};
       float cn=c[0]*n[0]+c[1]*n[1]+c[2]*n[2];float pc[3]{c[0]-cn*n[0],c[1]-cn*n[1],c[2]-cn*n[2]};
       float pcl=std::sqrt(pc[0]*pc[0]+pc[1]*pc[1]+pc[2]*pc[2]);
       if(kCameraSideAxis&&pcl>0.3f){haveSide=true;sideLen=pcl;
        sideP[0]=pc[1]*n[2]-pc[2]*n[1];sideP[1]=pc[2]*n[0]-pc[0]*n[2];sideP[2]=pc[0]*n[1]-pc[1]*n[0];for(float& v:sideP)v/=pcl;}
       for(int i=0;i<3;++i){f[i]+=fs*u[i];}driveUpAssist.fetch_add(1,std::memory_order_relaxed);}}}
    float fn=f[0]*n[0]+f[1]*n[1]+f[2]*n[2];float p[3]{f[0]-fn*n[0],f[1]-fn*n[1],f[2]-fn*n[2]};
    float pn=std::sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
    if(haveSide){std::memcpy(p,sideP,sizeof(p));pn=1.f;driveSideAxis.fetch_add(1,std::memory_order_relaxed);}
    int mode=0;
    if(pn>0.2f){for(float& c:p)c/=pn;std::memcpy(driveLatchP,p,sizeof(p));driveLatched=true;mode=1;}
    else if(driveLatched){float q=driveLatchP[0]*n[0]+driveLatchP[1]*n[1]+driveLatchP[2]*n[2];
     for(int i=0;i<3;++i){p[i]=driveLatchP[i]-q*n[i];}float qn=std::sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
     if(qn>0.5f){for(float& c:p)c/=qn;mode=2;}}
    if(mode){
     float r[3]{p[1]*n[2]-p[2]*n[1],p[2]*n[0]-p[0]*n[2],p[0]*n[1]-p[1]*n[0]};for(float& c:r)c*=float(hand);
     for(int i=0;i<3;++i){d[i]=sx*r[i]+sy*p[i];}
     cameraMapped=true;if(mode==2)driveHeldForward.fetch_add(1,std::memory_order_relaxed);}
    float snap[16]{f[0],f[1],f[2],n[0],n[1],n[2],sx,sy,d[0],d[1],d[2],h[0],h[1],h[2],pn,float(mode)};
    for(int i=0;i<16;++i){driveSnap[i].store(snap[i],std::memory_order_relaxed);}driveSnapSeq.fetch_add(1,std::memory_order_release);
   }
  }
 }
 if(!cameraMapped){
 if(!driveLateralSign){
  // Calibrate screen-right once: vanilla picked a horizontal axis direction for a clearly sideways input.
  float hl=h[0]*l[0]+h[1]*l[1]+h[2]*l[2];
  if(std::fabs(sx)>0.5f&&std::fabs(sy)<0.3f&&std::fabs(h[1])<0.1f&&std::fabs(hl)>0.9f){driveLateralSign=(hl>0)==(sx>0)?1:-1;driveCalibrations.fetch_add(1,std::memory_order_relaxed);}
  else{driveUncalibrated.fetch_add(1,std::memory_order_relaxed);return;}
 }
 for(int i=0;i<3;++i)d[i]=sx*float(driveLateralSign)*l[i]+sy*u[i];
 }else driveCameraMapped.fetch_add(1,std::memory_order_relaxed);
 float dn=std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);if(dn<1e-3f)return;for(float& c:d)c/=dn;
 // M6a: natural turning. Rotate the written direction toward the stick target inside the wall plane at a capped
 // rate instead of snapping, starting from our previous output (or the current travel when the stick was neutral).
 if(kSmoothTurn){
  LARGE_INTEGER qpc{},freq{};QueryPerformanceCounter(&qpc);QueryPerformanceFrequency(&freq);
  double t=double(qpc.QuadPart)/double(freq.QuadPart),dt=turnLastT>0?t-turnLastT:0.;turnLastT=t;
  if(!(dt>0.)||dt>0.1)dt=0.1;
  float cur[3];if(driveLastOutValid)std::memcpy(cur,driveLastOut,sizeof(cur));else std::memcpy(cur,h,sizeof(cur));
  float cn=cur[0]*n[0]+cur[1]*n[1]+cur[2]*n[2];for(int i=0;i<3;++i){cur[i]-=cn*n[i];}
  float cl=std::sqrt(cur[0]*cur[0]+cur[1]*cur[1]+cur[2]*cur[2]);
  float dnn=d[0]*n[0]+d[1]*n[1]+d[2]*n[2];
  if(cl>0.5f&&std::fabs(dnn)<0.2f){for(float& c:cur)c/=cl;
   float x[3]{cur[1]*d[2]-cur[2]*d[1],cur[2]*d[0]-cur[0]*d[2],cur[0]*d[1]-cur[1]*d[0]};
   float ang=std::atan2(x[0]*n[0]+x[1]*n[1]+x[2]*n[2],cur[0]*d[0]+cur[1]*d[1]+cur[2]*d[2]);
   float maxA=float(kTurnRateDegPerSec*3.14159265358979/180.*dt);
   if(std::fabs(ang)>maxA){float a=ang>0?maxA:-maxA,ca=std::cos(a),sa=std::sin(a);
    float nc[3]{n[1]*cur[2]-n[2]*cur[1],n[2]*cur[0]-n[0]*cur[2],n[0]*cur[1]-n[1]*cur[0]};
    for(int i=0;i<3;++i){d[i]=cur[i]*ca+nc[i]*sa;}
    float l2=std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);for(float& c:d)c/=l2;turnLimited.fetch_add(1,std::memory_order_relaxed);}}
 }
 if(std::fabs(d[0]-h[0])+std::fabs(d[1]-h[1])+std::fabs(d[2]-h[2])<1e-4f&&std::fabs(d[0]-h[3])+std::fabs(d[1]-h[4])+std::fabs(d[2]-h[5])<1e-4f)return;
 if(!writableRange(owner+kHeadingOffsetA,24)){tiltRefusedWritable.fetch_add(1,std::memory_order_relaxed);return;}
 std::memcpy(reinterpret_cast<void*>(owner+kHeadingOffsetA),d,sizeof(d));
 std::memcpy(reinterpret_cast<void*>(owner+kHeadingOffsetB),d,sizeof(d));
 std::memcpy(driveLastOut,d,sizeof(d));driveLastOutValid=true;
  cameraFollow(n,d);
 for(int i=0;i<3;++i){ws::drive::dir[i].store(d[i],std::memory_order_relaxed);}ws::drive::owner.store(owner,std::memory_order_relaxed);ws::drive::ms.store(GetTickCount64(),std::memory_order_release);
 driveWrites.fetch_add(1,std::memory_order_relaxed);++driveOwnerWrites;
}
}

namespace msm2::writer{bool readStickInput(float& x,float& y){return readStick(x,y);}}

extern "C" void ws_update_probe_sample(std::uintptr_t owner) noexcept{
 if(!thresholdProbeEnabled.load(std::memory_order_acquire)||!ws_update_probe_enabled.load(std::memory_order_acquire))return;
 samples.fetch_add(1,std::memory_order_relaxed);
 if(!owner||owner>UINTPTR_MAX-kWallRun250Offset){readFail.fetch_add(1,std::memory_order_relaxed);return;}
 std::uint32_t bits=0;if(!msm2::readMemory(owner+kWallRun250Offset,&bits,sizeof(bits))){readFail.fetch_add(1,std::memory_order_relaxed);return;}
 readOk.fetch_add(1,std::memory_order_relaxed);lastBits.store(bits,std::memory_order_relaxed);
 unsigned expected=0;if(firstCaptured.compare_exchange_strong(expected,1,std::memory_order_acq_rel))firstBits.store(bits,std::memory_order_release);
 float value=asFloat(bits);bool fieldNan=std::isnan(value);
 if(fieldNan)fieldNanSamples.fetch_add(1,std::memory_order_relaxed);else fieldNonNanSamples.fetch_add(1,std::memory_order_relaxed);
 float t0=asFloat(threshold0Bits.load(std::memory_order_relaxed)),t1=asFloat(threshold1Bits.load(std::memory_order_relaxed));
 bool unordered0=fieldNan||std::isnan(t0),unordered1=fieldNan||std::isnan(t1);
 if(unordered0||value>t0)consumer0True.fetch_add(1,std::memory_order_relaxed);
 if(unordered1||value>t1)consumer1Fallthrough.fetch_add(1,std::memory_order_relaxed);
 tiltHeading(owner);
 driveHeading(owner);
 if(auto listener=ws_wallsprint_update_listener.load(std::memory_order_acquire))listener();
}

extern "C" void ws_write_dispatch(unsigned,std::uintptr_t) noexcept{}
extern "C" std::uint64_t ws_write_update_pre(std::uintptr_t) noexcept{return 0;}
extern "C" void ws_write_update_post(std::uintptr_t,std::uint64_t) noexcept{}
extern "C" void ws_write_bridge_reject(const char*) noexcept{ws_write_dispatch_enabled.store(0,std::memory_order_release);}
