#include "Math/source_orientation.hpp"
#include "Math/compact_basis.hpp"
#include "GameInterop/atomic_publication.hpp"
#include <cassert>
#include <iostream>
#include <limits>
using namespace ws;using namespace ws::source_math;
int main(){
 Matrix base{};base.v[0]=base.v[5]=base.v[10]=base.v[15]=1;base.v[12]=10;base.v[13]=20;base.v[14]=30;
 for(Vec3 n: {Vec3{0,0,1},Vec3{-.70710678f,0,.70710678f},Vec3{1,0,0},Vec3{0,0,-1}}){
  // Forward must lie on the surface; choose a direction perpendicular to each N.
  auto original=base;Vec3 f;if(std::abs(n.z)>.99f)f={1,0,0};else f={0,0,1};Vec3 r;assert(unit(cross({0,1,0},f),r));float rr[]={r.x,r.y,r.z},ff[]={f.x,f.y,f.z};for(unsigned k=0;k<3;++k){original.v[k]=rr[k];original.v[8+k]=ff[k];}
  Transition a;assert(a.enter(original,n,1000));float t=0;Matrix out{};
  assert(a.sample(original,n,1000,.25f,out,t)&&t==0);auto initial=diag::actorAxes(original.v),start=diag::actorAxes(out.v);assert(dot(start.up,initial.up)>.9999);
  float lastDot=-1;for(unsigned ms=0;ms<=250;ms+=10){assert(a.sample(original,n,1000+ms,.25f,out,t));auto axes=diag::actorAxes(out.v);assert(axes.valid);float d=dot(axes.up,a.surface);assert(d>=lastDot-.0001f);lastDot=d;assert(!std::memcmp(out.v+12,original.v+12,16));assert(out.v[3]==0&&out.v[7]==0&&out.v[11]==0);}
  assert(dot(diag::actorAxes(out.v).up,n)>.9999f);assert(a.sample(original,n,2000,.25f,out,t)&&t==1); // No world-Up restart.
  assert(!a.sample(original,n*-1,2010,.25f,out,t));assert(!a.sample(original,{0,1,0},2010,.25f,out,t));
 }
 Transition bad;assert(!bad.enter(base,{0,1,0},0));assert(!bad.enter(base,{0,0,2},0));assert(!bad.enter(base,{0,0,1},0)); // projected forward degenerates
 auto corrupt=base;corrupt.v[0]=std::numeric_limits<float>::quiet_NaN();assert(!valid(corrupt));corrupt=base;corrupt.v[15]=0;assert(!valid(corrupt));corrupt=base;corrupt.v[4]=1;assert(!valid(corrupt));
 CompactBlock compact{};compact.rows[0]=2;compact.rows[4]=1.5f;compact.rows[8]=.75f;for(unsigned i=0;i<compact.metadata.size();++i)compact.metadata[i]=static_cast<unsigned char>(i+1);
 CompactBlock replacement{};Vec3 scales{};diag::ActorAxes rotated{{0,0,-1},{0,1,0},{1,0,0},true};assert(compactReplacement(compact,rotated,replacement,scales));
 assert(std::abs(scales.x-2)<1e-6f&&std::abs(scales.y-1.5f)<1e-6f&&std::abs(scales.z-.75f)<1e-6f);assert(replacement.metadata==compact.metadata);assert(replacement.rows[2]==-2&&replacement.rows[4]==1.5f&&replacement.rows[6]==.75f);
 auto degenerate=compact;degenerate.rows[4]=0;assert(!compactReplacement(degenerate,rotated,replacement,scales));auto oversized=compact;oversized.rows[0]=3;assert(!compactReplacement(oversized,rotated,replacement,scales));
 struct Publication {std::uint64_t session;float basis[9];bool active;};interop::AtomicPublication<Publication> publication;Publication published{42,{1,0,0,0,1,0,0,0,1},true},read{};publication.publish(published);assert(publication.read(read)&&!std::memcmp(&published,&read,sizeof(read)));
 std::cout<<"PASS: A/E/axis surface basis, 0.25s monotonic shortest quaternion interpolation from captured entry (no restart), fixed heading/position/W preservation, invalid/slope/corner/degenerate/nonfinite rejection\n";
}
