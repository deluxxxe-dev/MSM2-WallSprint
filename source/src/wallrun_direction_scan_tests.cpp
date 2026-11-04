#include "wallrun_direction_scan.hpp"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>
static std::string out;
static void logf(const char* f, ...) { char b[512]; va_list v; va_start(v, f); std::vsnprintf(b, sizeof b, f, v); va_end(v); out += b; out += '\n'; }
static void put(unsigned char* m, unsigned off, float x, float y, float z) { float v[3]{x, y, z}; std::memcpy(m + off, v, sizeof v); }
int main() {
    ws::direction_scan::Scanner s; s.begin(7);
    unsigned char mem[ws::direction_scan::spanBytes]{};
    for (int k = 0; k < 40; ++k) {
        float a = k * 0.15f, dx = std::cos(a), dy = std::sin(a);
        put(mem, 0x1f0, dx, dy, 0);        // tracks travel
        put(mem, 0x300, -dx, -dy, 0);      // opposite
        put(mem, 0x100, 0, 0, 1);          // constant unit (normal-like): must be excluded
        put(mem, 0x400, dy, -dx, 0);       // perpendicular unit
        s.observe(mem, dx * 10, dy * 10, 0, true);
        s.observe(mem, 0, 0, 0, true);     // stationary: ignored
        s.observe(nullptr, 1, 0, 0, true); // missing bytes: ignored
    }
    s.finish(7, logf);
    assert(out.find("moving_samples=40") != std::string::npos);
    assert(out.find("sign=aligned rank=1 offset=0x1f0") != std::string::npos);
    assert(out.find("sign=opposite rank=1 offset=0x300") != std::string::npos);
    assert(out.find("offset=0x100 ") == std::string::npos);
    assert(out.find("offset=0x400 ") == std::string::npos);
    s.finish(7, logf); // second finish is a no-op
    std::puts("wallrun_direction_scan_tests: PASS");
}
