#pragma once
#include <windows.h>
#include <bcrypt.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "msm2/sdk_bridge.hpp"
#include "surface_calibration.hpp"
#include "this_access.hpp"
#include "static_field_observation.hpp"
namespace msm2::source {
struct PointerEvidence {unsigned offset{},reads{},vectors{},maxChildEnd{};std::uintptr_t instruction{};};
struct Channel {PointerEvidence evidence;std::uintptr_t previousChild{},child{};std::uint64_t previousSession{};std::uint64_t objectId{};ws::discovery::Candidates data;ws::calibration::Tracker tracker;};
inline std::map<unsigned,PointerEvidence> evidence;
inline std::array<Channel,4> channels;
inline ws::fields::Observer fieldObserver;
inline std::array<bool,4> planAdmitted{};inline bool fieldPlanEnabled{};
inline bool runtimeObservationEnabled=true;
inline unsigned count{};inline int loggingChannel=-1;inline bool enabled{};inline std::uint64_t nextId{},session{},epoch{};inline std::uintptr_t hero{},transform{};
inline void childLog(const char* format,...){
 char body[3500]{};va_list args;va_start(args,format);std::vsnprintf(body,sizeof(body),format,args);va_end(args);
 if(loggingChannel<0){msm2::writeLog("%s",body);return;}
 const auto& c=channels[unsigned(loggingChannel)];auto p=std::strstr(body,"child_offset=0x");unsigned skip=15;
 if(!p){p=std::strstr(body,"offset=0x");skip=9;}
 if(p){auto offset=std::strtoul(p+skip,nullptr,16);msm2::writeLog("source_path=WallRun.this+0x%x->child+0x%lx child_object_id=%llu child_pointer=0x%llx %s",c.evidence.offset,offset,static_cast<unsigned long long>(c.objectId),static_cast<unsigned long long>(c.child),body);}
 else msm2::writeLog("source_path=WallRun.this+0x%x->child child_object_id=%llu child_pointer=0x%llx %s",c.evidence.offset,static_cast<unsigned long long>(c.objectId),static_cast<unsigned long long>(c.child),body);
}
inline std::string sha256(const std::vector<unsigned char>& data){
 BCRYPT_ALG_HANDLE alg=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;unsigned char digest[32]{};DWORD n=0,done=0;std::vector<unsigned char> object;bool ok=BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
 if(ok)ok=BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&n),sizeof(n),&done,0)>=0;
 if(ok){object.resize(n);ok=BCryptCreateHash(alg,&hash,object.data(),n,nullptr,0,0)>=0;}
 if(ok)ok=BCryptHashData(hash,const_cast<PUCHAR>(data.data()),static_cast<ULONG>(data.size()),0)>=0;
 if(ok)ok=BCryptFinishHash(hash,digest,sizeof(digest),0)>=0;
 if(hash){BCryptDestroyHash(hash);}
 if(alg){BCryptCloseAlgorithmProvider(alg,0);}
 char text[65]{};if(ok){for(unsigned i=0;i<32;++i)std::snprintf(text+i*2,3,"%02x",digest[i]);return text;}return "unavailable";
}
inline void authenticatePlan(const char* name,const std::vector<unsigned char>& bytes){
 for(unsigned i=0;i<4;++i){const auto& p=ws::static_plan::functions[i];if(std::strcmp(p.name,name)==0){planAdmitted[i]=bytes.size()==p.bytes&&sha256(bytes)==p.hash;writeLog("static_access_v2 function=%s authenticated_plan=%u decoder=offline_Capstone_5.0.6 decoded_bytes=%u decoded_instructions=%u decoded_instruction_coverage=%.6f unsupported_byte_skip=0 semantics_complete=0 game_call=0",name,unsigned(planAdmitted[i]),p.bytes,p.instructions,planAdmitted[i]?1.:0.);}}
}
inline void analyzeNative(const char* name,std::uintptr_t address){
 // Invoked only after this function's unique AOB/prefix/exact-build guard, BEFORE patching it.
 DWORD64 base=0;auto fn=RtlLookupFunctionEntry(address,&base,nullptr);auto game=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
 if(!fn||base!=game||base+fn->BeginAddress!=address||fn->EndAddress<=fn->BeginAddress||fn->EndAddress-fn->BeginAddress>0x4000){writeLog("this_access_map function=%s available=0 reason=bounded_function_span_rejected",name);return;}
 unsigned size=fn->EndAddress-fn->BeginAddress;std::vector<unsigned char> bytes(size);if(!readMemory(address,bytes.data(),bytes.size())){writeLog("this_access_map function=%s available=0 reason=code_read_failed",name);return;}
 writeLog("native_function_capture function=%s rva=0x%llx size=%u sha256=%s source=admitted_native_function_before_MinHook instruction_decoder=HDE64_legacy_x64 no_new_game_call=1",name,static_cast<unsigned long long>(address-game),size,sha256(bytes).c_str());
 for(unsigned pos=0;pos<size;pos+=32){char hex[65]{};unsigned n=std::min(32u,size-pos);for(unsigned i=0;i<n;++i)std::snprintf(hex+2*i,3,"%02x",bytes[pos+i]);writeLog("native_function_code function=%s offset=0x%x hex=%s",name,pos,hex);}
 authenticatePlan(name,bytes);
 auto map=ws::native::analyze(bytes);
 writeLog("this_access_map function=%s converged=%u decoded=%u unknown_instructions=%u cfg_merge=alias_intersection unknown_writes=kill_provenance complete_type_recovery=0",name,unsigned(map.converged),map.decoded,map.unknownInstructions);
 if(!map.converged)return;
 for(const auto& row:map.accesses){
  auto ins=address+row.pc;auto scope=row.base==ws::native::Origin::This?"this":"one_level_child";
  writeLog("wallrun_this_access function=%s instruction_rva=0x%llx scope=%s parent_field_signed=%d offset_signed=%d operation=%s width=%u pointer_load=%u indirect_call=%u possible_type=%s",name,static_cast<unsigned long long>(ins-game),scope,row.field,row.offset,row.operation,row.width,unsigned(row.pointerLoad),unsigned(row.indirectCall),row.pointerLoad?"64bit_value_loaded_NOT_verified_pointer_type":row.width==16?"vector_or_packed_data":"scalar_or_unknown");
  if(row.base==ws::native::Origin::Child&&row.field>=0&&row.field<=0x400-8&&row.field%8==0&&row.offset>=0&&row.offset<0x400&&!row.indirectCall&&std::strcmp(row.operation,"UNKNOWN")!=0){
   auto& e=evidence[unsigned(row.field)];e.offset=unsigned(row.field);++e.reads;e.vectors+=row.width==16;e.maxChildEnd=std::max(e.maxChildEnd,unsigned(row.offset)+row.width);e.instruction=ins;
  }
 }
 for(auto pc:map.directCalls){hde64s h{};unsigned char padded[16]{};std::memcpy(padded,bytes.data()+pc,std::min(std::size_t(16),bytes.size()-pc));hde64_disasm(padded,&h);auto delta=static_cast<std::int32_t>(h.imm.imm32);auto target=static_cast<std::uintptr_t>(static_cast<std::int64_t>(address+pc+h.len)+delta);
  writeLog("wallrun_direct_callee function=%s call_rva=0x%llx target=0x%llx semantic_identity=UNVERIFIED_SURFACE_FUNCTION hook_installed=0 no_call_from_mod=1",name,static_cast<unsigned long long>(address+pc-game),static_cast<unsigned long long>(target));
 }
}
inline void reset(const char* reason){fieldObserver.reset();for(unsigned i=0;i<count;++i){loggingChannel=int(i);channels[i].tracker.reset(reason,childLog);channels[i].data={};channels[i].child=channels[i].previousChild=0;channels[i].previousSession=0;channels[i].objectId=0;}loggingChannel=-1;session=0;}
inline void prepare(){reset("restart");enabled=fieldPlanEnabled=false;planAdmitted={};for(auto& c:channels){c.previousChild=0;c.previousSession=0;}count=0;evidence.clear();}
inline void finalize(bool verified,bool requested){
 fieldPlanEnabled=verified&&requested&&std::all_of(planAdmitted.begin(),planAdmitted.end(),[](bool v){return v;});
 writeLog("static_field_plan enabled=%u selected_root_fields=%u root_range_cap=0x480 runtime_code_hash_gate=all_four_original_bodies blind_root_scan=0 unknown_game_call=0",unsigned(fieldPlanEnabled),ws::fields::roots);
 enabled=verified&&requested;if(!enabled){writeLog("surface_source_discovery enabled=0 reason=full_build_hook_gate_or_INI_off");return;}
 std::vector<PointerEvidence> sorted;for(auto& item:evidence)sorted.push_back(item.second);std::sort(sorted.begin(),sorted.end(),[](const auto& a,const auto& b){if(a.vectors!=b.vectors)return a.vectors>b.vectors;if(a.reads!=b.reads)return a.reads>b.reads;return a.offset<b.offset;});
 count=std::min(unsigned(sorted.size()),unsigned(channels.size()));for(unsigned i=0;i<count;++i){channels[i].evidence=sorted[i];writeLog("surface_pointer_plan source_path=WallRun.this+0x%x->child static_native_dereference_evidence=1 max_native_child_access=0x%x observation_bytes=0x%x depth=1 instruction=0x%llx",sorted[i].offset,sorted[i].maxChildEnd,fieldPlanEnabled&&sorted[i].offset==0x128?0x300:0x200,static_cast<unsigned long long>(sorted[i].instruction));}
 writeLog("surface_source_discovery enabled=1 candidate_child_pointer_count=%u statically_supported_fields=%zu max_children=4 root_field_cap=0x400 child_cap=0x300_only_for_authenticated_0x128_otherwise_0x200 direct_root_Vec3_ranking=STATIC_FIELDS_ONLY",count,sorted.size());
 writeLog("surface_field_exclusion source_path=WallRun.this+0x128->child+0x14 status=CONSTANT_WORLD_AXIS_SUSPECT surface_axis_ranking=EXCLUDED evidence=v01a7_all_four_faces_fixed_0_1_0");
 writeLog("surface_getter_candidate semantic_identity=UNVERIFIED_SURFACE_FUNCTION hook_installed=0 reason=no_authenticated_build_specific_surface_getter_identity ABI_unknown=1");
}
inline bool readable(std::uintptr_t address,void* out,std::size_t size,bool child){
 if(!address||!size||size>0x400||address>UINTPTR_MAX-size)return false;
 auto pos=address,end=address+size;
 while(pos<end){MEMORY_BASIC_INFORMATION m{};if(!VirtualQuery(reinterpret_cast<void*>(pos),&m,sizeof(m))||m.State!=MEM_COMMIT||(m.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;auto prot=m.Protect&0xff;
  if(prot!=PAGE_READONLY&&prot!=PAGE_READWRITE&&prot!=PAGE_WRITECOPY&&prot!=PAGE_EXECUTE_READ&&prot!=PAGE_EXECUTE_READWRITE&&prot!=PAGE_EXECUTE_WRITECOPY)return false;
  if(child&&(m.Type==MEM_IMAGE||prot==PAGE_EXECUTE_READ||prot==PAGE_EXECUTE_READWRITE||prot==PAGE_EXECUTE_WRITECOPY))return false;
  auto base=reinterpret_cast<std::uintptr_t>(m.BaseAddress);if(m.RegionSize>UINTPTR_MAX-base||base+m.RegionSize<=pos)return false;pos=std::min(end,base+m.RegionSize);
 }
 return readMemory(address,out,size);
}
inline void begin(std::uint64_t id,std::uintptr_t h,std::uintptr_t t,std::uint64_t e){
 if(!runtimeObservationEnabled)return;
 session=id;hero=h;transform=t;epoch=e;
 if(fieldPlanEnabled)fieldObserver.begin(id,h,t,e,GetTickCount64());
 char offsets[128]{};unsigned length=0;for(unsigned i=0;i<count;++i){int n=std::snprintf(offsets+length,sizeof(offsets)-length,"%s0x%x",i?",":"",channels[i].evidence.offset);if(n>0)length=std::min(unsigned(sizeof(offsets)-1),length+unsigned(n));}
 writeLog("surface_source_session wallrun_session=%llu wallrun_this_accessed_offsets=%s offset_list_scope=selected_statically_dereferenced_pointer_fields candidate_child_pointer_count=%u face_candidate_group=unassigned_until_crawl",static_cast<unsigned long long>(id),count?offsets:"none",count);
 for(unsigned i=0;i<count;++i){auto& c=channels[i];c.child=0;c.objectId=0;c.data={};loggingChannel=int(i);c.tracker.begin(id,h,t,e,childLog);}loggingChannel=-1;
}
inline void observe(std::uintptr_t owner,const Snapshot& snapshot,const ws::diag::Motion& motion,std::uint64_t now){
 if(!runtimeObservationEnabled||!enabled||!session){return;}
 ws::Vec3 position{snapshot.position[0],snapshot.position[1],snapshot.position[2]};
 if(fieldPlanEnabled){fieldObserver.motion(motion.velocity,motion.valid);for(unsigned i=0;i<ws::fields::roots;++i){ws::Vec3 value;auto off=ws::fields::offset(i);bool ok=owner<=UINTPTR_MAX-off&&readable(owner+off,&value,sizeof(value),false);fieldObserver.observe(i,ok?&value:nullptr,position,now,0,writeLog);}}
 if(fieldPlanEnabled)fieldObserver.endBatch(position,motion.velocity,motion.valid,now,writeLog);
 unsigned valid=0;bool focusObserved=false;
 for(unsigned i=0;i<count;++i){auto& c=channels[i];std::uintptr_t pointer=0,confirm=0;auto off=c.evidence.offset;
  if(owner>UINTPTR_MAX-off||!readable(owner+off,&pointer,8,false)||pointer<0x10000||pointer>=0x0000800000000000ull||pointer%8||pointer==owner||(pointer>=owner&&pointer-owner<0x480))continue;
  unsigned char copy[0x300]{};unsigned copyBytes=fieldPlanEnabled&&off==0x128?0x300:0x200;if(!readable(pointer,copy,copyBytes,true)||!readable(owner+off,&confirm,8,false)||confirm!=pointer)continue;
  loggingChannel=int(i);
  if(c.child!=pointer){if(c.child){c.tracker.reset("child_pointer_changed_mid_session",childLog);c.tracker.begin(session,hero,transform,epoch,childLog);c.data={};}
   bool persistent=c.previousSession&&c.previousSession!=session&&c.previousChild==pointer;
   c.child=pointer;c.objectId=++nextId;
   childLog("child_lifetime session=%llu previous_session=%llu persistent_child_across_wallrun_sessions=%u contact_instance_likelihood_penalty=%u dynamic_internal_contact_NOT_excluded=1",static_cast<unsigned long long>(session),static_cast<unsigned long long>(c.previousSession),unsigned(persistent),unsigned(persistent));c.previousChild=pointer;c.previousSession=session;childLog("child_object_observation session=%llu parent=0x%llx parent_field=0x%x child_bytes=0x%x depth=1 native_dereference_evidence=1 source_type=UNVERIFIED lifetime=worker_sample_NOT_owned",static_cast<unsigned long long>(session),static_cast<unsigned long long>(owner),off,copyBytes);
  }
  if(fieldPlanEnabled&&off==0x128){focusObserved=true;for(unsigned j=0;j<3;++j){ws::Vec3 value;std::memcpy(&value,copy+ws::fields::childOffsets[j],sizeof(value));fieldObserver.observe(ws::fields::roots+j,&value,position,now,pointer,writeLog);}}
  c.data.observe(copy,ws::diag::actorAxes(snapshot.prefix),motion.velocity,motion.valid,&position,now,off==0x128?0x14:~0u);++valid;
 }
 loggingChannel=-1;
 if(fieldPlanEnabled&&!focusObserved){for(unsigned j=0;j<3;++j)fieldObserver.observe(ws::fields::roots+j,nullptr,position,now,0,writeLog);}
 // Failure/success count is bounded and summarized at end; no per-frame pointer dumps.
 (void)valid;
}
inline void finish(std::uint64_t id,double deinit){
 if(!runtimeObservationEnabled||!enabled||id!=session)return;
 if(fieldPlanEnabled)fieldObserver.finish(id,deinit,writeLog);
 for(unsigned i=0;i<count;++i){auto& c=channels[i];loggingChannel=int(i);childLog("child_source_session session=%llu candidate_child_pointer_count=%u observed_samples=%u depth=1",static_cast<unsigned long long>(id),count,c.data.count());if(c.data.count()){c.tracker.summarize(c.data,id,"verified_deinit",childLog);c.tracker.finish(c.data,id,deinit,childLog);}c.data={};}
 loggingChannel=-1;session=0;
}
inline void tick(double now,std::uint64_t ms,const Snapshot& snapshot,const ws::diag::Motion& motion,bool active){
 if(!runtimeObservationEnabled||!enabled){return;}
 ws::Vec3 p{snapshot.position[0],snapshot.position[1],snapshot.position[2]};auto axes=ws::diag::actorAxes(snapshot.prefix);
 if(fieldPlanEnabled)fieldObserver.tick(now,ms,snapshot.hero,snapshot.transform,motion.epoch,axes,active,writeLog);
 for(unsigned i=0;i<count;++i){loggingChannel=int(i);channels[i].tracker.tick(now,ms,snapshot.hero,snapshot.transform,motion.epoch,axes,active,childLog,&p);}loggingChannel=-1;
}
}
