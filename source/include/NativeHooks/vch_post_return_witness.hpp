#pragma once
// Sequential test-owned bridge contract. No production admission or OS dispatch
// simulation. The coordinator reconciles actual callback observations to this.
#include <array>
#include <cstdint>
#include "NativeHooks/debug_register_comparison.hpp"
namespace ws::vch_post_return_witness {
struct Identity {
 std::uint64_t thread{},epoch{},context{},exception{},lease{},window{},generation{},sequence{},originalRip{};
};
inline bool same(const Identity& a,const Identity& b){return a.thread==b.thread&&a.epoch==b.epoch&&a.context==b.context&&a.exception==b.exception&&a.lease==b.lease&&a.window==b.window&&a.generation==b.generation&&a.sequence==b.sequence&&a.originalRip==b.originalRip;}
inline bool valid(const Identity& a){return a.thread&&a.epoch&&a.context&&a.exception&&a.lease&&a.window&&a.generation&&a.sequence&&a.originalRip;}
struct Ledger {
 Identity id{};std::uint32_t token{};bool reserved{},finalSeen{},published{},redirected{},acknowledged{},poison{};
 unsigned reservationOrder{},searchOrder{},finalOrder{},vchOrder{};
 bool refuse(){poison=true;return false;}
 bool reserve(Identity value,unsigned beforeSearch,unsigned escape){
  if(poison||reserved||!valid(value)||!beforeSearch||beforeSearch>=escape)return refuse();
  id=value;reservationOrder=beforeSearch;searchOrder=escape;reserved=true;return true;
 }
 bool observeFinal(Identity value,unsigned downstream,unsigned vch,bool unique,bool contextEqual,bool actualCallback){
  if(poison||!reserved||finalSeen||!same(value,id)||!unique||!contextEqual||!actualCallback||downstream<=searchOrder||vch<=downstream)return refuse();
  finalOrder=downstream;vchOrder=vch;finalSeen=true;return true;
 }
 bool publish(Identity value,std::uint32_t next,bool prepared,bool currentCode,bool range,bool mitigation){
  if(poison||!finalSeen||published||!same(value,id)||!next||!prepared||!currentCode||!range||!mitigation)return refuse();
  token=next;published=true;return true;
 }
 bool redirect(Identity value,bool onlyRip,bool finalStillEqual){
  if(poison||!published||redirected||!same(value,id)||!onlyRip||!finalStillEqual)return refuse();
  redirected=true;return true;
 }
 bool acknowledge(Identity value,std::uint32_t observed,bool currentCode){
  if(poison||!redirected||acknowledged||!same(value,id)||!observed||observed!=token||!currentCode)return refuse();
  acknowledged=true;return true;
 }
 unsigned pending()const{return reserved&&!acknowledged?1:0;}
 bool canCleanup()const{return reserved&&acknowledged&&!poison;}
 bool storageReclaimable(bool allRetainedWorkersExited)const{return allRetainedWorkersExited;}
};
// Strict same-event configuration result, separate from RAW_EXACT/c44 and the
// unchanged c47-v1 profile. No DR6 bit is ignored or written. Intel DR6 reports
// last-event status (accepted SDM253669-090US20.2.3). Require the COMPLETE known
// mixed-event/final-VCH word throughout post-return and final readbacks.
struct Configuration {
 bool nativeWindows{},intel{},admitted{},boundAndAcknowledged{},pollFalse{},leasesInactive{},stable{};
 bool complete{};continuation_diagnostics::Registers saved{},entry{},afterWS{},finalVch{},postReturn{},after{};
 std::array<continuation_diagnostics::Registers,3> readbacks{};
};
inline bool configurationRestored(const Configuration& f){
 if(!f.nativeWindows||!f.intel||!f.admitted||!f.boundAndAcknowledged||!f.pollFalse||!f.leasesInactive||!f.stable||!f.complete)return false;
 const auto statusMasks=debug_register_comparison::masks(debug_register_comparison::Vendor::Intel,6);
 if(f.saved[4]&(0xe00full|statusMasks.unknown|statusMasks.zero))return false;
 if(f.entry[4]!=0xffff0ff5ull||f.afterWS[4]!=0xffff0ff4ull||f.finalVch[4]!=0xffff0ff0ull||f.postReturn[4]!=f.finalVch[4]||f.after[4]!=f.finalVch[4])return false;
 if(f.postReturn!=f.finalVch)return false;
 for(unsigned i:{0u,1u,2u,3u,5u})if(f.entry[i]!=f.afterWS[i]||f.afterWS[i]!=f.finalVch[i])return false;
 if(f.entry[2]!=f.saved[2]||f.entry[3]!=f.saved[3])return false;
 // Last-event DR6 is compared in full to the authenticated final VCH, NOT
 // masked against saved pre-event API status. The original saved/raw/c44
 // comparisons remain independent in the report and may be false.
 const auto m=debug_register_comparison::masks(debug_register_comparison::Vendor::Intel,7);
 auto one=[&](const continuation_diagnostics::Registers& current){
  for(unsigned i=0;i<4;++i)if(current[i]!=f.saved[i])return false;
  const auto delta=current[5]^f.saved[5];
  return current[4]==f.finalVch[4]&&(delta&(m.meaningful|m.osExact|m.unknown))==0&&
   ((current[5]|f.saved[5])&m.unknown)==0&&(current[5]&m.one)==m.one&&(current[5]&m.zero)==0&&
   !(f.entry[5]&(1ull<<11))&&!(f.finalVch[5]&(1ull<<11));
 };
 if(!one(f.after))return false;
 for(const auto& r:f.readbacks)if(!one(r)||r!=f.after)return false;
 return true;
}
} // namespace ws::vch_post_return_witness
