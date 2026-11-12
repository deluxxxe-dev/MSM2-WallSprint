"""Actual bounded position mirror query/capture against owned labeled memory."""
from pathlib import Path
import subprocess
r=Path(__file__).resolve().parents[1];s=(r/'src/NativeHooks/camera_ref.cpp').read_text();core=''
for tag in ['M14_FRAME_CLOCK','M14_STATE_CORE']:core+=s.split('// '+tag+'_BEGIN:',1)[1].split('\n',1)[1].split('// '+tag+'_END',1)[0]
tests=r'''
static StateSnapshotRing ring;static StateSnap captured;
void add(StateLabel label,int i){auto& s=ring.reserve(label);std::memset(s.hero,0,sizeof s.hero);s.heroBytes=4096;s.transformBytes=256;s.positionValid=true;
 s.position[0]=label==StateLabel::J ? 110:label==StateLabel::L ? 120:100.f+i;s.position[1]=200;s.position[2]=300;
 std::memcpy(s.hero+0x600,s.position,12);s.childCount=1;auto& child=s.children[0];child.address=0x20000;child.heroOffset=0x20;child.readable=512;std::memset(child.bytes,0,512);
 float v[3]={label==StateLabel::J||label==StateLabel::L ? 100.f:s.position[0],200,300};std::memcpy(child.bytes+0x40,v,12);
}
bool childFound(){for(const auto& c:ring.positionMirrors())if(c.heroOffset==0x20&&c.offset==0x40)return true;return false;}
int main(){
 for(int i=0;i<10;++i){add(StateLabel::G,i);add(StateLabel::W,i);}for(int i=0;i<4;++i){add(StateLabel::J,i);add(StateLabel::L,i);}
 auto candidates=ring.positionMirrors();assert(!candidates.empty()&&candidates.size()<=32);bool child=false,root=false;
 for(auto c:candidates){if(c.heroOffset==0x20&&c.offset==0x40){child=true;assert(c.matchG==10&&c.matchW==10&&c.nL==4&&c.nJ==4&&c.meanL==20&&c.meanJ==10);}if(c.heroOffset==0xffffffffu&&c.offset==0x600){root=true;assert(c.meanL==0&&c.meanJ==0);}}
 assert(child&&root);float nan=NAN;std::memcpy(ring.snapshots[60].children[0].bytes+0x40,&nan,4);assert(childFound());std::memcpy(ring.snapshots[61].children[0].bytes+0x40,&nan,4);assert(!childFound()); //90percent, not missing-data exclusion
 for(int i=0;i<2;++i)std::memcpy(ring.snapshots[60+i].children[0].bytes+0x40,ring.snapshots[60+i].position,12);
 assert(childFound());
 for(int i=0;i<4;++i)ring.snapshots[360+i].children[0].address=0x30000;
 for(auto c:ring.positionMirrors())if(c.heroOffset==0x20)assert(c.nL==0); // distinct object identity cannot fabricate lag
 ring.snapshots[240].positionValid=ring.snapshots[241].positionValid=false;assert(ring.positionMirrors().empty());
 ring.reset();assert(ring.positionMirrors().empty());
 puts("PASS: M17 root/child mirrors require90percent G/W including bad data, report L/J distances, bind identities and cap32");
 captured.heroBytes=4096;captured.transformBytes=256;float position[3]={100,200,300};std::memcpy(captured.transform+0x30,position,12);
 for(unsigned i=0;i<128;++i){std::uintptr_t ptr=0x10000+i*0x1000;std::memcpy(captured.hero+i*8,&ptr,8);}
 std::memcpy(captured.hero+8,captured.hero,8);std::uintptr_t invalid=UINTPTR_MAX;std::memcpy(captured.hero+16,&invalid,8);
 unsigned reads=0;auto reader=[&](std::uintptr_t a,unsigned char* out,unsigned bytes){++reads;assert(a>=0x10000&&a<0x90000&&bytes==256);if(a==0x13000)return false;std::memset(out,0,bytes);std::uintptr_t deeper=0xfeed0000;std::memcpy(out,&deeper,8);return true;};
 capturePositionObjects(captured,0x90000,reader);assert(captured.positionValid&&captured.childAttempts==64&&captured.childCount==63&&reads==127);assert(captured.children[0].heroOffset==0);
 captured.transformBytes=32;reads=0;capturePositionObjects(captured,0x90000,reader);assert(!captured.positionValid&&reads==0&&captured.childCount==0);
 puts("PASS: M17 guarded one-hop capture, duplicate/noncanonical refusal,64 attempted objects/512bytes, unreadable block stop and no recursive chase");
}
'''
(r/'build').mkdir(exist_ok=True);p=r/'build/position_mirror_tests.cpp';p.write_text('#include <algorithm>\n#include <map>\n#include <cmath>\n#include <cstdint>\n#include <cstring>\n#include <cassert>\n#include <cstdio>\n#include <vector>\n#include <initializer_list>\n'+core+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(r/'build/position_mirror_tests')],check=True)
subprocess.run([str(r/'build/position_mirror_tests')],check=True)
