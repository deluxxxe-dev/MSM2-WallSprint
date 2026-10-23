#pragma once
#include "Math/source_orientation.hpp"
#include <array>
namespace ws::source_math {
struct CompactBlock {float rows[9]{};std::array<unsigned char,28> metadata{};};
static_assert(sizeof(CompactBlock)==64);
inline bool compactAxes(const CompactBlock& original,diag::ActorAxes& axes,Vec3& lengths){
 Vec3 unitRows[3];float scales[3];
 for(unsigned i=0;i<3;++i){Vec3 v{original.rows[3*i],original.rows[3*i+1],original.rows[3*i+2]};if(!finite(v))return false;
  float s=std::sqrt(dot(v,v));if(!std::isfinite(s)||s<.5f||s>2.f||!unit(v,unitRows[i]))return false;scales[i]=s;
 }
 if(!diag::measure({unitRows[0],unitRows[1],unitRows[2]}).properRotation)return false;
 axes={unitRows[0],unitRows[1],unitRows[2],true};lengths={scales[0],scales[1],scales[2]};return true;
}
inline bool compactReplacement(const CompactBlock& original,const diag::ActorAxes& shared,CompactBlock& result,Vec3& scales){
 diag::ActorAxes incoming{};if(!compactAxes(original,incoming,scales)||!shared.valid||!diag::measure({shared.right,shared.up,shared.forward}).properRotation)return false;
 result=original;Vec3 rows[]={shared.right*scales.x,shared.up*scales.y,shared.forward*scales.z};
 for(unsigned i=0;i<3;++i){result.rows[i*3]=rows[i].x;result.rows[i*3+1]=rows[i].y;result.rows[i*3+2]=rows[i].z;}
 return result.metadata==original.metadata;
}
}
