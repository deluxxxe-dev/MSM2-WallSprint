#pragma once
#include <cstdint>
#include <cstddef>
namespace msm2::reset_copy {
struct ProofStatus {bool writer{},caller{},map{};};
ProofStatus proofStatus() noexcept;
void configure(bool exactAndVerified,std::uintptr_t base,std::size_t size);
bool authenticated() noexcept;
bool compactProofAuthenticated() noexcept;
void stop();
}
