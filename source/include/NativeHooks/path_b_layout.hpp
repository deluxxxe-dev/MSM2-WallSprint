#pragma once
#include <cstdint>
#include "wallsprint.hpp"
namespace msm2::path_b {
struct Entry {
 std::uint64_t id{},session{},generation{},update{},qpc{};
 std::uintptr_t owner{},rcx{},rdx{},r8{},r9{},rsp{},stackLimit{},stackBase{},caller{};
 unsigned thread{};float elapsed{};ws::Vec3 normal{},actorBefore{},pathAUp{};
 bool readable{},inside{},spanInside{},relation{},referenceValid{};unsigned char raw[128]{};
};
bool readBounded(std::uintptr_t,void*,unsigned) noexcept;
void configure(bool);
void capture(Entry) noexcept;
void drainLogs();
void kill() noexcept;
}
