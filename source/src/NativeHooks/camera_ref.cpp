// M5b (2026-11-05). Independent, exact-build implementation; the reference mod was used only to learn which vanilla
// manager branch exists (analysis/M5_CAMERA_20261105.md). Identity: pinned EXE SHA (checked by caller) + .pdata begin +
// SHA256 of the first 64 entry bytes taken from the owner's current EXE.
//   HeroCameraManager+0x304 (byte): vanilla "actor-relative reference" branch selector (VERIFIED-STATIC decode).
// Behaviour: while WallSprint is active, +0x304 is set to 1 (vanilla then blends the follow-camera reference toward the
// actor orientation, which WS already sets to the wall). 300 ms after the last WS Update the original byte is restored.
// A read-only scan also looks for a camera-forward vector in the manager (ground, stick held forward) for M5c.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <MinHook.h>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "NativeHooks/camera_ref.hpp"
extern "C" { void ws_camref_thunk(); void* ws_camref_original{}; extern std::uintptr_t ws_camref_this; extern std::atomic<void(*)()> ws_wallsprint_update_listener;
// M5c: +1/-1 once ground samples prove manager+0x1E0 is the camera forward and give the right = sign*cross(forward, up) handedness.
extern std::atomic<int> ws_camera_handedness; extern std::atomic<unsigned> ws_camera_forward_offset; extern std::atomic<int> ws_camera_forward_sign; extern std::atomic<std::uint64_t> ws_camera_engage_ms; }
namespace {
constexpr std::uintptr_t kEntryRva = 0x1065680;
constexpr char kEntrySha[] = "04c49502901d0740a191ec0d821bf8b5ed269b0c1936030028491f9c701e91e6";
constexpr std::uintptr_t kActorRelativeFlag = 0x304;
constexpr unsigned kScanSpan = 0x1000, kScanSlots = (kScanSpan - 12) / 4 + 1;
std::uintptr_t target{};
std::atomic<bool> installed{false}, killed{false};
std::atomic<std::uintptr_t> engagedManager{0};
std::atomic<unsigned char> savedFlag{0};
std::atomic<std::uint64_t> lastWsMs{0}, engages{0}, restores{0}, refusedWritable{0}, wsSessionStartMs{0}, delayedUpdates{0};
constexpr std::uint64_t kEngageDelayMs = 0; // M5e-2 tried 1000 ms: did not stop the dive (REJECTED), reverted
struct Slot { unsigned fn{}, sn{}; double fsum{}, sdot{}, scross{}; };
Slot slots[kScanSlots];
unsigned scanSamples{}; std::uint64_t lastScanLog{};
// M5e diagnostic (read-only): camera-to-hero distance during WS entry, to locate the "camera too close" cause.
unsigned distOffsets[8]{}; unsigned distCount{}; std::uint64_t distLastMs{}, distEngageSeen{}; unsigned distBaselineLogs{}, distEntryLogs{};


bool sha256(const unsigned char* data, unsigned count, char (&text)[65]) {
  BCRYPT_ALG_HANDLE alg = nullptr; BCRYPT_HASH_HANDLE hash = nullptr; unsigned char object[512]{}, result[32]{}; DWORD size = 0, done = 0;
  bool ok = BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) >= 0;
  if (ok) ok = BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&size), sizeof(size), &done, 0) >= 0 && size <= sizeof(object);
  if (ok) ok = BCryptCreateHash(alg, &hash, object, size, nullptr, 0, 0) >= 0;
  if (ok) ok = BCryptHashData(hash, const_cast<PUCHAR>(data), count, 0) >= 0;
  if (ok) ok = BCryptFinishHash(hash, result, 32, 0) >= 0;
  if (hash) BCryptDestroyHash(hash);
  if (alg) BCryptCloseAlgorithmProvider(alg, 0);
  constexpr char hex[] = "0123456789abcdef";
  for (unsigned i = 0; i < 32; ++i) { text[2 * i] = hex[result[i] >> 4]; text[2 * i + 1] = hex[result[i] & 15]; }
  text[64] = 0; return ok;
}
bool writableByte(std::uintptr_t p) {
  MEMORY_BASIC_INFORMATION m{};
  return VirtualQuery(reinterpret_cast<void*>(p), &m, sizeof(m)) && m.State == MEM_COMMIT &&
         !(m.Protect & (PAGE_GUARD | PAGE_NOACCESS)) && (m.Protect & 0xff) == PAGE_READWRITE;
}
void restore(const char* reason) {
  auto manager = engagedManager.exchange(0);
  if (!manager) return;
  unsigned char current = 0;
  if (writableByte(manager + kActorRelativeFlag) && msm2::readMemory(manager + kActorRelativeFlag, &current, 1) && current == 1) {
    *reinterpret_cast<volatile unsigned char*>(manager + kActorRelativeFlag) = savedFlag.load();
  }
  restores.fetch_add(1);
  msm2::writeLog("WS_CAMERA_REF restore reason=%s manager=0x%llx restored_value=%u engages=%llu restores=%llu engage_delay_ms=%llu delayed_updates=%llu", reason,
                 static_cast<unsigned long long>(manager), unsigned(savedFlag.load()), static_cast<unsigned long long>(engages.load()),
                 static_cast<unsigned long long>(restores.load()), static_cast<unsigned long long>(kEngageDelayMs), static_cast<unsigned long long>(delayedUpdates.load()));
}
}

namespace {
void m5eDistance(const msm2::Snapshot& hero, std::uint64_t now) {
  auto manager = ws_camref_this;
  if (!manager || manager > UINTPTR_MAX - 0x1000 || !hero.hero || killed.load()) return;
  float blk[0x400 / 4];
  if (!msm2::readMemory(manager + 0xC00, blk, sizeof blk)) return;
  const float* p = hero.position;
  if (!std::isfinite(p[0]) || !std::isfinite(p[1]) || !std::isfinite(p[2]) || std::fabs(p[0]) + std::fabs(p[1]) + std::fabs(p[2]) < 1.f) return;
  auto dist = [&](unsigned o) { const float* v = &blk[(o - 0xC00) / 4]; float dx = v[0] - p[0], dy = v[1] - p[1], dz = v[2] - p[2]; return std::sqrt(dx * dx + dy * dy + dz * dz); };
  if (!distCount) { // pick position-like slots once: 0.5..20 m from the hero, not unit vectors
    for (unsigned o = 0xC00; o + 12 <= 0x1000 && distCount < 8; o += 4) {
      const float* v = &blk[(o - 0xC00) / 4];
      if (!std::isfinite(v[0]) || !std::isfinite(v[1]) || !std::isfinite(v[2])) continue;
      float l = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); if (l < 2.f) continue;
      float d = dist(o); if (d < 0.5f || d > 20.f) continue;
      if (distCount && o - distOffsets[distCount - 1] < 12) continue;
      distOffsets[distCount++] = o;
    }
    if (!distCount) return;
  }
  auto eng = ws_camera_engage_ms.load();
  bool entry = eng && now - eng < 3000;
  if (entry && eng != distEngageSeen) { distEngageSeen = eng; distEntryLogs = 0; }
  if (entry ? (now - distLastMs < 100 || distEntryLogs >= 30 || engages.load() > 12) : (now - distLastMs < 3000 || distBaselineLogs >= 20 || now - lastWsMs.load() < 1000)) return;
  distLastMs = now; if (entry) ++distEntryLogs; else ++distBaselineLogs;
  char t[700]; int len = 0;
  for (unsigned i = 0; i < distCount && len < int(sizeof t) - 30; ++i) len += std::snprintf(t + len, sizeof t - len, "%s0x%x:%.2f", i ? ";" : "", distOffsets[i], dist(distOffsets[i]));
  unsigned char st[4]{}; float bl[2]{}; msm2::readMemory(manager + 0x305, st, 4); msm2::readMemory(manager + 0x2fc, bl, 8);
  msm2::writeLog("WS_CAMERA_DISTANCE phase=%s t_ms=%llu engage=%llu distances=%s f2fc=%.3f f300=%.3f b305=%u,%u,%u,%u read_only=1", entry ? "entry" : "baseline",
    (unsigned long long)(entry ? now - eng : 0), (unsigned long long)engages.load(), t, bl[0], bl[1], st[0], st[1], st[2], st[3]);
}
}
namespace {
// M5e-3 diagnostic (read-only): find the camera pitch angle slot. On the ground (no WS for 1 s) every 50 ms,
// compare each float in manager +0..0x1000 with the pitch of the camera forward (+0xD34): +-radians, +-degrees.
struct PitchSlot { double e[4]{}; };
PitchSlot pitchSlots[0x1000 / 4]; unsigned pitchSamples{}; float pitchMin{10}, pitchMax{-10}; std::uint64_t pitchLastMs{}, pitchLastLog{}; unsigned pitchLogs{};
void m5ePitch(std::uint64_t now) {
  auto manager = ws_camref_this;
  if (!manager || manager > UINTPTR_MAX - 0x1000 || killed.load() || engagedManager.load() || now - lastWsMs.load() < 1000 || now - pitchLastMs < 50 || pitchLogs >= 8) return;
  pitchLastMs = now;
  static float blk[0x1000 / 4];
  if (!msm2::readMemory(manager, blk, sizeof blk)) return;
  const float* f = &blk[0xD34 / 4];
  float fl = std::sqrt(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]); if (!std::isfinite(fl) || fl < 0.95f || fl > 1.05f) return;
  float pitch = std::asin(std::fmax(-1.f, std::fmin(1.f, f[1] / fl)));
  pitchMin = std::fmin(pitchMin, pitch); pitchMax = std::fmax(pitchMax, pitch); ++pitchSamples;
  const double unit[4] = {1, -1, 57.29577951, -57.29577951};
  for (unsigned i = 0; i < 0x1000 / 4; ++i) { double v = blk[i]; if (!std::isfinite(v) || std::fabs(v) > 1e4) v = 1e4;
    for (int k = 0; k < 4; ++k) { double d = (v - unit[k] * pitch) / std::fabs(unit[k]); pitchSlots[i].e[k] += d * d; } }
  if (pitchSamples < 60 || pitchMax - pitchMin < 0.6f || now - pitchLastLog < 5000) return;
  pitchLastLog = now; ++pitchLogs;
  char t[900]; int len = 0, k2 = 0;
  const char* names[4] = {"rad", "-rad", "deg", "-deg"};
  for (unsigned i = 0; i < 0x1000 / 4 && k2 < 12; ++i) for (int k = 0; k < 4; ++k) {
    double rms = std::sqrt(pitchSlots[i].e[k] / pitchSamples); if (rms > 0.12) continue;
    if (i * 4 >= 0xD34 && i * 4 < 0xD40) continue; // the forward vector itself
    if (len < int(sizeof t) - 40) { len += std::snprintf(t + len, sizeof t - len, "%s0x%x:%s:%.3f", k2 ? ";" : "", i * 4, names[k], rms); }
    ++k2; }
  msm2::writeLog("WS_CAMERA_PITCH_SCAN samples=%u pitch_range=%.2f..%.2f candidates=%s read_only=1", pitchSamples, pitchMin, pitchMax, k2 ? t : "none");
}
}
namespace msm2::camera_ref {
void configure(bool verified, std::uintptr_t image, std::size_t imageSize) {
  if (!verified || !image || imageSize <= kEntryRva + 64) { writeLog("WS_CAMERA_REF admitted=0 reason=build_or_image_unverified"); return; }
  target = image + kEntryRva;
  DWORD64 base = 0; auto* f = RtlLookupFunctionEntry(target, &base, nullptr);
  unsigned char entry[64]{}; char hash[65]{};
  bool ok = f && base == image && f->BeginAddress == kEntryRva && readMemory(target, entry, sizeof(entry)) &&
            sha256(entry, sizeof(entry), hash) && !std::strcmp(hash, kEntrySha);
  if (!ok) { writeLog("WS_CAMERA_REF admitted=0 reason=entry_pdata_or_hash_mismatch sha=%s", hash[0] ? hash : "none"); return; }
  HMODULE pin = nullptr;
  if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, reinterpret_cast<LPCWSTR>(&configure), &pin)) return;
  auto init = MH_Initialize(); if (init != MH_OK && init != MH_ERROR_ALREADY_INITIALIZED) return;
  if (MH_CreateHook(reinterpret_cast<void*>(target), reinterpret_cast<void*>(&ws_camref_thunk), &ws_camref_original) != MH_OK) return;
  installed = MH_EnableHook(reinterpret_cast<void*>(target)) == MH_OK;
  if (installed.load()) ws_wallsprint_update_listener.store(&onWallSprintUpdate, std::memory_order_release);
  writeLog("WS_CAMERA_REF admitted=%u entry_rva=0x1065680 entry_sha256=%s flag_offset=0x304 mode=actor_relative_only_during_WS thunk=record_rcx_tailjump",
           unsigned(installed.load()), kEntrySha);
}

void onWallSprintUpdate() noexcept {
  if (!installed.load() || killed.load()) return;
  auto manager = ws_camref_this;
  if (!manager || manager > UINTPTR_MAX - 0x400) return;
  auto nowMs = GetTickCount64();
  // M5e-2: a new WS session starts after a >300 ms gap. Engage the actor-relative camera only once the
  // orientation override has rotated the hero onto the wall (entry 0.70 s + 0.25 s rotation), so the camera
  // blend starts from the final frame instead of following the rotating actor axes (camera dived to the feet).
  if (nowMs - lastWsMs.load() > 300) wsSessionStartMs.store(nowMs);
  lastWsMs.store(nowMs);
  if (engagedManager.load() == manager) return;
  if (nowMs - wsSessionStartMs.load() < kEngageDelayMs) { delayedUpdates.fetch_add(1); return; }
  if (engagedManager.load()) restore("manager_changed");
  unsigned char current = 0;
  if (!writableByte(manager + kActorRelativeFlag) || !readMemory(manager + kActorRelativeFlag, &current, 1)) { refusedWritable.fetch_add(1); return; }
  savedFlag.store(current);
  *reinterpret_cast<volatile unsigned char*>(manager + kActorRelativeFlag) = 1;
  engagedManager.store(manager); engages.fetch_add(1); ws_camera_engage_ms.store(GetTickCount64());
}

void tick(const msm2::Snapshot& snapshot, const ws::diag::Motion& motion, bool available, float sx, float sy, bool stick) noexcept {
  auto now = GetTickCount64();
  if (engagedManager.load() && now - lastWsMs.load() > 300) restore("wallsprint_ended");
  // (Disabled by default: actor_writer.cpp now starts with the accepted offset 0xD34, so the guard below returns at once.)
  // Read-only scan on the ground (no WS in the last second), moving: the camera forward is the unit vector whose
  // horizontal part is parallel to travel for a forward stick and perpendicular for a sideways stick
  // (travel = sign(sx) * h * cross(forward, up)). Accepted once per process; then used for camera-relative WS input.
  m5eDistance(snapshot, now);
  m5ePitch(now);
  auto manager = ws_camref_this;
  (void)available;
  if (ws_camera_forward_offset.load() || !manager || !motion.valid || now - lastWsMs.load() < 1000 || !stick) return;
  float vx = motion.velocity.x, vz = motion.velocity.z, sp = std::sqrt(vx * vx + vz * vz);
  if (!std::isfinite(sp) || sp < 3.f) return;
  vx /= sp; vz /= sp;
  const bool forward = sy > 0.8f && std::fabs(sx) < 0.3f, side = std::fabs(sx) > 0.7f && std::fabs(sy) < 0.35f;
  if (!forward && !side) return;
  unsigned char span[kScanSpan];
  unsigned validBytes = 0;
  for (unsigned o = 0; o < kScanSpan; o += 0x400) { if (!readMemory(manager + o, span + o, 0x400)) break; validBytes = o + 0x400; }
  if (!validBytes) return;
  ++scanSamples;
  const float sgn = sx > 0 ? 1.f : -1.f;
  const unsigned validSlots = (validBytes - 12) / 4 + 1;
  for (unsigned i = 0; i < validSlots; ++i) {
    float v[3]; std::memcpy(v, span + 4 * i, sizeof v);
    if (!std::isfinite(v[0]) || !std::isfinite(v[1]) || !std::isfinite(v[2])) continue;
    float n = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); if (n < 0.95f || n > 1.05f) continue;
    float h = std::sqrt(v[0] * v[0] + v[2] * v[2]); if (h < 0.3f) continue;
    float fx = v[0] / h, fz = v[2] / h;
    auto& s = slots[i];
    if (forward) { s.fn++; s.fsum += fx * vx + fz * vz; }
    else { s.sn++; s.sdot += fx * vx + fz * vz; s.scross += sgn * (-fz * vx + fx * vz); }
  }
  int best = -1; double bestScore = 0;
  for (unsigned i = 0; i < kScanSlots; ++i) {
    const auto& s = slots[i];
    if (s.fn < 10 || s.sn < 10) continue;
    double f = s.fsum / s.fn, d = s.sdot / s.sn, x = s.scross / s.sn;
    double score = std::fmin(std::fabs(f), std::fabs(x)) - std::fabs(d);
    if (score > bestScore) { bestScore = score; best = int(i); }
  }
  if (best >= 0 && now - lastScanLog > 3000) {
    lastScanLog = now;
    const auto& s = slots[unsigned(best)];
    double f = s.fsum / s.fn, d = s.sdot / s.sn, x = s.scross / s.sn;
    bool ok = std::fabs(f) > 0.9 && std::fabs(d) < 0.3 && std::fabs(x) > 0.85;
    int forwardSign = f > 0 ? 1 : -1, hand = (x > 0 ? 1 : -1) * forwardSign;
    if (ok) { ws_camera_forward_sign.store(forwardSign); ws_camera_handedness.store(hand); ws_camera_forward_offset.store(unsigned(best) * 4); }
    writeLog("WS_CAMERA_FORWARD_SCAN samples=%u best_offset=0x%x forward_n=%u forward_dot=%.4f side_n=%u side_dot=%.4f side_cross=%.4f accepted=%u forward_sign=%d handedness=%d game_write=0",
             scanSamples, unsigned(best) * 4, s.fn, f, s.sn, d, x, unsigned(ok), forwardSign, hand);
  }
}

void kill() noexcept { killed = true; ws_wallsprint_update_listener.store(nullptr); restore("kill"); }
void stop() noexcept {
  killed = true; ws_wallsprint_update_listener.store(nullptr); restore("stop");
  if (installed.exchange(false)) MH_DisableHook(reinterpret_cast<void*>(target));
}
}
