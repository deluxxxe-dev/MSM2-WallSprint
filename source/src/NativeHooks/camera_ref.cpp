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
#include <cstring>
#include "NativeHooks/camera_ref.hpp"
extern "C" { void ws_camref_thunk(); void* ws_camref_original{}; extern std::uintptr_t ws_camref_this; extern std::atomic<void(*)()> ws_wallsprint_update_listener;
// M5c: +1/-1 once ground samples prove manager+0x1E0 is the camera forward and give the right = sign*cross(forward, up) handedness.
extern std::atomic<int> ws_camera_handedness; }
namespace {
constexpr std::uintptr_t kEntryRva = 0x1065680;
constexpr char kEntrySha[] = "04c49502901d0740a191ec0d821bf8b5ed269b0c1936030028491f9c701e91e6";
constexpr std::uintptr_t kActorRelativeFlag = 0x304;
constexpr unsigned kScanSpan = 0x800, kScanSlots = (kScanSpan - 12) / 4 + 1;
std::uintptr_t target{};
std::atomic<bool> installed{false}, killed{false};
std::atomic<std::uintptr_t> engagedManager{0};
std::atomic<unsigned char> savedFlag{0};
std::atomic<std::uint64_t> lastWsMs{0}, engages{0}, restores{0}, refusedWritable{0};
struct Slot { unsigned n{}; double sum{}; };
Slot slots[kScanSlots];
unsigned scanSamples{}; std::uint64_t lastScanLog{};
constexpr std::uintptr_t kCameraForward = 0x1e0; // M5b scan: mean_dot 0.91-0.92 with forward-stick ground travel
unsigned sideSamples{}; double sideDot{}, sideCross{};

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
  msm2::writeLog("WS_CAMERA_REF restore reason=%s manager=0x%llx restored_value=%u engages=%llu restores=%llu", reason,
                 static_cast<unsigned long long>(manager), unsigned(savedFlag.load()), static_cast<unsigned long long>(engages.load()),
                 static_cast<unsigned long long>(restores.load()));
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
  lastWsMs.store(GetTickCount64());
  if (engagedManager.load() == manager) return;
  if (engagedManager.load()) restore("manager_changed");
  unsigned char current = 0;
  if (!writableByte(manager + kActorRelativeFlag) || !readMemory(manager + kActorRelativeFlag, &current, 1)) { refusedWritable.fetch_add(1); return; }
  savedFlag.store(current);
  *reinterpret_cast<volatile unsigned char*>(manager + kActorRelativeFlag) = 1;
  engagedManager.store(manager); engages.fetch_add(1);
}

void tick(const msm2::Snapshot&, const ws::diag::Motion& motion, bool available, float sx, float sy, bool stick) noexcept {
  auto now = GetTickCount64();
  if (engagedManager.load() && now - lastWsMs.load() > 300) restore("wallsprint_ended");
  // Read-only scans on the ground (no WS in the last second), moving.
  auto manager = ws_camref_this;
  (void)available;
  if (!manager || !motion.valid || now - lastWsMs.load() < 1000 || !stick) return;
  float vx = motion.velocity.x, vz = motion.velocity.z, sp = std::sqrt(vx * vx + vz * vz);
  if (!std::isfinite(sp) || sp < 3.f) return;
  vx /= sp; vz /= sp;
  if (std::fabs(sx) > 0.7f && std::fabs(sy) < 0.35f && !ws_camera_handedness.load()) {
    // Sideways stick: camera forward must be perpendicular to travel; travel = sign(sx)*s*cross(forward, up).
    float f[3];
    if (readMemory(manager + kCameraForward, f, sizeof f) && std::isfinite(f[0]) && std::isfinite(f[2])) {
      float h = std::sqrt(f[0] * f[0] + f[2] * f[2]);
      if (h > 0.3f) {
        float fx = f[0] / h, fz = f[2] / h;
        // cross(f,(0,1,0)) = (-fz, 0, fx)
        sideDot += fx * vx + fz * vz; sideCross += (sx > 0 ? 1.f : -1.f) * (-fz * vx + fx * vz); ++sideSamples;
        if (sideSamples % 5 == 0 && sideSamples < 10) writeLog("WS_CAMERA_HANDEDNESS progress samples=%u", sideSamples);
        if (sideSamples >= 10) {
          double d = sideDot / sideSamples, x = sideCross / sideSamples;
          bool ok = std::fabs(d) < 0.5 && std::fabs(x) > 0.6;
          if (ok) ws_camera_handedness.store(x > 0 ? 1 : -1);
          writeLog("WS_CAMERA_HANDEDNESS samples=%u forward_dot_travel=%.4f signed_cross_dot=%.4f accepted=%u handedness=%d offset=0x1e0",
                   sideSamples, d, x, unsigned(ok), ws_camera_handedness.load());
          if (!ok) { sideSamples = 0; sideDot = sideCross = 0; }
        }
      }
    }
    return;
  }
  if (sy < 0.8f || std::fabs(sx) > 0.25f) return;
  unsigned char span[kScanSpan];
  if (!readMemory(manager, span, 0x400) || !readMemory(manager + 0x400, span + 0x400, 0x400)) return;
  ++scanSamples;
  for (unsigned i = 0; i < kScanSlots; ++i) {
    float v[3]; std::memcpy(v, span + 4 * i, sizeof v);
    if (!std::isfinite(v[0]) || !std::isfinite(v[1]) || !std::isfinite(v[2])) continue;
    float n = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); if (n < 0.95f || n > 1.05f) continue;
    float h = std::sqrt(v[0] * v[0] + v[2] * v[2]); if (h < 0.3f) continue;
    slots[i].n++; slots[i].sum += (v[0] * vx + v[2] * vz) / h;
  }
  if (scanSamples >= 30 && now - lastScanLog > 10000) {
    lastScanLog = now;
    for (int sign : {1, -1}) {
      bool used[kScanSlots]{};
      for (unsigned rank = 0; rank < 6; ++rank) {
        int best = -1; double bestMean = 0;
        for (unsigned i = 0; i < kScanSlots; ++i) {
          if (used[i] || slots[i].n < 20) continue;
          double mean = sign * slots[i].sum / slots[i].n;
          if (best < 0 || mean > bestMean) { best = int(i); bestMean = mean; }
        }
        if (best < 0 || bestMean < 0.8) break;
        used[best] = true;
        writeLog("WS_CAMERA_FORWARD_SCAN samples=%u sign=%s rank=%u offset=0x%x n=%u mean_dot=%.4f semantics=OBSERVED_CORRELATION_ONLY game_write=0",
                 scanSamples, sign > 0 ? "aligned" : "opposite", rank + 1, unsigned(best) * 4, slots[best].n, slots[best].sum / slots[best].n);
      }
    }
  }
}

void kill() noexcept { killed = true; ws_wallsprint_update_listener.store(nullptr); restore("kill"); }
void stop() noexcept {
  killed = true; ws_wallsprint_update_listener.store(nullptr); restore("stop");
  if (installed.exchange(false)) MH_DisableHook(reinterpret_cast<void*>(target));
}
}
