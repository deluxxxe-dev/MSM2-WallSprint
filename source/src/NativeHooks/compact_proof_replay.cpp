// Private inputs are read locally and never embedded in this tool or its logs.
#include "NativeHooks/compact_map_proof.hpp"
#include <capstone/capstone.h>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <vector>
using Bytes=std::vector<unsigned char>;
static Bytes read(const char* name){std::ifstream f(name,std::ios::binary);assert(f);return Bytes(std::istreambuf_iterator<char>(f),{});}
static ws::compact_proof::Result writer(const Bytes& b){return ws::compact_proof::writer(b.data(),b.size());}
static ws::compact_proof::Result caller(const Bytes& b,std::uintptr_t ret=0x2cfc84b){return ws::compact_proof::callerProof(b.data(),b.size(),0x2cfc7d0,0x2cfc3c0,ret);}
int main(int argc,char** argv){
 if(argc!=3){std::puts("usage: compact_proof_replay.exe private-writer.bin private-caller.bin");return 2;}
 auto w=read(argv[1]),c=read(argv[2]);assert(w.size()==444&&c.size()==128);
 auto wp=writer(w),cp=caller(c);
 std::printf("PRIVATE_PROOF_REPLAY writer_admitted=%u writer_decoded=%u writer_instructions=%u caller_admitted=%u caller_decoded=%u caller_instructions=%u pairs=%u reason_writer=%s reason_caller=%s\n",unsigned(wp.admitted),wp.decodedBytes,wp.decodedInstructions,unsigned(cp.admitted),cp.decodedBytes,cp.decodedInstructions,wp.pairs,wp.reason,cp.reason);
 assert(wp.admitted&&wp.decodedBytes==444&&wp.pairs==9&&cp.admitted&&cp.decodedBytes==128&&cp.pairs==9);
 csh handle{};assert(cs_open(CS_ARCH_X86,CS_MODE_64,&handle)==CS_ERR_OK);cs_insn* ins{};auto count=cs_disasm(handle,w.data(),w.size(),0,0,&ins);assert(count);
 for(unsigned i=0;i<9;++i){auto& p=wp.map[i];assert(p.source==i*4&&p.destination==(i/3)*16+(i%3)*4);std::printf("RECOVERED_PAIR source=0x%x destination=0x%x store_offset=0x%x\n",p.source,p.destination,p.instruction);}
 auto m=w;unsigned second=wp.map[1].instruction,load=0,storeSize=0;
 for(std::size_t i=1;i<count;++i)if(ins[i].address==second){load=unsigned(ins[i-1].address);storeSize=ins[i].size;}
 assert(storeSize==3&&second-load==3);
 for(unsigned i=0;i<storeSize;++i)m[second+i]=0x90;
 assert(!writer(m).admitted); // Missing pair, without invalid instructions.
 m=w;m[load+2]=0;m[second+2]=0;assert(!writer(m).admitted); // Duplicate pair.
 m=w;m[load+2]=0x24;assert(!writer(m).admitted); // Outside source payload.
 m=w;m[second+2]=0x30;assert(!writer(m).admitted); // Outside destination.
 m=w;m.back()=0x0f;auto invalid=writer(m);assert(!invalid.admitted&&invalid.failureOffset==443&&invalid.decodedBytes==443);
 auto middle=unsigned(ins[count/2].address);m=w;m[middle]=0x06;auto midFailure=writer(m);assert(!midFailure.admitted&&midFailure.failureOffset==middle&&midFailure.decodedBytes==middle);
 auto badCaller=c;badCaller.back()=0x0f;assert(!caller(badCaller).admitted);
 badCaller=c;badCaller[0x38]=0x06;assert(!caller(badCaller).admitted); // Midstream invalid x64 opcode.
 badCaller=c;badCaller[0x1c]=0x10;assert(!caller(badCaller).admitted); // VMOVSS read cannot initialize local.
 badCaller=c;badCaller[6]=0x60;assert(!caller(badCaller).admitted); // Stack relation.
 assert(!caller(c,0x2cfc84a).admitted);
 badCaller=c;badCaller[0x77]^=1;assert(!caller(badCaller).admitted); // Wrong direct target.
 cs_free(ins,count);cs_close(&handle);
 std::puts("PASS: actual accepted-build writer444/caller128 complete proof, nine recovered pairs, missing/duplicate/outside source/outside destination/truncated+midstream writer+caller/wrong return/wrong call/stack-relation/read-versus-store negatives; no game execution");
}
