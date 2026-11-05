#pragma once
#include "wallsprint.hpp"
#include <cstdint>
namespace ws::observed {
inline constexpr const char* semantic="VERIFIED_WALLRUN_LIFECYCLE_FOR_BUILD_2_810_0_0";
enum class WallRunObservedState {Inactive,Active};
struct VerifiedWallContext {
 bool lifecycleVerified{},wallRunActive{},surfaceValid{};
 std::uint64_t wallRunSessionId{};std::uintptr_t wallRunOwner{};
 double wallRunStartMs{},elapsed{};std::uint32_t wallRunUpdateCalls{};
 Vec3 surfacePoint{},surfaceNormal{};float surfaceDistance{};
};
class Observer {
 VerifiedWallContext value{};std::uint64_t next{};double lastUpdate{};
 const char* reason="startup";
 // A long game stall (frame drop/streaming) can exceed the update timeout without a Deinit. The timed-out
 // session is remembered so an Update from the same owner (no Init/Deinit in between) resumes it unchanged.
 VerifiedWallContext timedOut{};bool resumedFlag{};
public:
 void configure(bool verified){reset("reconfigure");timedOut={};value.lifecycleVerified=verified;}
 bool takeResumed(){bool r=resumedFlag;resumedFlag=false;return r;}
 void reset(const char* why){bool verified=value.lifecycleVerified;value={};value.lifecycleVerified=verified;reason=why;lastUpdate=0;}
 const char* exitReason()const{return reason;}
 VerifiedWallContext context(double now)const {auto out=value;if(out.wallRunActive)out.elapsed=std::max(0.,now-out.wallRunStartMs)/1000.;return out;}
 // Only Init can begin a session. Stray Update/Entry/Deinit cannot create one.
 bool event(unsigned kind,std::uintptr_t owner,double ms){
  if(!value.lifecycleVerified||kind>3||!owner||!std::isfinite(ms))return false;
  if(!value.wallRunActive&&timedOut.wallRunActive){
   if(kind==2&&owner==timedOut.wallRunOwner&&ms>=timedOut.wallRunStartMs){bool verified=value.lifecycleVerified;value=timedOut;value.lifecycleVerified=verified;timedOut={};
    ++value.wallRunUpdateCalls;lastUpdate=ms;reason="none";resumedFlag=true;return true;}
   if(kind==0||kind==3||owner!=timedOut.wallRunOwner)timedOut={};
  }
  if(value.wallRunActive&&owner!=value.wallRunOwner){reset("candidate_owner_mismatch");return false;}
  if(kind==0){
   if(value.wallRunActive){reset("unexpected_duplicate_init");return false;}
   value.wallRunActive=true;value.wallRunSessionId=++next;value.wallRunOwner=owner;value.wallRunStartMs=ms;lastUpdate=ms;reason="none";return true;
  }
  if(!value.wallRunActive)return false;
  if(ms<value.wallRunStartMs){reset("event_time_discontinuity");return false;}
  if(kind==2){++value.wallRunUpdateCalls;lastUpdate=std::max(lastUpdate,ms);}
  if(kind==3)reset("verified_deinit");
  return true;
 }
 void expire(double now){if(value.wallRunActive&&now-lastUpdate>5000.){auto keep=value;reset("emergency_update_timeout_5s");timedOut=keep;}}
};
inline const char* subtype(Vec3 v,bool valid){
 if(!valid||!finite(v))return "Unavailable";
 double vertical=std::abs(v.y),horizontal=std::hypot(v.x,v.z);
 if(std::max(vertical,horizontal)<1.)return "Mixed";
 if(vertical>2*horizontal)return "WallRunUp";
 if(horizontal>2*vertical)return "WallRunSide";
 return "Mixed";
}
}
