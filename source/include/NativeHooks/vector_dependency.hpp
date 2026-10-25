#pragma once
#include <array>
namespace ws::rdx {
// Dependency categories only; never vector bits, lane values or pointers.
struct ReadVectorTags {
  std::array<unsigned,16> x{}; // XMM alias / low128.
  std::array<unsigned,16> high=[] {std::array<unsigned,16> a{};a.fill(7);return a;}();
  std::array<bool,16> above256Zero{};
  bool vectorTracked{};
  unsigned xmmRead(unsigned bank) const {return bank<16?x[bank]:7;}
  unsigned ymmRead(unsigned bank) const {return bank<16?(x[bank]|high[bank]):7;}
  bool legacyXmmWrite(unsigned bank,unsigned tags) {
    if (bank>=16 || (tags&~7u)) return false;
    x[bank]=tags;return true; // All upper state preserved by legacy SSE.
  }
  void pinnedYmm0Unknown() {
    x[0]=high[0]=7;above256Zero[0]=true;vectorTracked=true;
  }
  // ZMM/EVEX operations remain unsupported even when an above256 zero is known.
};
}
