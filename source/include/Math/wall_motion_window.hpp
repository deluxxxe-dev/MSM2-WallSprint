#pragma once
#include "Math/reversal_heading.hpp"
#include <array>
#include <cstdint>
#include <cstring>
namespace ws::motion_diag {
// Worker-owned, value-only ring. No memory access or source permission authority.
struct Frame {
 std::uint64_t ms{},update{},nativeMs{};double clock{},dt{};
 Vec3 delta{},tangent{},normal{},sampledNormal{},requested{},published{};
 std::uint64_t decisionMs{},decisionUpdate{};const char* decision="none";
 diag::ActorAxes actor{};const char* event="none";
};
struct Window {
 static constexpr unsigned capacity=16,maxWindows=16;
 std::array<Frame,capacity> frames{};unsigned count{},head{},windows{};
 std::uint64_t overwritten{},throttled{},suppressed{},lastMs{},lastUpdate{},beginMs{};
 Vec3 position{};bool baseline{},finished{};
 std::uint64_t decisionMs{},decisionUpdate{};const char* decision="none";
 bool sample(Frame f,Vec3 p){
  if(finished)return false;
  if(windows>=maxWindows){++suppressed;return false;}
  if(!baseline||f.ms<lastMs||f.update<lastUpdate){overwritten+=count;count=head=0;beginMs=0;decision="none";decisionMs=decisionUpdate=0;baseline=true;position=p;lastMs=f.ms;lastUpdate=f.update;return false;}
  if(std::strcmp(f.event,"hold")!=0){decision=f.event;decisionMs=f.ms;decisionUpdate=f.update;}
  f.decision=decision;f.decisionMs=decisionMs;f.decisionUpdate=decisionUpdate;
  if(f.update==lastUpdate||f.ms-lastMs<100){++throttled;return false;}
  f.dt=double(f.ms-lastMs);f.delta=p-position;
  f.tangent=f.delta-f.normal*dot(f.delta,f.normal);
  position=p;lastMs=f.ms;lastUpdate=f.update;
  if(!count)beginMs=f.ms;
  frames[head]=f;head=(head+1)%capacity;
  if(count<capacity)++count;else ++overwritten;
  return f.ms-beginMs>=2000;
 }
 const Frame& at(unsigned i)const{return frames[(head+capacity-count+i)%capacity];}
 void flushed(){count=head=0;++windows;}
};
}
