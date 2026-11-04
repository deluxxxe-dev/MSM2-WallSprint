#pragma once
// M3b (2026-11-05): read-only search for a WallRun.this float3 that tracks the actual travel direction.
// Worker-side only (100 ms cadence), owner bytes are copied by the caller with guarded reads; nothing is written.
// Output is a bounded per-session ranking; it is OBSERVED-CORRELATION evidence, not field semantics.
#include "surface_calibration.hpp"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace ws::direction_scan {

inline constexpr unsigned spanBytes = 0x800;
inline constexpr unsigned slots = (spanBytes - 12) / 4 + 1;
inline constexpr float minSpeed = 3.0f;
inline constexpr unsigned minSamples = 10;
inline constexpr unsigned topCount = 8;

struct Slot {
    unsigned unitSamples{}, changes{};
    double sumDot{};
    float minDot{2}, maxDot{-2};
    float last[3]{};
    bool haveLast{};
};

class Scanner {
    std::array<Slot, slots> s{};
    std::uint64_t session{};
    unsigned movingSamples{};
    float firstDir[3]{};
    bool haveFirst{};
    float spreadMinDot{1};
public:
    void begin(std::uint64_t id) { s = {}; session = id; movingSamples = 0; haveFirst = false; spreadMinDot = 1; }
    void reset() { session = 0; }

    // bytes: copy of owner+[0,spanBytes); velocity: worker finite-difference travel estimate.
    void observe(const unsigned char* bytes, float vx, float vy, float vz, bool valid) {
        if (!session || !bytes || !valid) return;
        const float speed = std::sqrt(vx * vx + vy * vy + vz * vz);
        if (!std::isfinite(speed) || speed < minSpeed) return;
        const float u[3]{vx / speed, vy / speed, vz / speed};
        ++movingSamples;
        if (!haveFirst) { std::memcpy(firstDir, u, sizeof u); haveFirst = true; }
        const float spread = u[0] * firstDir[0] + u[1] * firstDir[1] + u[2] * firstDir[2];
        if (spread < spreadMinDot) spreadMinDot = spread;
        for (unsigned i = 0; i < slots; ++i) {
            float v[3];
            std::memcpy(v, bytes + 4 * i, sizeof v);
            if (!std::isfinite(v[0]) || !std::isfinite(v[1]) || !std::isfinite(v[2])) continue;
            const float n = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            auto& slot = s[i];
            if (slot.haveLast && std::memcmp(slot.last, v, sizeof v) != 0) ++slot.changes;
            std::memcpy(slot.last, v, sizeof v);
            slot.haveLast = true;
            if (n < 0.95f || n > 1.05f) continue;
            const float d = (v[0] * u[0] + v[1] * u[1] + v[2] * u[2]) / n;
            ++slot.unitSamples;
            slot.sumDot += d;
            if (d < slot.minDot) slot.minDot = d;
            if (d > slot.maxDot) slot.maxDot = d;
        }
    }

    void finish(std::uint64_t id, calibration::Logger log) {
        if (!session || id != session) return;
        log("wallrun_direction_scan_summary session=%llu moving_samples=%u travel_direction_spread_min_dot=%.4f span_bytes=0x%x min_speed=%.1f game_write=0 semantics=OBSERVED_CORRELATION_ONLY",
            static_cast<unsigned long long>(id), movingSamples, spreadMinDot, spanBytes, minSpeed);
        for (int sign : {1, -1}) {
            bool used[slots]{};
            for (unsigned rank = 0; rank < topCount; ++rank) {
                int best = -1;
                double bestMean = 0;
                for (unsigned i = 0; i < slots; ++i) {
                    const auto& slot = s[i];
                    if (used[i] || slot.unitSamples < minSamples || slot.changes == 0) continue;
                    const double mean = sign * slot.sumDot / slot.unitSamples;
                    if (best < 0 || mean > bestMean) { best = int(i); bestMean = mean; }
                }
                if (best < 0 || bestMean < 0.5) break;
                used[best] = true;
                const auto& slot = s[unsigned(best)];
                log("wallrun_direction_scan_candidate session=%llu sign=%s rank=%u offset=0x%x unit_samples=%u changes=%u mean_dot=%.4f min_dot=%.4f max_dot=%.4f last=(%.6g,%.6g,%.6g)",
                    static_cast<unsigned long long>(id), sign > 0 ? "aligned" : "opposite", rank + 1, unsigned(best) * 4,
                    slot.unitSamples, slot.changes, slot.sumDot / slot.unitSamples, slot.minDot, slot.maxDot,
                    slot.last[0], slot.last[1], slot.last[2]);
            }
        }
        session = 0;
    }
};

} // namespace ws::direction_scan
