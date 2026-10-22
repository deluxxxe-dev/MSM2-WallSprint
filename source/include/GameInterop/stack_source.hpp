#pragma once
#include "Math/source_orientation.hpp"
#include <cstdint>
namespace ws::interop {
struct StackRange {std::uintptr_t limit{},base{};};
StackRange currentStack() noexcept;
bool stackMatrix(std::uintptr_t source,std::uintptr_t originalRsp,StackRange stack,source_math::Matrix& out) noexcept;
struct WriteResult {bool success{},positionUnchanged{},rolledBack{},rollbackFailed{};std::uint64_t commitQpc{};};
WriteResult writeStackOrientation(std::uintptr_t source,const source_math::Matrix& expected,const source_math::Matrix& replacement) noexcept;
}
