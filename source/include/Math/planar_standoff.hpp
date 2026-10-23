#pragma once
#include "diagnostics.hpp"
namespace ws::adhesion {
inline constexpr float target=.5f,deadzone=.01f,maxStep=.02f,maxError=.35f,gain=.25f;
struct Evaluation {bool valid{},inRange{},transitionReady{};float distance{},error{},requested{};Vec3 normal{},delta{};const char* reason="invalid_plane_or_position";};
inline Evaluation evaluate(Vec3 hero,Vec3 point,Vec3 normal,float t){
 Evaluation e{};if(!finite(hero)||!finite(point)||!finite(normal)||!std::isfinite(t))return e;
 auto length=std::sqrt(dot(normal,normal));if(length<.95f||length>1.05f||std::abs(normal.y/length)>.2f)return e;
 e.normal=normal*(1/length);e.distance=dot(hero-point,e.normal);e.error=target-e.distance;if(!std::isfinite(e.distance)||!std::isfinite(e.error))return e;
 e.valid=true;e.inRange=std::abs(e.error)<=maxError;e.transitionReady=t>=1.f;
 e.reason=!e.inRange?"out_of_range":!e.transitionReady?"orientation_transition_incomplete":std::abs(e.error)<=deadzone?"deadzone":"position_proof_not_admitted";
 if(e.inRange&&e.transitionReady&&std::abs(e.error)>deadzone){e.requested=std::clamp(e.error*gain,-maxStep,maxStep);e.delta=e.normal*e.requested;}
 return e;
}
}
