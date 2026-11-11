"""Owned verified-row math, handedness, untouched W and bounded phase policy."""
from pathlib import Path
import subprocess,configparser
r=Path(__file__).resolve().parents[1];s=(r/'src/NativeHooks/camera_ref.cpp').read_text()
core=s.split('// M16_AIR_FACING_BEGIN:',1)[1].split('\n',1)[1].split('// M16_AIR_FACING_END',1)[0]
ini=configparser.ConfigParser();ini.read(r/'mod/WallSprint.ini');assert ini.getint('WallSprint','WALLSPRINT_WS_FACE_AIR_FACING')==1
assert 'wsFaceAirFacing{false}' in (r/'include/msm2/drive_command.hpp').read_text()
assert 'L"WALLSPRINT_WS_FACE_AIR_FACING",0,' in (r/'src/msm2/telemetry.cpp').read_text()
tests=r'''
int main(){
 float rows[12]={1,0,0,77,0,1,0,88,0,0,1,99},out[12],n[3]={0,1,0},f[3]={1,0,0};
 FaceAirFacing facing;assert(facing.begin(f,n));assert(!facing.prepare(rows,out,false,true));assert(!facing.prepare(rows,out,true,false));
 assert(facing.prepare(rows,out,true,true));assert(out[0]==0&&out[2]==-1&&out[4]==0&&out[5]==1&&out[8]==1);
 for(int k:{3,7,11})assert(std::memcmp(out+k,rows+k,4)==0);
 facing.committed();assert(facing.writes==1&&facing.holdWrites==1);
 assert(facing.prepare(rows,out,true,true));assert(facing.overwritten==1&&facing.maxDev>89);facing.committed();
 assert(!facing.prepare(out,rows,true,true)); // target retained, no gratuitous write
 float mirrored[12]={-1,0,0,11,0,1,0,22,0,0,1,33};FaceAirFacing left;assert(left.begin(f,n));assert(left.prepare(mirrored,out,true,true));assert(left.sign==-1&&out[2]==1);
 assert(!left.prepare(rows,out,true,true)); // changed sign never borrows initial handedness
 FaceAirFacing cap;assert(cap.begin(f,n));float original[12]={1,0,0,77,0,1,0,88,0,0,1,99};
 for(int i=0;i<4000;++i){assert(cap.prepare(original,out,true,true));cap.committed();}
 assert(!cap.prepare(original,out,true,true)&&cap.holdWrites==4000);assert(cap.begin(f,n));assert(!cap.prepare(original,out,true,true)); // cap survives per-jump reset
 float bad[12];std::memcpy(bad,original,48);bad[5]=0;assert(!facing.prepare(bad,out,true,true));bad[5]=NAN;assert(!facing.prepare(bad,out,true,true));
 float zero[3]{};assert(!facing.begin(zero,n));assert(!facing.prepare(original,out,true,true));
 puts("PASS: M16 air-facing coherent rows and both signs, exact W preservation, deviation/overwrite counts and4000-per-hold cap");
 puts("PASS: M16 facing no phase/disabled/invalid rows/sign/direction refusal and code-default0/shipped1 contract");
}
'''
(r/'build').mkdir(exist_ok=True);p=r/'build/face_air_facing_tests.cpp';p.write_text('#include <cmath>\n#include <cstring>\n#include <cassert>\n#include <cstdio>\n#include <initializer_list>\n'+core+tests)
subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','-fsanitize=address,undefined',str(p),'-o',str(r/'build/face_air_facing_tests')],check=True)
subprocess.run([str(r/'build/face_air_facing_tests')],check=True)
