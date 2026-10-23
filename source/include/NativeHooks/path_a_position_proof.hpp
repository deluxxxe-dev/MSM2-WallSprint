#pragma once
#include <array>
#include <cstddef>
namespace ws::position_proof {
// c6 intentionally has no Position write permission. Decoded accesses are
// candidates; a whole-path/effect/W proof is required before any later writer.
struct Access {unsigned offset{},width{};long long displacement{},sourceOffset{-1};bool write{},target{},directCandidate{},afterControlFlow{};};
struct Control {unsigned offset{};long long targetRva{-1};bool call{},direct{};};
struct Result {
 bool admitted{},writerDecoded{},callerDecoded{},callVerified{},stackRelation{};
 unsigned writerBytes{},callerBytes{},branches{},calls{},sourceReads{},targetStores{},directMask{},accessCount{},suppressed{},failureOffset{};
 const char* reason="original_function_identity_unavailable";std::array<Access,24> accesses{};
 unsigned controlCount{},suppressedControls{};std::array<Control,16> controls{};
};
Result inspect(const unsigned char*,std::size_t,const unsigned char*,std::size_t) noexcept;
}
namespace ws::position_proof {
struct Causal {bool complete{},directRowOnly{};unsigned decoded{},firstUnsupported{};const char* reason="original_identity_unavailable";};
// Strong but deliberately narrow whole-function certificate: one direct Vec4
// row copy, no other effects/control flow. Everything unsupported is rejected.
Causal causal(const unsigned char*,std::size_t,const unsigned char*,std::size_t) noexcept;
struct Store {bool decoded{},matched{};unsigned instruction{},width{};long long displacement{};unsigned baseRegister{},sourceRegister{};unsigned calls{},branches{};};
Store positionStore(const unsigned char*,std::size_t,unsigned afterOffset) noexcept;
inline bool causalMatch(const Causal& c,bool caller,bool stack,bool identity,bool allFourEqual,bool wObserved,bool overwrite){return c.directRowOnly&&caller&&stack&&identity&&allFourEqual&&wObserved&&!overwrite;}
}
