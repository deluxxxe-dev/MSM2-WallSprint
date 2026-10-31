#include <cstdio>
#include "hde64.h"
int main(){
 const unsigned char a[]={0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x08};
 const unsigned char b[]={0x48,0x89,0x5c,0x24,0x08};
 hde64s h{};unsigned n=hde64_disasm(a,&h);
 if(n!=3||h.flags&F_ERROR)return 1;
 n+=hde64_disasm(a+n,&h);if(n!=7||h.flags&F_ERROR)return 2;
 if(hde64_disasm(b,&h)!=5||h.flags&F_ERROR)return 3;
 std::puts("PASS: vendored HDE64 MOV-only entry-prefix lengths request7 / owner5 (fixture only)");
}
