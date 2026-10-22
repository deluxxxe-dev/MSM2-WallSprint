#include "msm2/orientation_log.hpp"
#include <cstring>
#include "msm2/wallrun_hooks.hpp"
namespace {
void vector(const char* label,ws::Vec3 v){msm2::writeLog("%s=(%.7f,%.7f,%.7f) opposite=(%.7f,%.7f,%.7f)",label,v.x,v.y,v.z,-v.x,-v.y,-v.z);}
void basis(const char* layout,const char* candidate,const ws::diag::Basis& axes,const ws::diag::Metrics& m){
 msm2::writeLog("basis %s candidate=%s finite=%u orthonormal=%u proper_rotation=%u axis_mapping=row_major_for_target_build_or_secondary_transpose",layout,candidate,unsigned(m.finite),unsigned(m.orthonormal),unsigned(m.properRotation));
 for(unsigned i=0;i<3;++i)msm2::writeLog("%c%u=(%.7f,%.7f,%.7f)",layout[0]=='r'?'R':'C',i,axes[i].x,axes[i].y,axes[i].z);
 msm2::writeLog("%s length=(%.9f,%.9f,%.9f) dot01=%.9g dot02=%.9g dot12=%.9g det=%.9f orthogonality_error_Frobenius=%.9g",layout,m.length[0],m.length[1],m.length[2],m.pairDot[0],m.pairDot[1],m.pairDot[2],m.determinant,m.orthogonalityError);
 msm2::writeLog("candidate %s: right=axis0 up=axis1 forward=axis2 (row convention accepted for target build; sanity checks required)",candidate);
 vector("candidate_right (+right,-right)",axes[0]);vector("candidate_up (+up,-up)",axes[1]);vector("candidate_forward (+forward,-forward)",axes[2]);
}
}
namespace msm2 {
void dumpDiagnostic(const Snapshot* s,const ws::diag::Motion& motion,std::uint64_t tick,std::uint64_t id,const char* trigger){
 writeLog("=== %s === sample_id=%llu monotonic_ms=%llu hero_epoch=%llu ground_truth_source=%s non_wallrun_state=Unknown",trigger,static_cast<unsigned long long>(id),static_cast<unsigned long long>(tick),static_cast<unsigned long long>(motion.epoch),std::strncmp(trigger,"MARK_",5)==0?"USER_MARKER":"NONE");
 if(!s){writeLog("snapshot=unavailable hero=0 transform=0 position=unavailable yaw_scalar=unavailable raw_transform=unavailable basis=unavailable velocity_estimate=unavailable (NO stale sample substituted)");}
 else {
  writeLog("hero=0x%llx transform=0x%llx position=(%.6f,%.6f,%.6f) yaw_scalar_template=%.7f",static_cast<unsigned long long>(s->hero),static_cast<unsigned long long>(s->transform),s->position[0],s->position[1],s->position[2],s->yaw);
  for(unsigned r=0;r<3;++r)writeLog("transform_raw_0x%02x=(%.7f,%.7f,%.7f,%.7f)",r*16,s->prefix[r*4],s->prefix[r*4+1],s->prefix[r*4+2],s->prefix[r*4+3]);
  auto orientation=ws::diag::analyze(s->prefix);
  basis("row-major","PRIMARY",orientation.rows,orientation.rowMetrics);
  writeLog("column-major secondary_numeric_check_only det=%.9f orthogonality_error=%.9g NOT_actor_axes",orientation.columnMetrics.determinant,orientation.columnMetrics.orthogonalityError);
  auto axes=ws::diag::actorAxes(s->prefix);
  writeLog("actor_axes valid=%u Right=R0 Up=R1 Forward=R2 world_up=(0,1,0) source=user_log_validated_target_build",unsigned(axes.valid));
  if(motion.valid)writeLog("velocity_estimate=(%.6f,%.6f,%.6f) source=position_difference NOT_physics_velocity",motion.velocity.x,motion.velocity.y,motion.velocity.z);else writeLog("velocity_estimate=unavailable_or_discontinuous NOT_physics_velocity");
 }
 msm2::hooks::logState(tick);
 writeLog("WallCrawl=Unknown Ground=Unknown Air=Unknown surface_hit=unavailable point=unavailable normal=unavailable distance=unavailable world_up=(0,1,0) abs_dot_normal_world_up=unavailable");
 writeLog("END_DIAGNOSTIC sample_id=%llu",static_cast<unsigned long long>(id));
}
}
