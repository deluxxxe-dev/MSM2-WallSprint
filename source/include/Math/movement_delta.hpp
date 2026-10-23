#pragma once
#include "diagnostics.hpp"
namespace ws::movement {
struct Delta {Vec3 world{},velocity{};float normal{},forward{},right{},speed{};bool valid{};};
inline Delta decompose(Vec3 before,Vec3 after,const diag::ActorAxes& frame,float dtMs){
 Delta r{};if(!finite(before)||!finite(after)||!frame.valid||!diag::measure({frame.right,frame.up,frame.forward}).properRotation||!std::isfinite(dtMs)||dtMs<=0||dtMs>500)return r;
 r.world=after-before;r.normal=dot(r.world,frame.up);r.forward=dot(r.world,frame.forward);r.right=dot(r.world,frame.right);r.velocity=r.world*(1000.f/dtMs);r.speed=std::sqrt(dot(r.velocity,r.velocity));r.valid=finite(r.world)&&finite(r.velocity)&&std::isfinite(r.speed);return r;
}
}
