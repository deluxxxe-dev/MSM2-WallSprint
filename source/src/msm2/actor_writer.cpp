// Direct Actor writes remain disabled. The separate b5 module may change only
// a verified caller-local source; it does not use this legacy Actor adapter.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xinput.h>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "msm2/actor_writer.hpp"
#include "msm2/update_bridge.hpp"
#include "msm2/sdk_bridge.hpp"

extern "C" {
// M5b: set by camera_ref::configure; called on the game thread from the verified WallRun Update pre-original callback.
std::atomic<void(*)()> ws_wallsprint_update_listener{nullptr};
// Storage for camera_ref.cpp (kept here so smokes that include only this file link): manager `this` recorded by the
// camera thunk, and the proven camera handedness (0 = unknown).
std::uintptr_t ws_camref_this{};
// M5c accepted values (owner run 2026-11-05: forward_dot 0.985, side_cross 0.991). Used from the start; the ground scan no longer runs.
std::atomic<int> ws_camera_handedness{1};
std::atomic<unsigned> ws_camera_forward_offset{0xd34}; // +0x1E0 rejected: it follows hero travel
std::atomic<int> ws_camera_forward_sign{1};
std::atomic<unsigned> ws_write_dispatch_enabled{0};
std::atomic<unsigned> ws_update_probe_enabled{0};
}

namespace {
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
std::atomic<std::uint64_t> driveWrites{0},driveNoInput{0},driveUncalibrated{0},driveBadNormal{0},driveCalibrations{0},driveCameraMapped{0};
using XInputGetStateFn=DWORD (WINAPI*)(DWORD,XINPUT_STATE*);
std::atomic<XInputGetStateFn> xinputGetState{nullptr};
std::uintptr_t driveOwner{};int driveLateralSign{};
bool driveLatched{};float driveLatchP[3]{}; // camera forward latched at stick press (avoids camera<->heading feedback)
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
bool readStick(float& sx,float& sy){
 sx=sy=0;
 if(auto get=xinputGetState.load()){XINPUT_STATE st{};if(get(0,&st)==ERROR_SUCCESS){
  float x=st.Gamepad.sThumbLX/32767.f,y=st.Gamepad.sThumbLY/32767.f;
  if(x*x+y*y>=kStickDeadzone*kStickDeadzone){sx=std::fmax(-1.f,std::fmin(1.f,x));sy=std::fmax(-1.f,std::fmin(1.f,y));return true;}}}
 float kx=float((GetAsyncKeyState('D')<0)-(GetAsyncKeyState('A')<0)),ky=float((GetAsyncKeyState('W')<0)-(GetAsyncKeyState('S')<0));
 if(kx!=0||ky!=0){sx=kx;sy=ky;return true;}
 return false;
}
void driveHeading(std::uintptr_t owner){
 if(!kM3dStickDrive||owner>UINTPTR_MAX-0x3a4)return;
 if(owner!=driveOwner){driveOwner=owner;driveLateralSign=0;driveOwnerWrites=0;driveLatched=false;}
 {auto now=GetTickCount64();if(now-driveLastCallMs>500)driveOwnerWrites=0;driveLastCallMs=now;}
 if(driveOwnerWrites>=kTiltWriteCap)return;
 float sx,sy;if(!readStick(sx,sy)){driveLatched=false;driveNoInput.fetch_add(1,std::memory_order_relaxed);return;}
 float h[6]{},n[3]{};
 if(!msm2::readMemory(owner+kHeadingOffsetA,h,sizeof(h))||!msm2::readMemory(owner+kWallNormalOffset,n,sizeof(n))){driveBadNormal.fetch_add(1,std::memory_order_relaxed);return;}
 for(float v:h)if(!std::isfinite(v))return;
 float nn=n[0]*n[0]+n[1]*n[1]+n[2]*n[2];
 if(!std::isfinite(nn)||nn<0.9f||nn>1.1f||std::fabs(n[1])>0.2f){driveBadNormal.fetch_add(1,std::memory_order_relaxed);return;}
 float u[3]{-n[0]*n[1],1.f-n[1]*n[1],-n[2]*n[1]};float un=std::sqrt(u[0]*u[0]+u[1]*u[1]+u[2]*u[2]);for(float& c:u)c/=un;
 float l[3]{u[1]*n[2]-u[2]*n[1],u[2]*n[0]-u[0]*n[2],u[0]*n[1]-u[1]*n[0]};float ln=std::sqrt(l[0]*l[0]+l[1]*l[1]+l[2]*l[2]);if(ln<0.5f)return;for(float& c:l)c/=ln;

 // M5c: camera-relative mapping once the camera forward (+0x1E0 of the camera manager) and handedness are proven.
 bool cameraMapped=false;float d[3]{};
 unsigned camOff=ws_camera_forward_offset.load(std::memory_order_relaxed);
 if(int hand=ws_camera_handedness.load(std::memory_order_relaxed);hand&&camOff&&ws_camref_this&&ws_camref_this<=UINTPTR_MAX-0x1010){
  float f[3]{};
  bool haveF=false;
  if(driveLatched){std::memcpy(f,driveLatchP,sizeof(f));haveF=true;}
  else if(msm2::readMemory(ws_camref_this+camOff,f,sizeof(f))&&std::isfinite(f[0])&&std::isfinite(f[1])&&std::isfinite(f[2])){
   float fl=std::sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
   if(fl>0.95f&&fl<1.05f){float fs=float(ws_camera_forward_sign.load(std::memory_order_relaxed));for(float& c:f)c*=fs/fl;std::memcpy(driveLatchP,f,sizeof(f));driveLatched=true;haveF=true;}
  }
  if(haveF){
   float fn=f[0]*n[0]+f[1]*n[1]+f[2]*n[2];float p[3]{f[0]-fn*n[0],f[1]-fn*n[1],f[2]-fn*n[2]};
   float pn=std::sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
   if(pn>0.2f){for(float& c:p)c/=pn;
    float r[3]{p[1]*n[2]-p[2]*n[1],p[2]*n[0]-p[0]*n[2],p[0]*n[1]-p[1]*n[0]};for(float& c:r)c*=float(hand);
    for(int i=0;i<3;++i){d[i]=sx*r[i]+sy*p[i];}
    cameraMapped=true;}
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
 if(std::fabs(d[0]-h[0])+std::fabs(d[1]-h[1])+std::fabs(d[2]-h[2])<1e-4f&&std::fabs(d[0]-h[3])+std::fabs(d[1]-h[4])+std::fabs(d[2]-h[5])<1e-4f)return;
 if(!writableRange(owner+kHeadingOffsetA,24)){tiltRefusedWritable.fetch_add(1,std::memory_order_relaxed);return;}
 std::memcpy(reinterpret_cast<void*>(owner+kHeadingOffsetA),d,sizeof(d));
 std::memcpy(reinterpret_cast<void*>(owner+kHeadingOffsetB),d,sizeof(d));
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
