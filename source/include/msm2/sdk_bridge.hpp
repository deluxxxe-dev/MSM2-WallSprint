#pragma once
#include <cstdint>
#include <cstddef>
#include <array>
namespace msm2 {
constexpr char templateCommit[]="999d0c7437720be5378049455209e69447f4a930";
struct Snapshot {std::uintptr_t hero{},transform{};float position[3]{},yaw{},prefix[12]{};};
struct HookCandidate {std::size_t matches{};std::uintptr_t address{};};
std::array<HookCandidate,4> hookCandidates();
bool initialize(bool allowGameCalls,bool auditReferencePatterns,bool resolveWallRun=false);
bool sample(Snapshot&);
void writeLog(const char* format,...);
bool readMemory(std::uintptr_t address,void* output,std::size_t size);
}
