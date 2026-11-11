"""Compile the actual pure M11 helpers with owned geometry, never game memory."""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[1]
code = (root / 'src/NativeHooks/camera_ref.cpp').read_text()
axes = code.split('// M11_FACE_AXES_BEGIN:', 1)[1].split('\n', 1)[1].split('// M11_FACE_AXES_END', 1)[0]
tests = r'''
int main() {
  float n[3]={1,0,0}, h[3]={0,10,0}, c[3]={3,20,0}, p[3], r[3];
  FaceAxes a;
  assert(a.update(n,h,c,p,r,1)); assert(near(p[1],-1) && near(r[2],1));
  // Looking down the face: up/down map to world down/up, left/right to opposite wall-side axes.
  assert(near(p[0],0) && near(p[2],0) && near(r[1],0));
  c[1]=10; c[2]=-10; assert(a.update(n,h,c,p,r,1)); assert(near(p[2],1) && near(r[1],1));
  // Degenerate projection retains last valid direction; fresh state cannot invent one.
  c[2]=0; assert(a.update(n,h,c,p,r,1) && near(p[2],1));
  FaceAxes empty; assert(!empty.update(n,h,c,p,r,1));
  c[2]=NAN; assert(!a.update(n,h,c,p,r,1)); c[2]=-10;
  float positive[3]={0,0.02f,0}, negative[3]={0,-0.02f,0};
  for(int i=0;i<19;++i) assert(!a.observe(n,negative,1));
  assert(a.observe(n,negative,1) && a.sign==-1);
  assert(a.update(n,h,c,p,r,1) && near(r[1],-1));
  assert(!a.observe(n,positive,1) && a.sign==-1); // one decision per hold/manager
  FaceAxes mixed; assert(mixed.update(n,h,c,p,r,1));
  for(int i=0;i<4;++i) assert(!mixed.observe(n,negative,1));
  for(int i=0;i<15;++i) assert(!mixed.observe(n,positive,1));
  assert(mixed.observe(n,positive,1) && mixed.sign==1); // 16/20 = 80%
  FaceAxes abstain; assert(abstain.update(n,h,c,p,r,1));
  for(int i=0;i<25;++i) assert(!abstain.observe(n,positive,0.7f));
  assert(abstain.positive==0 && abstain.negative==0);
  // Opposite stick yields the same physical handedness.
  for(int i=0;i<19;++i) assert(!abstain.observe(n,negative,-1));
  assert(abstain.observe(n,negative,-1) && abstain.sign==1);
  puts("PASS: M11 screen axes/camera turn/degeneracy/handedness threshold");
}
'''
(root / 'build').mkdir(exist_ok=True)
unit = root / 'build/face_control_tests.cpp'
unit.write_text('#include <algorithm>\n#include <cmath>\n#include <cassert>\n#include <cstdio>\nbool near(float a,float b){return std::fabs(a-b)<1e-5f;}\n' + axes + tests)
subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', '-pedantic', '-fsanitize=address,undefined', '-g', str(unit), '-o', str(root/'build/face_control_tests')], check=True)
subprocess.run([str(root/'build/face_control_tests')], check=True)
