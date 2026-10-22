#include "pattern.hpp"
#include <cassert>
#include <iostream>
int main(){
 const std::uint8_t b[]={0x48,0x89,0x05,0xff,0x48,0x89,0x05,0x11};ws::Pattern p("48 89 05 **");ws::Matches m;
 ws::scan(b,3,0x1000,p,m);assert(m.count==0);
 ws::scan(b,4,0x1000,p,m);assert(m.count==1&&m.only==0x1000);
 ws::scan(b+4,4,0x2000,p,m);assert(m.count==2&&m.only==0);
 ws::Matches end;ws::scan(b,8,0x1000,ws::Pattern("05 11"),end);assert(end.count==1&&end.only==0x1006);
 for(auto text:{"","G0","?","123","AA ?B"}){bool threw=false;try{ws::Pattern bad(text);}catch(const std::invalid_argument&){threw=true;}assert(threw);}
 std::cout<<"PASS: short buffers, unique/ambiguous AOB, final byte, invalid patterns\n";
}
