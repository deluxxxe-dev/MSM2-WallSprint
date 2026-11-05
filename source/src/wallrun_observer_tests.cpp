#include "wallrun_observer.hpp"
#include "object_candidates.hpp"
#include <cassert>
#include <iostream>
#include <limits>
int main(int argc,char**){
 ws::observed::Observer o;o.configure(false);
 assert(!o.event(0,100,0));assert(!o.context(0).wallRunActive);
 o.configure(true);
 for(unsigned k=1;k<4;++k){assert(!o.event(k,100,0));assert(!o.context(0).wallRunActive);}
 assert(o.event(0,100,10));assert(o.event(1,100,11));
 assert(o.event(2,100,710));auto c=o.context(752);
 assert(c.wallRunActive&&c.wallRunOwner==100&&c.wallRunUpdateCalls==1&&std::abs(c.elapsed-.742)<1e-8&&!c.surfaceValid);
 o.expire(2210);assert(o.context(2210).wallRunActive); // former 1.5s cutoff must not exit
 assert(o.event(3,100,2300));assert(!o.context(2400).wallRunActive);assert(std::string(o.exitReason())=="verified_deinit");
 // Tilting into crawl / moving does not create WallRun, nor do stray updates.
 assert(!o.event(2,100,3000));assert(!o.context(3000).wallRunActive);
 assert(o.event(0,100,4000));auto firstId=o.context(4000).wallRunSessionId;
 assert(!o.event(2,200,4010));assert(!o.context(4010).wallRunActive);assert(std::string(o.exitReason())=="candidate_owner_mismatch");
 assert(o.event(0,200,5000));assert(o.context(5000).wallRunSessionId>firstId);
 assert(!o.event(0,200,5001));assert(!o.context(5001).wallRunActive);
 assert(o.event(0,200,6000));o.expire(11000);assert(o.context(11000).wallRunActive);
 o.expire(11001);assert(!o.context(11001).wallRunActive);
 for(auto reason:{"Hero_or_Transform_changed","script_disable","hook_ownership_lost","hook_event_loss"}){
  assert(o.event(0,200,12000));o.reset(reason);assert(!o.context(12001).wallRunActive&&o.context(12001).wallRunOwner==0);
 }
 assert(o.event(0,200,13000));assert(!o.event(2,200,12999));assert(!o.context(13000).wallRunActive);
 assert(!o.event(0,0,14000));assert(!o.event(0,200,std::numeric_limits<double>::quiet_NaN()));
 assert(std::string(ws::observed::subtype({0,20,1},true))=="WallRunUp");
 assert(std::string(ws::observed::subtype({20,1,0},true))=="WallRunSide");
 assert(std::string(ws::observed::subtype({1,1,0},true))=="Mixed");
 unsigned char raw[ws::discovery::bytes]{};ws::Vec3 x{1,0,0};std::memcpy(raw+0x40,&x,sizeof(x));
 ws::diag::ActorAxes axes{{1,0,0},{0,1,0},{-1,0,0},true};ws::discovery::Candidates fields;
 fields.observe(raw,axes,{0,20,0},true);x={-1,0,0};std::memcpy(raw+0x40,&x,sizeof(x));fields.observe(raw,axes,{0,0,20},true);
 auto field=fields.fields()[0x40/4];assert(field.samples==2&&field.up==1&&field.side==1&&field.positive==1&&field.negative==1&&field.minStability==-1);
 auto invalid=std::numeric_limits<float>::infinity();std::memcpy(raw+0x40,&invalid,4);fields.observe(raw,axes,{0,20,0},true);assert(fields.fields()[0x40/4].samples==2);
 // Replay sparsely logged real-game events. Counts are thinned in original log.
 if(argc>1){o.configure(true);unsigned kind,session,updates=0,ends=0;double ms;std::uintptr_t owner;
  while(std::cin>>session>>kind>>owner>>ms){
   assert(o.event(kind,owner,ms));auto state=o.context(ms);
   if(kind==3){assert(!state.wallRunActive);++ends;}else {assert(state.wallRunActive);if(kind==2)++updates;}
  }
  assert(ends==6&&updates>10);
  std::cout<<"PASS: real v0.1a4 thinned event replay: six Init/Entry/Update/Deinit sessions (not reconstructed frame counts)\n";
 }
 { // stall longer than the timeout: same-owner Update resumes the same session; Init/Deinit/other owner do not
  ws::observed::Observer r;r.configure(true);assert(r.event(0,300,1000));auto id=r.context(1000).wallRunSessionId;assert(r.event(2,300,1100));
  r.expire(7000);assert(!r.context(7000).wallRunActive);assert(!r.event(2,301,7100));assert(!r.context(7100).wallRunActive);
  assert(r.event(0,300,9000));assert(r.context(9000).wallRunSessionId!=id); // Init after timeout = new session
  ws::observed::Observer q;q.configure(true);assert(q.event(0,400,1000));auto qid=q.context(1000).wallRunSessionId;q.expire(7000);
  assert(q.event(2,400,7200));assert(q.takeResumed()&&q.context(7200).wallRunActive&&q.context(7200).wallRunSessionId==qid&&q.context(7200).wallRunOwner==400);
  assert(!q.takeResumed());assert(q.event(3,400,7300));assert(!q.context(7300).wallRunActive);q.expire(20000);assert(!q.event(2,400,20100));}
 std::cout<<"PASS: build admission, owner/lifecycle/elapsed, short gap vs emergency timeout, reset, crawl isolation, bounded vector filters/sign correlation\n";
}
