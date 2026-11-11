"""Compile actual M14 read-only state helpers with owned labeled objects."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
s=(root/'src/NativeHooks/camera_ref.cpp').read_text()
clock=s.split('// M14_FRAME_CLOCK_BEGIN:',1)[1].split('\n',1)[1].split('// M14_FRAME_CLOCK_END',1)[0]
core=s.split('// M14_STATE_CORE_BEGIN:',1)[1].split('\n',1)[1].split('// M14_STATE_CORE_END',1)[0]
tests=r'''
static StateSnapshotRing ring;
void add(StateLabel label){
 auto& s=ring.reserve();std::memset(s.hero,0,sizeof s.hero);std::memset(s.transform,0,sizeof s.transform);s.heroBytes=4096;s.transformBytes=256;s.label=label;
 bool air=label==StateLabel::J||label==StateLabel::A;std::uint32_t flag=air ? 2 : 1,same=3;float f=air||label==StateLabel::H ? 2.f:1.f;
 std::uint64_t ptr=air ? 0x12346000 : 0x12345000;
 std::memcpy(s.hero,&flag,4);std::memcpy(s.hero+4,&same,4);std::memcpy(s.hero+8,&f,4);std::memcpy(s.hero+16,&ptr,8);
 float row=label==StateLabel::J ? 3.f:1.f;for(int k=0;k<64;++k)std::memcpy(s.transform+4*k,&row,4);
}
int main(){
 StateFrameSampler sampler;float p[3]={0,0,0};StateContext ground{false,false,false,false,true};sampler.call(10,p,ground);
 for(int i=1;i<=10;++i)sampler.call(10+i*0.016,p,ground);
 assert(sampler.metrics.quietY==10);assert(chooseStateLabel(false,false,false,0,0,true,0,sampler.metrics)==StateLabel::G);
 assert(chooseStateLabel(false,false,false,0,0,true,-4,sampler.metrics)==StateLabel::A);
 assert(chooseStateLabel(false,false,false,0,0,false,-4,sampler.metrics)==StateLabel::Other);
 StateContext pushed{true,true,false,true,false};sampler.call(10.176,p,pushed);
 for(int i=1;i<=10;++i)sampler.call(10.176+i*0.016,p,pushed);
 assert(sampler.metrics.zeroPushed==10);assert(chooseStateLabel(true,true,false,1,0,false,0,sampler.metrics)==StateLabel::H);
 p[1]=0.08f;sampler.call(10.352,p,pushed);assert(chooseStateLabel(true,true,false,1,0,false,0,sampler.metrics)==StateLabel::W);
 assert(chooseStateLabel(true,true,false,0.1f,0,false,0,sampler.metrics)==StateLabel::F);
 assert(chooseStateLabel(true,true,true,1,0.2f,false,0,sampler.metrics)==StateLabel::J);
 assert(chooseStateLabel(true,true,true,1,0.4f,false,0,sampler.metrics)==StateLabel::Other);
 unsigned reads=0;unsigned char bytes[0x400]{};
 auto reader=[&](std::uintptr_t a,unsigned char*,unsigned n){++reads;assert(n==0x100);return a<0x1200;};
 assert(readStatePrefix(0x1000,bytes,0x400,reader)==0x200 && reads==3);
 reads=0;assert(readStatePrefix(UINTPTR_MAX-0x80,bytes,0x100,reader)==0 && reads==0);
 StateCadence cadence;assert(cadence.due(1000));assert(!cadence.due(1100));assert(!cadence.due(1200));assert(cadence.due(1300));assert(cadence.due(1500));assert(!cadence.due(1500));assert(cadence.due(10000));assert(!cadence.due(10001));
 puts("PASS: M14 first-unreadable256 block/overflow refusal,250ms no-duplicate/no-catchup cadence");
 puts("PASS: M14 F/W/J/H/G/A labels, native ten-frame counters and vertical/air window refusals");
 sampler.reset();p[1]=0;sampler.call(20,p,pushed);float wanted[3]={0,0.04f,0},actual[3]={0,0.07f,0};sampler.applied(wanted,actual);
 assert(!sampler.clock.step(20.0021,true));p[1]=0.09f;sampler.call(20.016,p,pushed);assert(near(sampler.metrics.length,0.06f));
 for(StateLabel label:{StateLabel::F,StateLabel::W,StateLabel::J,StateLabel::H,StateLabel::G,StateLabel::A})for(int i=0;i<4;++i)add(label);
 assert(ring.coverage());auto candidates=ring.candidates();assert(candidates.size()<=64);
 bool flag=false,floating=false,pointer=false;
 for(auto c:candidates){if(c.offset==0&&c.view==StateView::Byte)flag=true;if(c.offset==8&&c.view==StateView::Float)floating=true;if(c.offset==16&&c.view==StateView::Pointer)pointer=true;assert(c.offset!=4);}
 assert(flag&&floating&&pointer);StateSeparation sets;assert(ring.separate(0,StateView::Byte,sets));assert(sets.ground.contains(1)&&sets.air.contains(2)&&sets.h.contains(1));
 double means[4];unsigned n[4];ring.transformMeans(0,StateLabel::J,means,n);assert(means[0]==3&&n[0]==4);ring.transformMeans(0,StateLabel::W,means,n);assert(means[0]==1&&n[0]==4);
 ring.snapshots[0].heroBytes=16;assert(!ring.separate(16,StateView::Pointer,sets)); // missing any required-label byte refuses field
 ring.reset();for(int i=0;i<3;++i)add(StateLabel::F);assert(!ring.coverage()&&ring.candidates().empty());
 ring.reset();for(int i=0;i<300;++i)add(StateLabel::Other);assert(ring.used==240);
 puts("PASS: M14 disjoint flag/float/pointer-like candidates, H sets, overlap/partial/coverage refusal, means and240 ring cap");
}
'''
(root/'build').mkdir(exist_ok=True)
p=root/'build/state_snapshot_tests.cpp';p.write_text('#include <algorithm>\n#include <cmath>\n#include <cstdint>\n#include <cstring>\n#include <cstdio>\n#include <cassert>\n#include <vector>\n#include <initializer_list>\nbool near(float a,float b){return std::fabs(a-b)<1e-5f;}\n'+clock+core+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(root/'build/state_snapshot_tests')],check=True)
subprocess.run([str(root/'build/state_snapshot_tests')],check=True)
