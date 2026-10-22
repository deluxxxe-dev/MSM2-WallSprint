#pragma once
#include "object_candidates.hpp"
namespace ws::discovery {
enum class LayoutKind {Unknown,QuaternionLike,VectorArray,MatrixLike,ConstantBasis};
inline const char* layoutName(LayoutKind k){switch(k){case LayoutKind::QuaternionLike:return "quaternion-like";case LayoutKind::VectorArray:return "vector-array";case LayoutKind::MatrixLike:return "matrix-like";case LayoutKind::ConstantBasis:return "constant-basis";default:return "unknown";}}
inline Vec3 rawVector(const std::array<unsigned char,bytes>& raw,unsigned offset){Vec3 v{};if(offset+sizeof(v)<=bytes)std::memcpy(&v,raw.data()+offset,sizeof(v));return v;}
inline bool vectorNear(Vec3 a,Vec3 b,double tolerance=.0001){return finite(a)&&finite(b)&&diag::preciseDot(a-b,a-b)<tolerance*tolerance;}
struct Layout {
 std::array<unsigned,offsets> cluster{},repeatRepresentative{};
 std::array<LayoutKind,offsets> kind{};
 std::array<bool,offsets> basisSuspect{},overlap{},matrixBoundary{};
};
inline Layout classify(const Candidates& data){
 Layout out;const auto& fields=data.fields();
 unsigned cluster=0,end=0;
 for(unsigned i=0;i<offsets;++i){out.repeatRepresentative[i]=i;
  if(fields[i].samples<2)continue;
  if(i*4>=end){cluster=i*4;end=i*4+12;}else {end=i*4+12;}
  out.cluster[i]=cluster;
 }
 for(unsigned i=0;i<offsets;++i)if(fields[i].samples>=2){
  for(unsigned j=0;j<offsets;++j)if(i!=j&&fields[j].samples>=2&&out.cluster[i]==out.cluster[j])out.overlap[i]=true;
  // Numeric 4-float norm only: quaternion-like is ambiguous with vector+padding.
  if(i*4+16<=bytes){bool both=true;for(const auto* raw:{&data.firstBlock(),&data.lastBlock()}){
   float v[4];std::memcpy(v,raw->data()+i*4,16);double q=0;for(float f:v){both=both&&std::isfinite(f)&&std::abs(f)<=1.1f;q+=double(f)*f;}both=both&&q>.98&&q<1.02;
  }if(both)out.kind[i]=LayoutKind::QuaternionLike;}
 }
 // Matrix hypotheses require three non-overlapping orthonormal rows at stride 12/16.
 for(unsigned p=0;p+36<=bytes;p+=4)for(unsigned stride:{12u,16u}){
  unsigned span=2*stride+12;if(p+span>bytes)continue;
  diag::Basis first{},last{};for(unsigned row=0;row<3;++row){first[row]=rawVector(data.firstBlock(),p+row*stride);last[row]=rawVector(data.lastBlock(),p+row*stride);}
  if(!diag::measure(first).orthonormal||!diag::measure(last).orthonormal)continue;
  for(unsigned row=0;row<3;++row)out.matrixBoundary[(p+row*stride)/4]=true;
  for(unsigned i=0;i<offsets;++i)if(i*4<p+span&&i*4+12>p&&fields[i].samples>=2)out.kind[i]=LayoutKind::MatrixLike;
 }
 // Repeated stable coordinate-axis vectors identify suspected constant basis records.
 // They are grouped numerically, not asserted to be distinct native fields.
 for(unsigned i=0;i<offsets;++i){const auto& c=fields[i];if(c.samples<2||c.samples!=data.count()||c.minStability<.9999||!vectorNear(c.first,c.last))continue;
  const auto v=c.first;bool coordinate=(std::abs(v.x)>.9999&&std::abs(v.y)<.0001&&std::abs(v.z)<.0001)||(std::abs(v.y)>.9999&&std::abs(v.x)<.0001&&std::abs(v.z)<.0001)||(std::abs(v.z)>.9999&&std::abs(v.x)<.0001&&std::abs(v.y)<.0001);
  if(!coordinate)continue;
  unsigned matches=0,representative=i;
  for(unsigned j=0;j<offsets;++j){const auto& d=fields[j];if(d.samples==data.count()&&d.minStability>.9999&&vectorNear(v,d.first)&&vectorNear(v,d.last)){++matches;representative=std::min(representative,j);}}
  if(matches>=3){out.basisSuspect[i]=true;out.repeatRepresentative[i]=representative;out.kind[i]=LayoutKind::ConstantBasis;}
 }
 // Non-overlapping unit records with common stride support vector-array, not field identity.
 for(unsigned i=0;i<offsets;++i)for(unsigned stride:{3u,4u,5u,6u}){
  if(i+2*stride>=offsets)continue;
  if(fields[i].samples>=2&&fields[i+stride].samples>=2&&fields[i+2*stride].samples>=2)
   for(unsigned j:{i,i+stride,i+2*stride})if(out.kind[j]==LayoutKind::Unknown||out.kind[j]==LayoutKind::QuaternionLike)out.kind[j]=LayoutKind::VectorArray;
 }
 return out;
}
}
