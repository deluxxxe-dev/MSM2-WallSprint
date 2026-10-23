#pragma once
#include "Math/source_orientation.hpp"
#include "Math/compact_basis.hpp"
#include <cstdint>
namespace ws::interop {
struct StackRange {std::uintptr_t limit{},base{};};
StackRange currentStack() noexcept;
bool stackLocation(std::uintptr_t source,std::uintptr_t originalRsp,StackRange) noexcept;
bool stackMatrix(std::uintptr_t source,std::uintptr_t originalRsp,StackRange stack,source_math::Matrix& out) noexcept;
struct WriteResult {bool success{},positionUnchanged{},rolledBack{},rollbackFailed{};std::uint64_t commitQpc{};};
bool callerStackMatrix(std::uintptr_t source,std::uintptr_t originalRsp,StackRange stack,source_math::Matrix& out) noexcept;
bool stackCompact(std::uintptr_t,std::uintptr_t,StackRange,source_math::CompactBlock&) noexcept;
WriteResult writeStackCompact(std::uintptr_t,const source_math::CompactBlock&,const source_math::CompactBlock&) noexcept;
WriteResult writeStackOrientation(std::uintptr_t source,const source_math::Matrix& expected,const source_math::Matrix& replacement) noexcept;
}
