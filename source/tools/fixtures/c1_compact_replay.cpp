#include "NativeHooks/compact_map_proof.hpp"
#include <fstream>
#include <iterator>
#include <vector>
#include <cassert>
#include <cstdio>
namespace ws::compact_proof_c1 {ws::compact_proof::Result writer(const unsigned char*,std::size_t) noexcept;bool caller(const unsigned char*,std::size_t,std::uintptr_t,std::uintptr_t,std::uintptr_t) noexcept;}
int main(int argc,char** argv){assert(argc==3);std::ifstream wf(argv[1],std::ios::binary),cf(argv[2],std::ios::binary);std::vector<unsigned char>w(std::istreambuf_iterator<char>(wf),{}),c(std::istreambuf_iterator<char>(cf),{});assert(w.size()==444&&c.size()==128);auto r=ws::compact_proof_c1::writer(w.data(),w.size());auto caller=ws::compact_proof_c1::caller(c.data(),c.size(),0x2cfc7d0,0x2cfc3c0,0x2cfc84b);assert(!r.admitted&&r.decodedBytes==5&&r.pairs==0&&!caller);std::printf("PASS: exact published c1 decoder with actual authenticated bytes reproduces writer_admitted=0 decoded_prefix_bytes=%u pairs=%u caller_verified=0\n",r.decodedBytes,r.pairs);}
