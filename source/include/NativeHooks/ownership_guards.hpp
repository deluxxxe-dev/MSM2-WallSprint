#pragma once
#include <array>
#include <cstdint>
#include <cstring>
namespace msm2::guards {
struct PatchStatus {
 bool enabled{},readOk{},match{};unsigned firstDifference{32};
 bool good() const noexcept {return enabled&&readOk&&match;}
};
template<class Reader> PatchStatus inspectPatch(bool enabled,std::uintptr_t address,const unsigned char* expected,Reader read){
 PatchStatus s{};s.enabled=enabled;unsigned char bytes[32]{};
 s.readOk=address&&read(address,bytes,32);
 if(s.readOk){s.match=true;for(unsigned i=0;i<32;++i)if(bytes[i]!=expected[i]){s.match=false;s.firstDifference=i;break;}}
 return s;
}
struct ObservationStatus {
 bool verifiedAvailable{},fieldPlanEnabled{};std::array<PatchStatus,4> hooks{};
 bool sourceIntegrity() const noexcept {if(!verifiedAvailable)return false;for(auto s:hooks)if(!s.good())return false;return true;}
 bool diagnosticViable() const noexcept {return fieldPlanEnabled&&sourceIntegrity();}
 unsigned failureMask() const noexcept {
  unsigned mask=unsigned(!verifiedAvailable)|(unsigned(!fieldPlanEnabled)<<1);
  for(unsigned i=0;i<4;++i){auto s=hooks[i];mask|=(unsigned(!s.enabled)|(unsigned(!s.readOk)<<1)|(unsigned(!s.match)<<2))<<(2+3*i);}return mask;
 }
 const char* failedHook() const noexcept {
  constexpr const char* names[]={"Init","Entry","Update","Deinit"};for(unsigned i=0;i<4;++i)if(!hooks[i].good())return names[i];return "none";
 }
};
}
