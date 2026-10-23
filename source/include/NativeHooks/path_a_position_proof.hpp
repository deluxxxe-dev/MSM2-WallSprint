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
