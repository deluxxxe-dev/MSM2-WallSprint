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
#include <xinput.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <unordered_map>
#include <vector>
#include "NativeHooks/camera_ref.hpp"
#include "msm2/actor_writer.hpp"
#include "msm2/ws_settings.hpp"
#include "msm2/drive_command.hpp"
extern "C" { void ws_camset_thunk(); void* ws_camset_original{}; std::uintptr_t ws_camset_record(std::uintptr_t rcx, std::uintptr_t rdx, std::uintptr_t r8, std::uintptr_t r9) noexcept; }
extern "C" { void ws_camref_thunk(); void* ws_camref_original{}; extern std::uintptr_t ws_camref_this; extern std::uintptr_t ws_camref_raw, ws_camref_seq, ws_camref_ring[8]; extern std::atomic<void(*)()> ws_wallsprint_update_listener;
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
std::atomic<std::uint64_t> exitRestoreMs{0}, exitEpisode{0}, exitBlendStart{0};
std::atomic<std::uint64_t> lastWsMs{0}, engages{0}, restores{0}, refusedWritable{0}, wsSessionStartMs{0}, delayedUpdates{0};
constexpr std::uint64_t kEngageDelayMs = 0; // M5e-2 tried 1000 ms: did not stop the dive (REJECTED), reverted
struct Slot { unsigned fn{}, sn{}; double fsum{}, sdot{}, scross{}; };
Slot slots[kScanSlots];
unsigned scanSamples{}; std::uint64_t lastScanLog{};
// M5e diagnostic (read-only): camera-to-hero distance during WS entry, to locate the "camera too close" cause.
std::uint64_t statusLastMs{}; unsigned statusLogs{};
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
  exitRestoreMs.store(GetTickCount64()); exitEpisode.fetch_add(1);
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
double rowDot[3]{};
void m5ePitch(const msm2::Snapshot& hero, std::uint64_t now) {
  auto manager = ws_camref_this;
  if (!manager || manager > UINTPTR_MAX - 0x1000 || killed.load() || engagedManager.load() || now - lastWsMs.load() < 1000 || now - pitchLastMs < 50 || pitchLogs >= 8) return;
  pitchLastMs = now;
  static float blk[0x1000 / 4];
  if (!msm2::readMemory(manager, blk, sizeof blk)) return;
  // M5e-3d: pitch of the actual view line camera position (+0xD54) -> hero (the matrix row naming is unproven).
  const float* c = &blk[0xD54 / 4];
  float lx = hero.position[0] - c[0], ly = hero.position[1] + 1.0f - c[1], lz = hero.position[2] - c[2];
  float ll = std::sqrt(lx * lx + ly * ly + lz * lz); if (!hero.hero || !std::isfinite(ll) || ll < 0.5f || ll > 30.f) return;
  lx /= ll; ly /= ll; lz /= ll;
  for (int r = 0; r < 3; ++r) { const float* v = &blk[(0xD24 + 0x10 * r) / 4]; rowDot[r] += v[0] * lx + v[1] * ly + v[2] * lz; }
  float pitch = std::asin(std::fmax(-1.f, std::fmin(1.f, ly)));
  pitchMin = std::fmin(pitchMin, pitch); pitchMax = std::fmax(pitchMax, pitch); ++pitchSamples;
  const double unit[4] = {1, -1, 57.29577951, -57.29577951};
  for (unsigned i = 0; i < 0x1000 / 4; ++i) { double v = blk[i]; if (!std::isfinite(v) || std::fabs(v) > 1e4) v = 1e4;
    for (int k = 0; k < 4; ++k) { double d = (v - unit[k] * pitch) / std::fabs(unit[k]); pitchSlots[i].e[k] += d * d; } }
  if (pitchSamples < 60 || pitchMax - pitchMin < 0.6f || now - pitchLastLog < 5000) return;
  pitchLastLog = now; ++pitchLogs;
  char t[900]; int len = 0, k2 = 0;
  const char* names[4] = {"rad", "-rad", "deg", "-deg"};
  for (unsigned i = 0; i < 0x1000 / 4 && k2 < 12; ++i) for (int k = 0; k < 4; ++k) {
    double rms = std::sqrt(pitchSlots[i].e[k] / pitchSamples); if (rms > 0.15) continue;
    if (i * 4 >= 0xD24 && i * 4 < 0xD60) continue; // the matrix rows and position themselves
    if (len < int(sizeof t) - 40) { len += std::snprintf(t + len, sizeof t - len, "%s0x%x:%s:%.3f", k2 ? ";" : "", i * 4, names[k], rms); }
    ++k2; }
  msm2::writeLog("WS_CAMERA_PITCH_SCAN samples=%u pitch_range=%.2f..%.2f view_line_dot_D24=%.3f D34=%.3f D44=%.3f candidates=%s read_only=1 rev=m5e3d", pitchSamples, pitchMin, pitchMax, rowDot[0] / pitchSamples, rowDot[1] / pitchSamples, rowDot[2] / pitchSamples, k2 ? t : "none");
}
}
namespace {
// M5e-3c: choose the live camera manager among recent callers: unit forward (+0xD34) and camera position
// (+0xD54) within 30 m of the hero; nearest wins. Keeps the previous choice while it stays valid.
std::uint64_t selectLastLog{}; unsigned selectChanges{};
bool liveCamera(std::uintptr_t m, const msm2::Snapshot& hero, float& distance) {
  if (!m || m > UINTPTR_MAX - 0x1000) return false;
  float v[8]{}; if (!msm2::readMemory(m + 0xD34, v, sizeof v)) return false; // forward D34..D3F, up D44, position D54 via second read
  float fl = v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; if (!std::isfinite(fl) || fl < 0.9f || fl > 1.1f) return false;
  float p[3]{}; if (!msm2::readMemory(m + 0xD54, p, sizeof p) || !std::isfinite(p[0]) || !std::isfinite(p[1]) || !std::isfinite(p[2])) return false;
  if (!hero.hero) { distance = 0; return true; }
  float dx = p[0] - hero.position[0], dy = p[1] - hero.position[1], dz = p[2] - hero.position[2];
  distance = std::sqrt(dx * dx + dy * dy + dz * dz); return std::isfinite(distance) && distance < 30.f;
}
void selectCamera(const msm2::Snapshot& hero, std::uint64_t now) {
  std::uintptr_t seen[9]{}; unsigned n = 0;
  auto add = [&](std::uintptr_t m) { if (!m) return; for (unsigned i = 0; i < n; ++i) if (seen[i] == m) return; seen[n++] = m; };
  for (auto m : ws_camref_ring) { add(m); }
  add(ws_camref_raw);
  std::uintptr_t best = 0; float bestD = 1e9f, curD = 0; bool curOk = liveCamera(ws_camref_this, hero, curD);
  for (unsigned i = 0; i < n; ++i) { float d; if (liveCamera(seen[i], hero, d) && d < bestD) { bestD = d; best = seen[i]; } }
  std::uintptr_t pick = curOk ? ws_camref_this : best;
  if (pick != ws_camref_this) { ++selectChanges;
    msm2::writeLog("WS_CAMERA_SELECT manager=0x%llx previous=0x%llx distinct_callers=%u distance=%.2f changes=%u", (unsigned long long)pick, (unsigned long long)ws_camref_this, n, pick ? bestD : 0.f, selectChanges);
    if (engagedManager.load() && engagedManager.load() != pick) restore("camera_manager_changed");
    ws_camref_this = pick; }
  (void)now; (void)selectLastLog;
}
}
namespace {
// M6c diagnostic (read-only): ground sprint facing turn rate, to copy the ground behavior onto the wall.
// Samples (100 ms worker) on the ground (no WS for 1 s) with the stick held and horizontal speed > 5 m/s.
const float kTurnBins[] = {45, 90, 180, 270, 360, 540, 720, 900, 1200};
double steadySum[2]{}; float steadyMax[2]{}; unsigned steadyN[2]{}, steadyLogs{}; std::uint64_t steadySince[2]{}, steadyLastLog{};
unsigned turnHist[10]{}; unsigned turnSamples{}; float turnMax{}; double turnSpeedSum{}; float prevYaw{}; std::uint64_t prevYawMs{}, turnLastLog{}; unsigned turnLogs{}; bool prevYawOk{};
void m6GroundTurn(const msm2::Snapshot& hero, const ws::diag::Motion& motion, bool stick, std::uint64_t now) {
  auto axes = ws::diag::actorAxes(hero.prefix);
  float sp = motion.valid ? std::sqrt(motion.velocity.x * motion.velocity.x + motion.velocity.z * motion.velocity.z) : 0.f;
  bool ok = hero.hero && axes.valid && std::fabs(axes.up.y) > 0.95f && now - lastWsMs.load() > 1000 && stick && std::isfinite(sp) && sp > 5.f;
  float yaw = std::atan2(axes.forward.x, axes.forward.z);
  if (ok && prevYawOk && now > prevYawMs && now - prevYawMs <= 250) {
    float d = yaw - prevYaw; while (d > 3.14159265f) d -= 6.2831853f; while (d < -3.14159265f) d += 6.2831853f;
    float rate = std::fabs(d) * 57.29578f * 1000.f / float(now - prevYawMs);
    unsigned b = 0; while (b < 9 && rate >= kTurnBins[b]) ++b; ++turnHist[b]; ++turnSamples; turnMax = std::fmax(turnMax, rate); turnSpeedSum += sp;
  }
  // M6i: steady straight-line speed (yaw rate < 20 deg/s for >= 1 s, stick held), ground vs WS, to match WS speed.
  { bool wsNow = engagedManager.load() != 0 && now - lastWsMs.load() < 300; float sp3 = motion.valid ? std::sqrt(ws::dot(motion.velocity, motion.velocity)) : 0.f;
    float yr = 1e9f; if (prevYawOk && now > prevYawMs && now - prevYawMs <= 250) { float dd = yaw - prevYaw; while (dd > 3.14159265f) dd -= 6.2831853f; while (dd < -3.14159265f) dd += 6.2831853f; yr = std::fabs(dd) * 57.29578f * 1000.f / float(now - prevYawMs); }
    bool straight = stick && hero.hero && axes.valid && std::isfinite(sp3) && sp3 > 3.f && (wsNow || (now - lastWsMs.load() > 1000 && std::fabs(axes.up.y) > 0.95f && yr < 20.f));
    int k = wsNow ? 1 : 0; if (straight) { if (!steadySince[k]) steadySince[k] = now; if (now - steadySince[k] >= 1000) { steadySum[k] += sp3; steadyMax[k] = std::fmax(steadyMax[k], sp3); ++steadyN[k]; } } else steadySince[k] = 0;
    if (now - steadyLastLog >= 10000 && (steadyN[0] || steadyN[1]) && steadyLogs < 30) { steadyLastLog = now; ++steadyLogs;
      msm2::writeLog("WS_STEADY_SPEED ground_n=%u mean=%.2f max=%.2f | ws_n=%u mean=%.2f max=%.2f read_only=1", steadyN[0], steadyN[0] ? steadySum[0] / steadyN[0] : 0., steadyMax[0], steadyN[1], steadyN[1] ? steadySum[1] / steadyN[1] : 0., steadyMax[1]); } }
  prevYaw = yaw; prevYawMs = now; prevYawOk = ok;
  if (turnSamples >= 30 && now - turnLastLog >= 10000 && turnLogs < 20) { turnLastLog = now; ++turnLogs;
    msm2::writeLog("WS_GROUND_TURN_RATE samples=%u mean_speed=%.2f max_deg_s=%.0f hist_deg_s=<45:%u,<90:%u,<180:%u,<270:%u,<360:%u,<540:%u,<720:%u,<900:%u,<1200:%u,>=1200:%u read_only=1",
      turnSamples, turnSpeedSum / turnSamples, turnMax, turnHist[0], turnHist[1], turnHist[2], turnHist[3], turnHist[4], turnHist[5], turnHist[6], turnHist[7], turnHist[8], turnHist[9]); }
}
}
namespace {
// M6e diagnostic (read-only): camera framing, ground vs WS, for parity. Camera rows: +0xD44 forward (view-line dot
// 0.86 on ground, M5e-3d run), +0xD34 up, +0xD54 position. "Surface up" = hero actor up (wall normal during WS).
struct Framing { double off{}, offMax{}, dist{}, height{}, behind{}; unsigned n{}, nb{}; };
Framing framing[2]; std::uint64_t framingLastMs{}, framingLastLog{}; unsigned framingLogs{};
// M6l: WS framing split by travel direction (world vertical): 0 up, 1 sideways, 2 down.
struct DirBin { double dist{}, off{}, speed{}; unsigned n{}; }; DirBin dirBins[3];
void m6Framing(const msm2::Snapshot& hero, const ws::diag::Motion& motion, std::uint64_t now) {
  auto m = ws_camref_this; if (!m || m > UINTPTR_MAX - 0x1000 || !hero.hero || now - framingLastMs < 100) return;
  framingLastMs = now;
  float r[12]{}; if (!msm2::readMemory(m + 0xD34, r, 12) || !msm2::readMemory(m + 0xD44, r + 4, 12) || !msm2::readMemory(m + 0xD54, r + 8, 12)) return;
  ws::Vec3 f{r[4], r[5], r[6]}, c{r[8], r[9], r[10]}; float fl = std::sqrt(ws::dot(f, f)); if (!std::isfinite(fl) || fl < 0.9f || fl > 1.1f) return; f = f * (1 / fl);
  auto axes = ws::diag::actorAxes(hero.prefix); if (!axes.valid) return; ws::Vec3 up = axes.up;
  bool ws = engagedManager.load() != 0 && now - lastWsMs.load() < 300;
  ws::Vec3 p{hero.position[0], hero.position[1], hero.position[2]}; ws::Vec3 target = p + up * 1.0f, to = target - c;
  float d = std::sqrt(ws::dot(to, to)); if (!std::isfinite(d) || d < 0.3f || d > 30.f) return;
  float off = std::acos(std::fmax(-1.f, std::fmin(1.f, ws::dot(f, to * (1 / d))))) * 57.29578f;
  float h = ws::dot(c - p, up);
  auto& F = framing[ws ? 1 : 0]; F.off += off; F.offMax = std::fmax(F.offMax, off); F.dist += d; F.height += h; ++F.n;
  ws::Vec3 v = motion.velocity - up * ws::dot(motion.velocity, up), fp = f - up * ws::dot(f, up);
  float vl = std::sqrt(ws::dot(v, v)), fpl = std::sqrt(ws::dot(fp, fp));
  if (ws && motion.valid) { float sp = std::sqrt(ws::dot(motion.velocity, motion.velocity)); if (sp > 3.f) { float vy = motion.velocity.y / sp; int b = vy > 0.5f ? 0 : vy < -0.5f ? 2 : 1;
    dirBins[b].dist += d; dirBins[b].off += off; dirBins[b].speed += sp; ++dirBins[b].n; } }
  if (motion.valid && vl > 3.f && fpl > 0.2f) { F.behind += std::acos(std::fmax(-1.f, std::fmin(1.f, ws::dot(v, fp) / (vl * fpl)))) * 57.29578f; ++F.nb; }
  if (now - framingLastLog >= 10000 && framingLogs < 30 && (framing[0].n || framing[1].n)) { framingLastLog = now; ++framingLogs;
    auto avg = [](double a, unsigned n) { return n ? a / n : 0.; };
    msm2::writeLog("WS_CAMERA_FRAMING ground_n=%u off_center_deg=%.1f max=%.1f dist=%.2f height=%.2f travel_vs_view_deg=%.1f(n=%u) | ws_n=%u off_center_deg=%.1f max=%.1f dist=%.2f height=%.2f travel_vs_view_deg=%.1f(n=%u) read_only=1",
      framing[0].n, avg(framing[0].off, framing[0].n), framing[0].offMax, avg(framing[0].dist, framing[0].n), avg(framing[0].height, framing[0].n), avg(framing[0].behind, framing[0].nb), framing[0].nb,
      framing[1].n, avg(framing[1].off, framing[1].n), framing[1].offMax, avg(framing[1].dist, framing[1].n), avg(framing[1].height, framing[1].n), avg(framing[1].behind, framing[1].nb), framing[1].nb);
    auto bavg = [](double a, unsigned n) { return n ? a / n : 0.; };
    msm2::writeLog("WS_CAMERA_FRAMING_BY_DIR up_n=%u dist=%.2f off=%.1f speed=%.1f | side_n=%u dist=%.2f off=%.1f speed=%.1f | down_n=%u dist=%.2f off=%.1f speed=%.1f read_only=1",
      dirBins[0].n, bavg(dirBins[0].dist, dirBins[0].n), bavg(dirBins[0].off, dirBins[0].n), bavg(dirBins[0].speed, dirBins[0].n),
      dirBins[1].n, bavg(dirBins[1].dist, dirBins[1].n), bavg(dirBins[1].off, dirBins[1].n), bavg(dirBins[1].speed, dirBins[1].n),
      dirBins[2].n, bavg(dirBins[2].dist, dirBins[2].n), bavg(dirBins[2].off, dirBins[2].n), bavg(dirBins[2].speed, dirBins[2].n));
    for (auto& b : dirBins) b = {};
    framing[0] = {}; framing[1] = {}; }
}
}
namespace {
// M6r: setter capture (game thread). Copies 64 bytes behind rdx and r8 when they are readable.
constexpr std::uintptr_t kSetterRva = 0x3350220;
std::atomic<bool> setYawLock{false}; std::atomic<std::uint64_t> lockWrites{0}, lockSkips{0};
std::atomic<std::uint64_t> flagWrites{0}; std::atomic<std::uintptr_t> setR9{0};
std::atomic<std::uint64_t> angleHoldWrites{0}, angleManual{0};
std::atomic<std::uint64_t> exitBlendWrites{0}; std::atomic<bool> gbAirCam{false};
std::atomic<std::uint64_t> setCalls{0}; std::atomic<std::uintptr_t> setRcx{0}, setRdx{0}, setR8{0};
std::atomic<float> setM[2][16]{}; std::atomic<unsigned> setMOk{0}; std::atomic<bool> setInstalled{false}; std::uintptr_t setTarget{};
unsigned setLogs{}; std::uint64_t setLastLog{};
void m6SetterLog(const msm2::Snapshot& hero, std::uint64_t now) {
  if (!setInstalled.load() || now - setLastLog < 500 || setLogs >= 120) return;
  bool wsNow = engagedManager.load() != 0 && now - lastWsMs.load() < 300; if (!wsNow && setLogs % 4) { if (now - setLastLog < 5000) return; }
  setLastLog = now; ++setLogs;
  auto axes = ws::diag::actorAxes(hero.prefix);
  char t[900]; int len = 0; unsigned ok = setMOk.load();
  for (int k = 0; k < 2; ++k) { if (!(ok & (1u << k))) { len += std::snprintf(t + len, sizeof t - len, " m%d=unreadable", k); continue; }
    float m[16]; for (int i = 0; i < 16; ++i) m[i] = setM[k][i].load(std::memory_order_relaxed);
    ws::Vec3 r0{m[0], m[1], m[2]}, r1{m[4], m[5], m[6]}, r2{m[8], m[9], m[10]};
    len += std::snprintf(t + len, sizeof t - len, " m%d=[%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.2f,%.2f,%.2f,%.2f] dots_vs_hero r0.right=%.2f r1.up=%.2f r2.fwd=%.2f",
      k, m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14], m[15],
      axes.valid ? ws::dot(r0, axes.right) : 0.f, axes.valid ? ws::dot(r1, axes.up) : 0.f, axes.valid ? ws::dot(r2, axes.forward) : 0.f); }
  msm2::writeLog("WS_CAMERA_SETTER exit_blend_writes=%llu angle_hold_writes=%llu manual=%llu flag_writes=%llu r9_in=0x%llx yaw_lock_writes=%llu skips=%llu calls=%llu ws=%u rcx=0x%llx rdx=0x%llx r8=0x%llx manager=0x%llx%s read_only=1", (unsigned long long)exitBlendWrites.load(), (unsigned long long)angleHoldWrites.load(), (unsigned long long)angleManual.load(), (unsigned long long)flagWrites.load(), (unsigned long long)setR9.load(), (unsigned long long)lockWrites.load(), (unsigned long long)lockSkips.load(), (unsigned long long)setCalls.load(), unsigned(wsNow),
    (unsigned long long)setRcx.load(), (unsigned long long)setRdx.load(), (unsigned long long)setR8.load(), (unsigned long long)ws_camref_this, t);
}
}
// M6s: yaw-stable WS camera reference (auto-centering OFF). M6r measured the setter's rdx block during WS:
// +0x00 = right at WS start (constant), +0x10 = wall normal (= hero up), +0x20 = current hero right (turns with the
// facing). Replacing +0x20 with the session-start right (projected onto the wall plane) keeps the reference yaw
// fixed while the roll onto the wall stays. Only with the pinned entry SHA, while the WS camera is engaged.
float lockRight[3]{}; bool lockValid{}; std::uint64_t lockLastMs{};
void yawLock(std::uintptr_t rdx) noexcept {
  auto now = GetTickCount64(); if (now - lockLastMs > 300) lockValid = false; lockLastMs = now;
  if (!setYawLock.load(std::memory_order_relaxed) || ws::drive::cameraAutoCenter.load(std::memory_order_relaxed) || !engagedManager.load() || killed.load()) { lockValid = false; return; }
  float v[12]; if (rdx < 0x10000 || rdx > UINTPTR_MAX - 48 || !msm2::readMemory(rdx, v, sizeof v)) { lockSkips.fetch_add(1); return; }
  for (float x : v) if (!std::isfinite(x)) { lockSkips.fetch_add(1); return; }
  float* up = v + 4; float* cur = v + 8;
  auto unit3 = [](float* a) { float l = std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]); if (!(l > 0.5f && l < 1.5f)) return false; for (int i = 0; i < 3; ++i) a[i] /= l; return true; };
  if (!unit3(up)) { lockSkips.fetch_add(1); return; }
  if (!lockValid) { std::memcpy(lockRight, cur, 12); lockValid = true; return; }
  float r[3]; std::memcpy(r, lockRight, 12); float d = r[0] * up[0] + r[1] * up[1] + r[2] * up[2]; for (int i = 0; i < 3; ++i) r[i] -= d * up[i];
  if (!unit3(r)) { std::memcpy(lockRight, cur, 12); lockSkips.fetch_add(1); return; }
  std::memcpy(lockRight, r, 12);
  std::memcpy(reinterpret_cast<void*>(rdx + 0x20), r, 12);
  lockWrites.fetch_add(1, std::memory_order_relaxed);
}
// M6t: setter flags. The ActorRelativeCamera reference (research hint) calls this setter with R9 = 3 to "preserve the
// current view" and the vanilla actor-relative call passes R9 = 0. HYPOTHESIS: flag bits 0/1 keep the current view
// yaw/pitch when the reference changes, i.e. no re-aim (no auto-centering). Auto-centering OFF -> R9 |= 3.
constexpr bool kSetterPreserveFlags = true, kSetterYawLock = false; // M6s yaw lock REJECTED (centering unchanged)
// M6z: rate-limited camera up (supersedes the M6y exit-only blend, which rarely triggered). The actor-relative
// reference follows the hero up, which turns ~90 deg in 0.1-0.2 s at WS entry/exit. Every setter call the up input
// (r8) is replaced with a camera up that turns toward the game's up at <= kCamUpDegPerSec. The camera stays engaged
// after WS until that up is back to world up (or 3 s). Preserve flags keep the world view during the turn.
constexpr float kCamUpDegPerSec = 150.f; constexpr std::uint64_t kExitBlendMs = 3000;
float camUp[3]{}; bool camUpValid{}; double camUpLastT{}; std::atomic<bool> camUpSettled{true}, camUpTurning{false};
void exitBlend(std::uintptr_t r8) noexcept {
  if (!setYawLock.load(std::memory_order_relaxed) || killed.load() || r8 < 0x10000 || r8 > UINTPTR_MAX - 16) return;
  float tgt[3];
  if (!msm2::readMemory(r8, tgt, sizeof tgt) || !std::isfinite(tgt[0]) || !std::isfinite(tgt[1]) || !std::isfinite(tgt[2])) return;
  float tl = std::sqrt(tgt[0] * tgt[0] + tgt[1] * tgt[1] + tgt[2] * tgt[2]); if (!(tl > 0.5f && tl < 1.5f)) return; for (float& c : tgt) c /= tl;
  if (exitBlendStart.load(std::memory_order_relaxed)) { tgt[0] = 0; tgt[1] = 1; tgt[2] = 0; } // WS over: head for world up
  LARGE_INTEGER q{}, f{}; QueryPerformanceCounter(&q); QueryPerformanceFrequency(&f); double t = double(q.QuadPart) / double(f.QuadPart), dt = camUpLastT > 0 ? t - camUpLastT : 0.; camUpLastT = t;
  if (!camUpValid || dt > 0.5) { camUp[0] = 0; camUp[1] = 1; camUp[2] = 0; camUpValid = true; } // engage starts from world up
  float d = std::fmax(-1.f, std::fmin(1.f, camUp[0] * tgt[0] + camUp[1] * tgt[1] + camUp[2] * tgt[2])), ang = std::acos(d);
  float mx = float(kCamUpDegPerSec * 3.14159265f / 180.f * std::fmin(dt, 0.1));
  camUpTurning.store(ang > mx + 0.01f, std::memory_order_relaxed);
  if (ang <= mx || ang < 1e-4f) std::memcpy(camUp, tgt, sizeof camUp);
  else { float k = mx / ang; float v[3]; for (int i = 0; i < 3; ++i) v[i] = camUp[i] + (tgt[i] - camUp[i]) * k; // small-step approach
    float l = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); if (!(l > 1e-3f)) return; for (int i = 0; i < 3; ++i) camUp[i] = v[i] / l; }
  camUpSettled.store(camUp[1] > 0.9998f, std::memory_order_relaxed);
  std::memcpy(reinterpret_cast<void*>(r8), camUp, sizeof camUp); exitBlendWrites.fetch_add(1, std::memory_order_relaxed);
}
// M6v: angle hold. Disassembly of the setter (owner camera_code.json, cloud decode, not committed): rdx = forward input,
// r8 = up input, r9 = flags; the camera's view angles are the mover's floats +0x2CC (yaw) and +0x2D0 (pitch, negated)
// in the reference frame; flag 1/2 recompute them so the world view is kept when the reference changes. The WS input
// forward (+0x00 of the block) is constant, so the reference does not turn with the hero; the centering is the follow
// logic moving +0x2CC/+0x2D0 toward the travel. Auto-centering OFF: while neither the right stick nor keyboard/mouse
// input is active, undo any change of the two angles since the previous setter call.
constexpr bool kAngleHold = true; float holdYaw{}, holdPitch{}; bool holdAngles{}; std::uintptr_t holdMover{}; std::uint64_t holdMs{};
void angleHold(std::uintptr_t mover) noexcept {
  auto now = GetTickCount64(); if (now - holdMs > 300 || mover != holdMover) holdAngles = false; holdMs = now; holdMover = mover;
  if (!kAngleHold || !setYawLock.load(std::memory_order_relaxed) || ws::drive::cameraAutoCenter.load(std::memory_order_relaxed) || !engagedManager.load() || killed.load()) { holdAngles = false; return; }
  if (!mover || mover > UINTPTR_MAX - 0x400) return;
  float a[2]; if (!msm2::readMemory(mover + 0x2CC, a, sizeof a) || !std::isfinite(a[0]) || !std::isfinite(a[1]) || std::fabs(a[0]) > 100.f || std::fabs(a[1]) > 10.f) { holdAngles = false; return; }
  float rx = 0, ry = 0, th = ws::settings::stickManualThreshold.load(std::memory_order_relaxed); bool stick = msm2::writer::readRightStickInput(rx, ry) && rx * rx + ry * ry > th * th;
  // M6w: GetLastInputInfo marked ~85% of WS calls as manual in the owner's controller-only run (M6v), so the hold
  // rarely acted. Controller right stick only for now; mouse look detection is a separate later step.
  bool kbm = msm2::writer::mouseMovedWithin(ws::settings::mouseManualMs.load(std::memory_order_relaxed)); // M7a mouse look
  if (!holdAngles || stick || kbm) { holdYaw = a[0]; holdPitch = a[1]; holdAngles = true; if (stick || kbm) angleManual.fetch_add(1, std::memory_order_relaxed); return; }
  if (a[0] == holdYaw && a[1] == holdPitch) return;
  MEMORY_BASIC_INFORMATION mi{}; if (!VirtualQuery(reinterpret_cast<void*>(mover + 0x2CC), &mi, sizeof mi) || mi.State != MEM_COMMIT || !(mi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE))) return;
  float w[2]{holdYaw, holdPitch}; std::memcpy(reinterpret_cast<void*>(mover + 0x2CC), w, sizeof w);
  angleHoldWrites.fetch_add(1, std::memory_order_relaxed);
}
extern "C" std::uintptr_t ws_camset_record(std::uintptr_t rcx, std::uintptr_t rdx, std::uintptr_t r8, std::uintptr_t r9) noexcept {
  if (kSetterYawLock) yawLock(rdx);
  exitBlend(r8);
  if (!exitBlendStart.load(std::memory_order_relaxed) && !camUpTurning.load(std::memory_order_relaxed) && !gbAirCam.load(std::memory_order_relaxed)) angleHold(rcx); // the hold would undo the re-expressed angles while the up turns; M10p: off while airborne in wall gravity (re-centering)
  setR9.store(r9, std::memory_order_relaxed);
  std::uintptr_t outR9 = r9;
  if (kSetterPreserveFlags && setYawLock.load(std::memory_order_relaxed) && !ws::drive::cameraAutoCenter.load(std::memory_order_relaxed) && engagedManager.load() && !killed.load()) {
    outR9 = r9 | 3; if (outR9 != r9) flagWrites.fetch_add(1, std::memory_order_relaxed); }
  setCalls.fetch_add(1, std::memory_order_relaxed); setRcx.store(rcx, std::memory_order_relaxed); setRdx.store(rdx, std::memory_order_relaxed); setR8.store(r8, std::memory_order_relaxed);
  unsigned ok = 0; std::uintptr_t ptr[2]{rdx, r8};
  for (int k = 0; k < 2; ++k) { float m[16]; if (ptr[k] > 0x10000 && ptr[k] < UINTPTR_MAX - 64 && msm2::readMemory(ptr[k], m, sizeof m)) { ok |= 1u << k; for (int i = 0; i < 16; ++i) setM[k][i].store(m[i], std::memory_order_relaxed); } }
  setMOk.store(ok, std::memory_order_relaxed);
  return outR9;
}
namespace {
// M6x diagnostic (read-only): the camera jump on leaving WS. For each exit (restore of +0x304), log ~1.5 s at the
// 100 ms worker rate: mover (setter rcx) reference matrix +0x210 (3 rows), view angles +0x2CC/+0x2D0, manager +0x304,
// camera forward +0xD44 (world) and hero up. Also the last 3 samples before the exit. Max 6 exits.
struct ExitSample { std::uint64_t ms; float ref[9], yaw, pitch, fwd[3], up[3]; unsigned char flag; bool ok; };
ExitSample exitRing[3]{}; unsigned exitRingN{}; std::uint64_t exitLoggedEpisode{}, exitLastMs{}; unsigned exitLines{}, exitEpisodesLogged{};
bool exitSample(ExitSample& e, const msm2::Snapshot& hero, std::uint64_t now) {
  e = {}; e.ms = now; auto mover = setRcx.load(); auto m = ws_camref_this;
  if (!mover || mover > UINTPTR_MAX - 0x400 || !msm2::readMemory(mover + 0x210, e.ref, sizeof e.ref)) return false;
  float a[2]; if (!msm2::readMemory(mover + 0x2CC, a, sizeof a)) return false; e.yaw = a[0]; e.pitch = a[1];
  if (m && m <= UINTPTR_MAX - 0x1000) { msm2::readMemory(m + 0xD44, e.fwd, 12); msm2::readMemory(m + kActorRelativeFlag, &e.flag, 1); }
  auto ax = ws::diag::actorAxes(hero.prefix); if (ax.valid) { e.up[0] = ax.up.x; e.up[1] = ax.up.y; e.up[2] = ax.up.z; }
  e.ok = true; return true;
}
void exitLog(const char* phase, const ExitSample& e, std::uint64_t ep, std::uint64_t t0) {
  msm2::writeLog("WS_CAMERA_EXIT episode=%llu phase=%s t_ms=%lld flag304=%u yaw=%.4f pitch=%.4f ref_r0=%.3f,%.3f,%.3f ref_r1=%.3f,%.3f,%.3f ref_r2=%.3f,%.3f,%.3f view=%.3f,%.3f,%.3f hero_up=%.3f,%.3f,%.3f read_only=1",
    (unsigned long long)ep, phase, (long long)e.ms - (long long)t0, unsigned(e.flag), e.yaw, e.pitch, e.ref[0], e.ref[1], e.ref[2], e.ref[3], e.ref[4], e.ref[5], e.ref[6], e.ref[7], e.ref[8],
    e.fwd[0], e.fwd[1], e.fwd[2], e.up[0], e.up[1], e.up[2]);
}
void m6Exit(const msm2::Snapshot& hero, std::uint64_t now) {
  if (now - exitLastMs < 90) { return; }
  exitLastMs = now;
  auto ep = exitEpisode.load(), t0 = exitRestoreMs.load();
  ExitSample e; bool ok = exitSample(e, hero, now);
  if (engagedManager.load()) { if (ok) { exitRing[exitRingN % 3] = e; ++exitRingN; } return; }
  if (!ep || now - t0 > 1500 || exitEpisodesLogged > 6) return;
  if (ep != exitLoggedEpisode) { exitLoggedEpisode = ep; exitLines = 0; ++exitEpisodesLogged; if (exitEpisodesLogged > 6) return;
    for (unsigned k = exitRingN > 3 ? exitRingN - 3 : 0; k < exitRingN; ++k) { exitLog("before", exitRing[k % 3], ep, t0); }
    exitRingN = 0; }
  if (ok && exitLines < 18) { ++exitLines; exitLog("after", e, ep, t0); }
}
}
namespace {
// M8b diagnostic (read-only): hero velocity after WS ends (jump), split along the last WS wall normal n, world up y
// and the horizontal wall axis h = up x n, to measure the WR jump-off vector. 100 ms worker, <= 10 episodes x 14 lines.
std::atomic<float> m9N[3]{}; std::atomic<std::uintptr_t> m9Hero{0}; std::atomic<std::uint64_t> m9NMs{0};
ws::Vec3 jtN{}, jtPos{}, jtVel{}; bool jtWs{}, jtHave{}; std::uint64_t jtEnd{}; unsigned jtEpisodes{}, jtLines{};
void m8JumpTrace(const msm2::Snapshot& hero, const ws::diag::Motion& motion, std::uint64_t now) {
  auto axes = ws::diag::actorAxes(hero.prefix);
  bool ws = engagedManager.load() != 0 && now - lastWsMs.load() < 150;
  ws::Vec3 pos{hero.position[0], hero.position[1], hero.position[2]};
  if (ws) {
    if (hero.hero && axes.valid && std::fabs(axes.up.y) < 0.3f) { jtN = axes.up; jtPos = pos; jtVel = motion.valid ? motion.velocity : ws::Vec3{}; jtHave = true;
      m9N[0].store(axes.up.x); m9N[1].store(axes.up.y); m9N[2].store(axes.up.z); m9Hero.store(hero.hero); m9NMs.store(now); }
    jtWs = true; return;
  }
  if (jtWs) { jtWs = false; if (jtHave && jtEpisodes < 60) { jtEnd = lastWsMs.load(); jtLines = 0; ++jtEpisodes; } else jtEnd = 0; }
  if (!jtEnd || !jtHave || now - jtEnd > 1300 || jtLines >= 14 || !hero.hero) return;
  ws::Vec3 n = jtN, h{n.z, 0.f, -n.x}; float hl = std::sqrt(h.x * h.x + h.z * h.z); if (hl < 1e-3f) return; h = h * (1.f / hl);
  auto split = [&](const ws::Vec3& v, float out[3]) { out[0] = ws::dot(v, n); out[1] = v.y; out[2] = ws::dot(v, h); };
  float v[3]{}, d[3]{}, v0[3]{}; split(motion.valid ? motion.velocity : ws::Vec3{}, v); split(pos - jtPos, d); split(jtVel, v0);
  if (!jtLines) msm2::writeLog("WS_JUMP_TRACE episode=%u phase=ws_last n=%.3f,%.3f,%.3f vel_n=%.2f vel_y=%.2f vel_h=%.2f read_only=1", jtEpisodes, n.x, n.y, n.z, v0[0], v0[1], v0[2]);
  ++jtLines;
  msm2::writeLog("WS_JUMP_TRACE episode=%u phase=after t_ms=%llu valid=%u vel_n=%.2f vel_y=%.2f vel_h=%.2f disp_n=%.2f disp_y=%.2f disp_h=%.2f hero_up=%.3f,%.3f,%.3f read_only=1",
    jtEpisodes, (unsigned long long)(now - jtEnd), unsigned(motion.valid), v[0], v[1], v[2], d[0], d[1], d[2], axes.up.x, axes.up.y, axes.up.z);
}
}
namespace {
// M8c (2026-11-06): WS-only jump config. M8b A/B (analysis/M8_JUMP_PLAN_20261106.md) VERIFIED that
// WallRunJumpSetupConfig JumpOffData YBoost is the world-up speed added at a WR jump-off (vanilla ~10 m/s, 0 -> ~0.4).
// Instead of shipping a globally patched file, the loaded config is found in memory and the YBoost values are set to 0
// only while WS is active; they are restored 1.5 s after WS ends (after the jump-off read), on F8 and on stop.
// M8c-r4 runtime layout (VERIFIED-RUNTIME, one copy): each JumpOffData is 9 floats {SpeedMin, SpeedMax, GenBoostMin,
// GenBoostMax, GenBoostCap, YBoostMin, YBoostMax, TimeToPeakMin, TimeToPeakMax}; _00/_45/_90 are 48 bytes apart.
// Identity: all 27 vanilla floats, bit-exact. 1..4 matches accepted; every write re-checks the 27 floats.
constexpr float kJumpF[3][9] = {{8, 20, 3, 6, 30, 9, 14, 0.42f, 0.65f}, {8, 20, 3, 6, 30, 6, 12, 0.42f, 0.7f}, {10, 30, 4, 8, 28, 2, 2, 0.5f, 0.8f}};
constexpr unsigned kJumpStride = 48, kYMin = 5, kYMax = 6;
std::atomic<int> jcState{0};             // 0 idle, 1 scanning, 2 accepted, 3 refused
std::uintptr_t jcAddr[4]{}; unsigned jcCount{}; bool jcApplied{}; std::uint64_t jcWrites{}, jcRestores{}, jcConflicts{};
bool jcGroupsEqual(const float (&cur)[3][9], bool patched) {
  for (unsigned k = 0; k < 3; ++k) for (unsigned j = 0; j < 9; ++j) {
    if (patched && j < 5) continue;   // M8j/M9d: Speed* and GenBoost* may hold a charge value while patched
    float want = (patched && (j == kYMin || j == kYMax)) ? 0.f : kJumpF[k][j];
    if (std::memcmp(&cur[k][j], &want, 4)) return false; }
  return true;
}
bool jcMatches(std::uintptr_t a, bool patched) {
  float cur[3][9]; for (unsigned k = 0; k < 3; ++k) if (!msm2::readMemory(a + k * kJumpStride, cur[k], 36)) return false;
  return jcGroupsEqual(cur, patched);
}
// M10g (2026-11-07, read-only): the M10e/M10f block is the hero gravity ORIENTATION; the fall acceleration (~31-35
// m/s^2) comes from elsewhere. During the jump-config scan, also count float3 (0, -g, 0) with g in [20, 60] (exact
// zeros in x/z) per distinct g: a world/hero gravity vector would show as a few copies of one value.
struct GvBin { float g; std::uint32_t count; std::uintptr_t addr[4]; DWORD type[4]; };
GvBin gvBins[48]{}; unsigned gvBinN{}; std::uint64_t gvOverflow{};
void gvRecord(float g, std::uintptr_t a, DWORD type) {
  for (unsigned i = 0; i < gvBinN; ++i) if (gvBins[i].g == g) { auto& b = gvBins[i]; if (b.count < 4) { b.addr[b.count] = a; b.type[b.count] = type; } ++b.count; return; }
  if (gvBinN < 48) { auto& b = gvBins[gvBinN++]; b.g = g; b.count = 1; b.addr[0] = a; b.type[0] = type; } else ++gvOverflow;
}
DWORD WINAPI jcScan(void*) {
  struct WsEx { void* va; ULONG_PTR attr; };
  using QueryWsFn = BOOL (WINAPI*)(HANDLE, PVOID, DWORD);
  auto queryWs = reinterpret_cast<QueryWsFn>(reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "K32QueryWorkingSetEx")));
  auto t0 = GetTickCount64(); std::uint64_t bytes = 0, regions = 0, nonResident = 0, mapped = 0, firstHits = 0; unsigned found = 0; std::uintptr_t hits[4]{};
  // The buffer and this DLL's own image are skipped so copies made by the scan itself are never matched.
  constexpr std::size_t kChunk = std::size_t(8) << 20;
  auto* buf = static_cast<unsigned char*>(VirtualAlloc(nullptr, kChunk, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
  if (!buf) { jcState.store(3); msm2::writeLog("WS_JUMP_CFG_SCAN result=refused reason=no_buffer"); return 0; }
  HMODULE self = nullptr; GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCWSTR>(&jcScan), &self);
  SYSTEM_INFO si{}; GetSystemInfo(&si);
  auto p = reinterpret_cast<std::uintptr_t>(si.lpMinimumApplicationAddress), end = reinterpret_cast<std::uintptr_t>(si.lpMaximumApplicationAddress);
  std::uint32_t first; std::memcpy(&first, &kJumpF[0][0], 4);
  bool timeout = false;
  while (p < end && !killed.load()) {
    MEMORY_BASIC_INFORMATION m{}; if (!VirtualQuery(reinterpret_cast<void*>(p), &m, sizeof m)) break;
    auto base = reinterpret_cast<std::uintptr_t>(m.BaseAddress), next = base + m.RegionSize; if (next <= p) break;
    // History: r1/r2 timed out (low priority, quadratic search, non-resident reads); r3 found no raw u8 copy; r4 found
    // the float struct. Readable non-image memory, working-set-resident pages only, aligned 4-byte search.
    const DWORD prot = m.Protect & 0xff;
    const bool readable = prot == PAGE_READWRITE || prot == PAGE_READONLY || prot == PAGE_WRITECOPY;
    if (m.State == MEM_COMMIT && m.Type != MEM_IMAGE && readable && !(m.Protect & PAGE_GUARD) &&
        m.AllocationBase != static_cast<void*>(buf) && m.AllocationBase != static_cast<void*>(self)) {
      ++regions; if (m.Type != MEM_PRIVATE) mapped += m.RegionSize; const DWORD mbType = m.Type;
      for (std::uintptr_t q = base; q < next && !killed.load(); ) {
        std::size_t n = std::size_t(std::min<std::uintptr_t>(kChunk, next - q));
        std::size_t pages = (n + 4095) / 4096; static WsEx ws[2048];
        for (std::size_t k = 0; k < pages; ++k) { ws[k].va = reinterpret_cast<void*>(q + k * 4096); ws[k].attr = 0; }
        bool wsOk = queryWs && queryWs(GetCurrentProcess(), ws, DWORD(pages * sizeof(WsEx)));
        for (std::size_t k = 0; k < pages; ) {
          if (wsOk && !(ws[k].attr & 1)) { nonResident += 4096; ++k; continue; }
          std::size_t k2 = k; while (k2 < pages && (!wsOk || (ws[k2].attr & 1))) ++k2;
          auto runAddr = q + k * 4096; std::size_t runLen = std::min<std::size_t>((k2 - k) * 4096, n - k * 4096);
          if (msm2::readMemory(runAddr, buf, runLen)) { bytes += runLen;
            for (std::size_t w = 0; w + 36 <= runLen; w += 4) {
              std::uint32_t x; std::memcpy(&x, buf + w, 4);
              if ((x & 0x7fffffffu) == 0 && w + 12 <= runLen) { float g3[3]; std::memcpy(g3, buf + w, 12);   // M10g: (0, -g, 0)
                std::uint32_t zb; std::memcpy(&zb, buf + w + 8, 4);
                if ((zb & 0x7fffffffu) == 0 && g3[1] <= -20.f && g3[1] >= -60.f) gvRecord(g3[1], runAddr + w, mbType); }
              if (x != first) continue;
              ++firstHits; auto a = runAddr + w;
              if (std::memcmp(buf + w, kJumpF[0], 36)) continue;                       // group _00 in the buffer
              if (w + 2 * kJumpStride + 36 <= runLen && (std::memcmp(buf + w + kJumpStride, kJumpF[1], 36) || std::memcmp(buf + w + 2 * kJumpStride, kJumpF[2], 36))) continue;
              bool dup = false; for (unsigned j = 0; j < found; ++j) dup |= hits[j] == a;
              if (!dup && found < 4 && jcMatches(a, false)) hits[found++] = a;
            } }
          k = k2;
        }
        q += n;
        if (GetTickCount64() - t0 > 300000) { timeout = true; break; }
      }
    }
    if (timeout) break;
    p = next;
  }
  VirtualFree(buf, 0, MEM_RELEASE);
  for (unsigned i = 0; i < found; ++i) jcAddr[i] = hits[i];
  jcCount = found;
  bool ok = found >= 1 && found <= 4 && !timeout;
  MEMORY_BASIC_INFORMATION hm{}; bool hq = found && VirtualQuery(reinterpret_cast<void*>(hits[0]), &hm, sizeof hm);
  msm2::writeLog("WS_JUMP_CFG_SCAN result=%s layout=float27 matches=%u first_float_hits=%llu regions=%llu mib=%llu nonresident_mib=%llu mapped_mib=%llu ms=%llu timeout=%u addr0=0x%llx addr1=0x%llx addr0_type=0x%lx addr0_protect=0x%lx",
    ok ? "accepted" : "refused", found, (unsigned long long)firstHits, (unsigned long long)regions, (unsigned long long)(bytes >> 20),
    (unsigned long long)(nonResident >> 20), (unsigned long long)(mapped >> 20), (unsigned long long)(GetTickCount64() - t0), unsigned(timeout),
    (unsigned long long)hits[0], (unsigned long long)hits[1], hq ? (unsigned long)hm.Type : 0ul, hq ? (unsigned long)hm.Protect : 0ul);
  for (unsigned i = 0; i < gvBinN; ++i) { auto& b = gvBins[i]; if (b.count > 200) continue;   // very common values are not a single gravity
    msm2::writeLog("WS_GRAVITY_VECTOR g=%.4f count=%u a0=0x%llx t0=0x%lx a1=0x%llx t1=0x%lx a2=0x%llx a3=0x%llx read_only=1", double(b.g), b.count,
      (unsigned long long)b.addr[0], (unsigned long)b.type[0], (unsigned long long)b.addr[1], (unsigned long)b.type[1], (unsigned long long)b.addr[2], (unsigned long long)b.addr[3]); }
  msm2::writeLog("WS_GRAVITY_VECTOR_SUMMARY distinct=%u overflow=%llu read_only=1", gvBinN, (unsigned long long)gvOverflow);
  jcState.store(ok ? 2 : 3);
  return 0;
}
// M9d: M8j-r3 showed the jump-off capped near ReleaseData.InSpeedMax 22. Charge now scales Speed*, GenBoost* (incl. Cap)
// of every group and InSpeedMax. Runtime ReleaseData (M8c-r4 float context): {InSpeedMax 22, TimeToPeakMin 0.45,
// TimeToPeakMax 0.8} at group_00 - 32; identity checked (InSpeedMax may hold the charge value while patched).
constexpr float kRelease[3] = {22.f, 0.45f, 0.8f}; constexpr int kReleaseOff = -32;
bool jcReleaseOk(std::uintptr_t a) { float r[3]; if (!msm2::readMemory(a + kReleaseOff, r, 12)) return false; return !std::memcmp(&r[1], &kRelease[1], 8) && std::isfinite(r[0]) && r[0] >= 22.f && r[0] <= 22.f * 5.f; }
std::atomic_flag jcLock = ATOMIC_FLAG_INIT;   // tick vs F8/stop restore
void jcWriteAll(bool patch) {
  while (jcLock.test_and_set(std::memory_order_acquire)) Sleep(0);
  unsigned done = 0, conflict = 0;
  for (unsigned i = 0; i < jcCount; ++i) {
    auto a = jcAddr[i];
    if (!jcMatches(a, !patch)) { ++conflict; continue; }
    bool w = true; for (unsigned k = 0; k < 3; ++k) w = w && writableByte(a + k * kJumpStride) && writableByte(a + k * kJumpStride + 4 * kYMax + 3);
    if (!w) { ++conflict; continue; }
    for (unsigned k = 0; k < 3; ++k) { auto* f = reinterpret_cast<volatile float*>(a + k * kJumpStride);
      f[kYMin] = patch ? 0.f : kJumpF[k][kYMin]; f[kYMax] = patch ? 0.f : kJumpF[k][kYMax];
      if (!patch) { for (unsigned j = 0; j < 5; ++j) f[j] = kJumpF[k][j]; } }
    if (!patch && jcReleaseOk(a) && writableByte(a + kReleaseOff)) *reinterpret_cast<volatile float*>(a + kReleaseOff) = kRelease[0];
    ++done;
  }
  jcConflicts += conflict; (patch ? jcWrites : jcRestores) += done;
  if (jcWrites + jcRestores <= 40 || conflict) msm2::writeLog("WS_JUMP_CFG_WRITE op=%s copies=%u conflicts=%u writes=%llu restores=%llu",
    patch ? "yboost_0" : "restore_vanilla", done, conflict, (unsigned long long)jcWrites, (unsigned long long)jcRestores);
  jcLock.clear(std::memory_order_release);
}
// M8j: charge jump strength. SpeedMin/Max of every group = vanilla * k, only while the YBoost patch is applied
// (restored with it). Every write re-checks the patched identity.
// M9e: field attribution. Each charge scales one field group only, cycling: 0 = InSpeedMax, 1 = Speed*, 2 = GenBoost*.
std::atomic<unsigned> jcVariantN{0}; std::atomic<int> jcLastVariant{-1};
bool jcSetSpeed(float k) {
  if (jcState.load() != 2 || !jcApplied) return false;
  int variant = int(jcVariantN.fetch_add(1) % 3); jcLastVariant.store(variant);
  while (jcLock.test_and_set(std::memory_order_acquire)) Sleep(0);
  unsigned done = 0;
  for (unsigned i = 0; i < jcCount; ++i) { auto a = jcAddr[i];
    if (!jcMatches(a, true) || !jcReleaseOk(a) || !writableByte(a + kReleaseOff) || !writableByte(a + 2 * kJumpStride + 35)) continue;
    for (unsigned g = 0; g < 3; ++g) { auto* f = reinterpret_cast<volatile float*>(a + g * kJumpStride);
      for (unsigned j = 0; j < 5; ++j) f[j] = kJumpF[g][j] * (((variant == 1 && j < 2) || (variant == 2 && j >= 2)) ? k : 1.f); }
    *reinterpret_cast<volatile float*>(a + kReleaseOff) = kRelease[0] * (variant == 0 ? k : 1.f);
    ++done; }
  jcLock.clear(std::memory_order_release);
  return done > 0;
}
// M8d diagnostic (read-only): does a WS jump start on button press or on release? A 5 ms poller records jump-button
// (XInput A / Space) edges; at each WS end it logs the last press/release relative to the end. <= 30 lines.
std::atomic<bool> jiStarted{false};
std::atomic<std::uint64_t> jiPress{0}, jiRelease{0};
DWORD WINAPI jiPoll(void*) {
  bool prev = false, wasWs = false; std::uint64_t press = 0, release = 0; unsigned logs = 0;
  while (!killed.load()) {
    auto now = GetTickCount64(); bool b = msm2::writer::readJumpButton();
    if (b && !prev) { press = now; jiPress.store(now); }
    if (!b && prev) { release = now; jiRelease.store(now); }
    prev = b;
    if (logs >= 30) { wasWs = false; Sleep(5); continue; }
    bool ws = engagedManager.load() != 0 && now - lastWsMs.load() < 150;
    if (wasWs && !ws) { auto end = lastWsMs.load(); ++logs;
      msm2::writeLog("WS_JUMP_INPUT ws_end held_now=%u press_before_end_ms=%lld release_before_end_ms=%lld read_only=1", unsigned(b),
        press ? (long long)end - (long long)press : -1ll, (release && release > press) ? (long long)end - (long long)release : -1ll); }
    wasWs = ws; Sleep(5);
  }
  return 0;
}
// M8e/M8f diagnostics (read-only, pass-through). M8e: xinput9_1_0 got only ~16 calls/s and no XInputSetState, so it
// is not the game's gameplay pad path. M8f hooks XInputGetState/SetState in every XInput DLL (1_4, 1_3, 9_1_0, uap),
// counts only calls from outside this DLL (WS itself reads xinput1_4), records the A button as each module returns it
// and logs rumble on change. Module list is logged at 30 s and 120 s (Windows.Gaming.Input / GameInput / HID).
using XGetFn = DWORD (WINAPI*)(DWORD, XINPUT_STATE*); using XSetFn = DWORD (WINAPI*)(DWORD, XINPUT_VIBRATION*);
constexpr const wchar_t* kXMods[4] = {L"xinput1_4.dll", L"xinput1_3.dll", L"xinput9_1_0.dll", L"xinputuap.dll"};
XGetFn xgOrig[4]{}; XSetFn xsOrig[4]{}; void* xgTarget[4]{}; void* xsTarget[4]{}; std::atomic<bool> xTried[4]{};
std::atomic<std::uint64_t> xgCalls[4]{}, xgPresses[4]{}, xsCalls[4]{}, xgPress{0}; std::atomic<bool> xgA[4]{};
std::atomic<unsigned> xsLogs{0}, xsLast{0xffffffffu}, xaLogs{0}; std::atomic<std::uint64_t> xhBlocked{0};
std::uintptr_t selfLo{}, selfHi{};
bool fromSelf(void* ret) { auto r = reinterpret_cast<std::uintptr_t>(ret); return r >= selfLo && r < selfHi; }
template <int M> DWORD WINAPI xgDetour(DWORD i, XINPUT_STATE* st) {
  DWORD r = xgOrig[M](i, st);
  if (fromSelf(__builtin_return_address(0))) return r;
  xgCalls[M].fetch_add(1, std::memory_order_relaxed);
  if (r == ERROR_SUCCESS && st && i == 0) {
    bool a = (st->Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
    // M8g experiment: while WS is active, the game does not see A from XInput. If the WS jump still fires, the game's
    // gameplay pad path is not XInput. INI WALLSPRINT_JUMP_HOLDBACK_TEST (default 0, shipped 1), F8 stops it.
    if (a && ws::drive::jumpHoldbackTest.load() && !killed.load() && engagedManager.load() != 0 && GetTickCount64() - lastWsMs.load() < 150) {
      st->Gamepad.wButtons = WORD(st->Gamepad.wButtons & ~XINPUT_GAMEPAD_A);
      if (xhBlocked.fetch_add(1) < 40) msm2::writeLog("WS_JUMP_HOLDBACK module=%ls a_hidden=1 blocked=%llu", kXMods[M], (unsigned long long)xhBlocked.load()); }
    if (a && !xgA[M].exchange(true)) { auto now = GetTickCount64(); xgPress.store(now); xgPresses[M].fetch_add(1);
      if (xaLogs.fetch_add(1) < 40) { auto kp = jiPress.load(); msm2::writeLog("WS_INPUT_GAME_A module=%ls since_ws_seen_press_ms=%lld read_only=1", kXMods[M], kp ? (long long)(now - kp) : -1ll); } }
    else if (!a) xgA[M].store(false); }
  return r;
}
template <int M> DWORD WINAPI xsDetour(DWORD i, XINPUT_VIBRATION* v) {
  if (!fromSelf(__builtin_return_address(0))) { xsCalls[M].fetch_add(1, std::memory_order_relaxed);
    if (v && i == 0) { unsigned key = (unsigned(v->wLeftMotorSpeed) << 16) | v->wRightMotorSpeed;
      if (xsLast.exchange(key) != key && xsLogs.fetch_add(1) < 400) { auto now = GetTickCount64(); auto p = xgPress.load(), kp = jiPress.load();
        msm2::writeLog("WS_RUMBLE module=%ls left=%u right=%u since_game_A_press_ms=%lld since_jump_press_ms=%lld read_only=1", kXMods[M], unsigned(v->wLeftMotorSpeed), unsigned(v->wRightMotorSpeed),
          p ? (long long)(now - p) : -1ll, kp ? (long long)(now - kp) : -1ll); } } }
  return xsOrig[M](i, v);
}
template <int M> void xHookOne() {
  if (xTried[M].load()) return;
  HMODULE x = GetModuleHandleW(kXMods[M]); if (!x) return;
  xTried[M].store(true);
  auto g = reinterpret_cast<void*>(GetProcAddress(x, "XInputGetState")), sv = reinterpret_cast<void*>(GetProcAddress(x, "XInputSetState"));
  bool okG = g && MH_CreateHook(g, reinterpret_cast<void*>(&xgDetour<M>), reinterpret_cast<void**>(&xgOrig[M])) == MH_OK && MH_EnableHook(g) == MH_OK;
  bool okS = sv && MH_CreateHook(sv, reinterpret_cast<void*>(&xsDetour<M>), reinterpret_cast<void**>(&xsOrig[M])) == MH_OK && MH_EnableHook(sv) == MH_OK;
  if (okG) { xgTarget[M] = g; }
  if (okS) { xsTarget[M] = sv; }
  msm2::writeLog("WS_INPUT_HOOK module=%ls get=%u set=%u pass_through=1", kXMods[M], unsigned(okG), unsigned(okS));
}
void m8InputHooks(std::uint64_t now) {
  static std::uint64_t lastTry = 0, lastLog = 0, start = 0; static unsigned logs = 0; static bool mod30 = false, mod120 = false;
  if (!start) { start = now; HMODULE self = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCWSTR>(&m8InputHooks), &self)) {
      MEMORY_BASIC_INFORMATION mb{}; auto lo = reinterpret_cast<std::uintptr_t>(self); std::uintptr_t q = lo;
      while (VirtualQuery(reinterpret_cast<void*>(q), &mb, sizeof mb) && mb.AllocationBase == static_cast<void*>(self)) q = reinterpret_cast<std::uintptr_t>(mb.BaseAddress) + mb.RegionSize;
      selfLo = lo; selfHi = q; } }
  if (now - lastTry > 2000) { lastTry = now; xHookOne<0>(); xHookOne<1>(); xHookOne<2>(); xHookOne<3>(); }
  if ((!mod30 && now - start > 30000) || (!mod120 && now - start > 120000)) { (mod30 ? mod120 : mod30) = true;
    const wchar_t* mods[] = {L"Windows.Gaming.Input.dll", L"GameInput.dll", L"hid.dll", L"dinput8.dll", L"xinput1_4.dll", L"xinput1_3.dll", L"xinput9_1_0.dll", L"xinputuap.dll", L"gameoverlayrenderer64.dll", L"steam_api64.dll", L"SDL2.dll", L"SDL3.dll"};
    char line[600]{}; int len = 0; for (auto m : mods) if (GetModuleHandleW(m) && len < 520) len += std::snprintf(line + len, sizeof line - std::size_t(len), "%s%ls", len ? "," : "", m);
    msm2::writeLog("WS_INPUT_MODULES t_s=%llu loaded=%s", (unsigned long long)((now - start) / 1000), line); }
  if (now - lastLog >= 10000 && logs < 30) { lastLog = now; ++logs;
    msm2::writeLog("WS_INPUT_HOOK_STATUS get_1_4=%llu get_1_3=%llu get_9_1_0=%llu get_uap=%llu presses_1_4=%llu presses_9_1_0=%llu set_1_4=%llu set_1_3=%llu set_9_1_0=%llu set_uap=%llu",
      (unsigned long long)xgCalls[0].load(), (unsigned long long)xgCalls[1].load(), (unsigned long long)xgCalls[2].load(), (unsigned long long)xgCalls[3].load(),
      (unsigned long long)xgPresses[0].load(), (unsigned long long)xgPresses[2].load(), (unsigned long long)xsCalls[0].load(), (unsigned long long)xsCalls[1].load(),
      (unsigned long long)xsCalls[2].load(), (unsigned long long)xsCalls[3].load()); }
}
// M8h (2026-11-07): M8g VERIFIED that hiding A from XInput during WS does not stop the WS jump, so gameplay pad input
// is not XInput. Windows.Gaming.Input is loaded. Pass-through hooks on the WGI Gamepad implementation (IGamepad vtable
// slots 7 put_Vibration and 8 GetCurrentReading, found through our own Gamepad object): call counts, A edges, rumble
// values on change; and, with WALLSPRINT_JUMP_HOLDBACK_TEST, A (GamepadButtons 0x4) hidden while WS is active.
struct WgiReading { std::uint64_t ts; std::uint32_t buttons; double lt, rt, lx, ly, rx, ry; };
struct WgiVibration { double left, right, ltrig, rtrig; };
using WgiReadFn = HRESULT (STDMETHODCALLTYPE*)(void*, WgiReading*); using WgiVibFn = HRESULT (STDMETHODCALLTYPE*)(void*, const WgiVibration*);
WgiReadFn wgiReadOrig{}; WgiVibFn wgiVibOrig{}; void* wgiReadTarget{}; void* wgiVibTarget{};
std::atomic<std::uint64_t> wgiReads{0}, wgiPresses{0}, wgiVibs{0}, wgiHidden{0}; std::atomic<bool> wgiA{false}; std::atomic<unsigned> wgiLogs{0};
WgiVibration wgiLastVib{-1, -1, -1, -1};
// M8j (2026-11-07): WS charge jump, controller only. M8i VERIFIED the game reads the pad through WGI
// GetCurrentReading (~77/s, A seen with 0 ms delay). While WS is active an A press is hidden from the game; on release
// the press is given back for 60 ms so the vanilla WS jump fires. Hold < 500 ms = normal WS jump; hold >= 500 ms sets
// SpeedMin/Max *= k first, k = sqrt(h / 1.8 m) with h the vanilla ground charge rise (M8e/M8f: ~15 m at 0.5 s rising
// linearly to ~39 m at 1.1 s, flat after) and 1.8 m the normal jump: k = 2.9 .. 4.65. HYPOTHESIS: Speed* sets the
// outward (wall-normal) jump-off speed. If WS ends while A is held, A stays hidden until release (no stray jump).
// INI WALLSPRINT_WS_CHARGE_JUMP (default 0, shipped 1); F8 stops it.
// M9b (2026-11-07): bounded velocity write test. M9a found two float3 slots matching the hero velocity in 92 % / 90 %
// of 1200 samples: [hero+0xBA0]+0x7FC and [hero+0xAE8]+0x1E0. On a WS charge release (hold >= 500 ms) a helper thread
// waits for the WS end (jump-off), then for 6 writes ~8 ms apart raises the wall-normal component of both slots to
// the charge launch speed v = sqrt(2 * 32 m/s^2 * h(hold)) (31-50 m/s; h = vanilla ground charge rise). Each write
// re-reads the pointer (private RW) and checks the slot is near the current velocity first. Speed* scaling is off.
// INI WALLSPRINT_WS_VELOCITY_WRITE (default 0, shipped 1); F8 stops it.
constexpr unsigned kVelSlot[2] = {0xBA0, 0xAE8}, kVelField[2] = {0x7FC, 0x1E0};
std::atomic<unsigned> m9Launches{0};
bool velReadable(std::uintptr_t p);
DWORD WINAPI m9Launch(void* arg) {
  float target = float(reinterpret_cast<std::uintptr_t>(arg)) / 100.f;
  auto t0 = GetTickCount64();
  while (!killed.load() && GetTickCount64() - t0 < 400) { auto now = GetTickCount64(), last = lastWsMs.load(); if (last < now && now - last > 30) break; Sleep(2); }
  auto hero = m9Hero.load(); float n[3] = {m9N[0].load(), m9N[1].load(), m9N[2].load()};
  unsigned writes = 0, refused = 0, why[2] = {0, 0}, wslot[2] = {0, 0}; std::uintptr_t ptrSeen[2] = {0, 0}; float before[2] = {-99, -99}, after[2] = {-99, -99};
  for (int i = 0; i < 6 && !killed.load() && hero; ++i) {
    for (int k = 0; k < 2; ++k) {
      std::uintptr_t obj = 0; if (!msm2::readMemory(hero + kVelSlot[k], &obj, 8)) { ++refused; why[k] = 1; continue; }
      auto a = obj + kVelField[k];
      // M9c: M9b refused slot 0 every time (velReadable needs 4 KiB after the object start). Check only the 12 bytes.
      MEMORY_BASIC_INFORMATION mb{}; if (!obj || !VirtualQuery(reinterpret_cast<void*>(a), &mb, sizeof mb) || mb.State != MEM_COMMIT || (mb.Protect & 0xff) != PAGE_READWRITE ||
          (mb.Protect & PAGE_GUARD) || reinterpret_cast<std::uintptr_t>(mb.BaseAddress) + mb.RegionSize < a + 12) { ++refused; why[k] = 2; ptrSeen[k] = obj; continue; }
      ptrSeen[k] = obj;
      float v[3]; if (!msm2::readMemory(a, v, 12) || !std::isfinite(v[0]) || !std::isfinite(v[1]) || !std::isfinite(v[2])) { ++refused; why[k] = 3; continue; }
      float vn = v[0] * n[0] + v[1] * n[1] + v[2] * n[2]; if (i == 0) before[k] = vn;
      if (std::fabs(v[0]) > 150 || std::fabs(v[1]) > 150 || std::fabs(v[2]) > 150) { ++refused; why[k] = 4; continue; }
      if (vn < target) { auto* f = reinterpret_cast<volatile float*>(a); float d = target - vn; f[0] = v[0] + n[0] * d; f[1] = v[1] + n[1] * d; f[2] = v[2] + n[2] * d; ++writes; ++wslot[k]; }
      after[k] = vn;
    }
    Sleep(8);
  }
  msm2::writeLog("WS_VEL_WRITE target_vn=%.1f writes=%u writes_slot=%u/%u refused=%u refuse_reason=%u/%u ptr=0x%llx/0x%llx vn_before=%.2f/%.2f vn_last_read=%.2f/%.2f wait_ms=%llu n=%.3f,%.3f,%.3f",
    target, writes, wslot[0], wslot[1], refused, why[0], why[1], (unsigned long long)ptrSeen[0], (unsigned long long)ptrSeen[1], before[0], before[1], after[0], after[1], (unsigned long long)(GetTickCount64() - t0), n[0], n[1], n[2]);
  return 0;
}
std::atomic<double> wgiRt{0}; std::atomic<unsigned> wgiL3Edges{0};
enum : int { kChIdle, kChHold, kChGive, kChSwallow };
std::atomic<int> chState{kChIdle}; std::atomic<std::uint64_t> chStart{0}, chGiveUntil{0}; unsigned chLogs{};
float chargeK(std::uint64_t ms) {
  if (ms < 500) return 1.f;
  float h = 15.f + 24.f * std::fmin(1.f, float(ms - 500) / 600.f);
  return std::sqrt(h / 1.8f);
}
// M8j-r2: input threads compare their own tick with lastWsMs stored by the game thread; a newer store made the unsigned
// difference wrap and read as "not WS". Signed check: a store newer than now counts as active.
bool wsActiveNow(std::uint64_t now) {
  auto last = lastWsMs.load(); return engagedManager.load() != 0 && last && (last >= now || now - last < 150);
}
// M9c: vanilla charge jumps are launched by releasing RT and A together. RT also holds the WR/WS, so releasing it
// ended the WS while A was charging and the charge was swallowed. While charging (and during the give-back) the game
// keeps seeing RT at its value from the charge start if that was >= 0.5.
std::atomic<double> chRt{0};
void chargeHoldRt(WgiReading* r) { double rt = chRt.load(); if (rt >= 0.5 && r->rt < rt) r->rt = rt; }
void wsChargeStep(WgiReading* r, bool a) {
  if (!ws::drive::wsChargeJump.load() || killed.load()) { chState.store(kChIdle); return; }
  auto now = GetTickCount64(); bool ws = wsActiveNow(now);
  // M8j-r3: `a && !prevA.exchange(a)` skipped the exchange while A was up, so prevA stayed true after the first press.
  static std::atomic<bool> prevA{false}; bool was = prevA.exchange(a); bool edge = a && !was;   // rising edge only
  switch (chState.load()) {
    case kChIdle:
      if (edge && ws) { chState.store(kChHold); chStart.store(now); chRt.store(r->rt); r->buttons &= ~0x4u; }
      break;
    case kChHold:
      r->buttons &= ~0x4u; chargeHoldRt(r);
      if (!ws) { chState.store(kChSwallow); if (chLogs++ < 60) msm2::writeLog("WS_CHARGE_JUMP event=ws_ended_while_held hold_ms=%llu", (unsigned long long)(now - chStart.load())); break; }
      if (!a) { auto hold = now - chStart.load(); float k = chargeK(hold); bool set = false;
        if (k > 1.f && ws::drive::wsVelocityWrite.load() && m9Launches.fetch_add(1) < 200) {
          float h = 15.f + 24.f * std::fmin(1.f, float(hold - 500) / 600.f); float v = std::sqrt(2.f * 32.f * h);
          if (HANDLE th = CreateThread(nullptr, 0, m9Launch, reinterpret_cast<void*>(std::uintptr_t(v * 100.f)), 0, nullptr)) { CloseHandle(th); set = true; } }
        else if (k > 1.f && !ws::drive::wsVelocityWrite.load()) set = jcSetSpeed(k);   // M8j fallback
        if (chLogs++ < 60) msm2::writeLog("WS_CHARGE_JUMP event=release hold_ms=%llu mode=%s k=%.2f strength_written=%u variant=%s", (unsigned long long)hold, k > 1.f ? "charge" : "normal", k, unsigned(set),
          !set ? "none" : jcLastVariant.load() == 0 ? "InSpeedMax" : jcLastVariant.load() == 1 ? "Speed" : "GenBoost");
        chState.store(kChGive); chGiveUntil.store(now + 60); r->buttons |= 0x4u; }
      break;
    case kChGive:
      if (now < chGiveUntil.load()) { r->buttons |= 0x4u; chargeHoldRt(r); } else { chState.store(kChIdle); chRt.store(0); r->buttons &= ~0x4u; }
      break;
    case kChSwallow:
      if (a) r->buttons &= ~0x4u; else { chState.store(kChIdle); chRt.store(0); }
      break;
  }
}
HRESULT STDMETHODCALLTYPE wgiReadDetour(void* self, WgiReading* r) {
  HRESULT hr = wgiReadOrig(self, r);
  if (fromSelf(__builtin_return_address(0))) return hr;
  wgiReads.fetch_add(1, std::memory_order_relaxed);
  if (SUCCEEDED(hr) && r) {
    bool a = (r->buttons & 0x4u) != 0;
    if (a && !wgiA.exchange(true)) { auto now = GetTickCount64(); xgPress.store(now); wgiPresses.fetch_add(1);
      if (wgiLogs.fetch_add(1) < 80) { auto kp = jiPress.load(); auto last = lastWsMs.load();
        msm2::writeLog("WS_INPUT_GAME_A module=WGI since_ws_seen_press_ms=%lld ws=%u engaged=%u since_ws_update_ms=%lld charge_state=%d charge_enabled=%u",
          kp ? (long long)(now - kp) : -1ll, unsigned(wsActiveNow(now)), unsigned(engagedManager.load() != 0), last ? (long long)now - (long long)last : -1ll, chState.load(), unsigned(ws::drive::wsChargeJump.load())); } }
    else if (!a) wgiA.store(false);
    if (a && ws::drive::jumpHoldbackTest.load() && !killed.load() && engagedManager.load() != 0 && GetTickCount64() - lastWsMs.load() < 150) {
      r->buttons &= ~0x4u; if (wgiHidden.fetch_add(1) < 40) msm2::writeLog("WS_JUMP_HOLDBACK module=WGI a_hidden=1 blocked=%llu", (unsigned long long)wgiHidden.load()); }
    wsChargeStep(r, a);
    wgiRt.store(r->rt, std::memory_order_relaxed);
    { bool l3 = (r->buttons & 0x1000u) != 0; static std::atomic<bool> prevL3{false}; bool was = prevL3.exchange(l3); if (l3 && !was) wgiL3Edges.fetch_add(1); }   // M10p gravity lock
  }
  return hr;
}
HRESULT STDMETHODCALLTYPE wgiVibDetour(void* self, const WgiVibration* v) {
  if (!fromSelf(__builtin_return_address(0))) { wgiVibs.fetch_add(1, std::memory_order_relaxed);
    if (v && (std::fabs(v->left - wgiLastVib.left) > 0.004 || std::fabs(v->right - wgiLastVib.right) > 0.004 || std::fabs(v->ltrig - wgiLastVib.ltrig) > 0.004 || std::fabs(v->rtrig - wgiLastVib.rtrig) > 0.004)) { wgiLastVib = *v;
      if (xsLogs.fetch_add(1) < 1500) { auto now = GetTickCount64(); auto p = xgPress.load(), kp = jiPress.load();
        msm2::writeLog("WS_RUMBLE module=WGI left=%.3f right=%.3f ltrig=%.3f rtrig=%.3f since_game_A_press_ms=%lld since_jump_press_ms=%lld game_A_held=%u since_release_ms=%lld read_only=1", v->left, v->right, v->ltrig, v->rtrig,
          p ? (long long)(now - p) : -1ll, kp ? (long long)(now - kp) : -1ll, unsigned(wgiA.load()), jiRelease.load() ? (long long)(now - jiRelease.load()) : -1ll); } } }
  return wgiVibOrig(self, v);
}
std::atomic<bool> wgiStarted{false};
DWORD WINAPI wgiSetup(void*) {
  HMODULE combase = LoadLibraryW(L"combase.dll"); if (!combase) { msm2::writeLog("WS_WGI_HOOK installed=0 reason=no_combase"); return 0; }
  using RoInitFn = HRESULT (WINAPI*)(int); using RoFactFn = HRESULT (WINAPI*)(void*, const IID&, void**);
  using StrFn = HRESULT (WINAPI*)(const wchar_t*, UINT32, void**); using StrDelFn = HRESULT (WINAPI*)(void*);
  auto roInit = reinterpret_cast<RoInitFn>(reinterpret_cast<void*>(GetProcAddress(combase, "RoInitialize")));
  auto roFact = reinterpret_cast<RoFactFn>(reinterpret_cast<void*>(GetProcAddress(combase, "RoGetActivationFactory")));
  auto mkStr = reinterpret_cast<StrFn>(reinterpret_cast<void*>(GetProcAddress(combase, "WindowsCreateString")));
  auto delStr = reinterpret_cast<StrDelFn>(reinterpret_cast<void*>(GetProcAddress(combase, "WindowsDeleteString")));
  if (!roInit || !roFact || !mkStr || !delStr) { msm2::writeLog("WS_WGI_HOOK installed=0 reason=no_ro_api"); return 0; }
  roInit(1);  // RO_INIT_MULTITHREADED on this helper thread only
  static const IID kStatics = {0x8bbce529, 0xd49c, 0x39e9, {0x95, 0x60, 0xe4, 0x7d, 0xde, 0x96, 0xb7, 0xc8}};
  static const IID kGamepad = {0xbc7bb43c, 0x0a69, 0x3903, {0x9e, 0x9d, 0xa5, 0x0f, 0x86, 0xa4, 0x5d, 0xe5}};
  const wchar_t* cls = L"Windows.Gaming.Input.Gamepad";
  for (int attempt = 0; attempt < 90 && !killed.load(); ++attempt, Sleep(2000)) {
    void* h = nullptr; if (FAILED(mkStr(cls, UINT32(std::wcslen(cls)), &h))) continue;
    void* statics = nullptr; HRESULT hr = roFact(h, kStatics, &statics); delStr(h);
    if (FAILED(hr) || !statics) { if (!attempt) msm2::writeLog("WS_WGI_HOOK factory_hr=0x%lx", (unsigned long)hr); continue; }
    auto svt = *reinterpret_cast<void***>(statics);
    void* view = nullptr; hr = reinterpret_cast<HRESULT (STDMETHODCALLTYPE*)(void*, void**)>(svt[10])(statics, &view);
    unsigned size = 0; void* pad = nullptr;
    if (SUCCEEDED(hr) && view) { auto vvt = *reinterpret_cast<void***>(view);
      reinterpret_cast<HRESULT (STDMETHODCALLTYPE*)(void*, unsigned*)>(vvt[7])(view, &size);
      if (size) reinterpret_cast<HRESULT (STDMETHODCALLTYPE*)(void*, unsigned, void**)>(vvt[6])(view, 0, &pad);
      reinterpret_cast<ULONG (STDMETHODCALLTYPE*)(void*)>(vvt[2])(view); }
    reinterpret_cast<ULONG (STDMETHODCALLTYPE*)(void*)>(svt[2])(statics);
    if (!pad) { if (attempt % 15 == 0) msm2::writeLog("WS_WGI_HOOK waiting gamepads=%u attempt=%d", size, attempt); continue; }
    void* igp = nullptr; auto pvt = *reinterpret_cast<void***>(pad);
    reinterpret_cast<HRESULT (STDMETHODCALLTYPE*)(void*, const IID&, void**)>(pvt[0])(pad, kGamepad, &igp);
    reinterpret_cast<ULONG (STDMETHODCALLTYPE*)(void*)>(pvt[2])(pad);
    if (!igp) { msm2::writeLog("WS_WGI_HOOK installed=0 reason=no_IGamepad"); return 0; }
    auto gvt = *reinterpret_cast<void***>(igp); void* readFn = gvt[8]; void* vibFn = gvt[7];
    reinterpret_cast<ULONG (STDMETHODCALLTYPE*)(void*)>(gvt[2])(igp);
    bool okR = MH_CreateHook(readFn, reinterpret_cast<void*>(&wgiReadDetour), reinterpret_cast<void**>(&wgiReadOrig)) == MH_OK && MH_EnableHook(readFn) == MH_OK;
    bool okV = MH_CreateHook(vibFn, reinterpret_cast<void*>(&wgiVibDetour), reinterpret_cast<void**>(&wgiVibOrig)) == MH_OK && MH_EnableHook(vibFn) == MH_OK;
    if (okR) { wgiReadTarget = readFn; }
    if (okV) { wgiVibTarget = vibFn; }
    msm2::writeLog("WS_WGI_HOOK installed_read=%u installed_vibration=%u gamepads=%u read_fn=0x%llx vib_fn=0x%llx pass_through=1", unsigned(okR), unsigned(okV), size,
      (unsigned long long)reinterpret_cast<std::uintptr_t>(readFn), (unsigned long long)reinterpret_cast<std::uintptr_t>(vibFn));
    return 0;
  }
  msm2::writeLog("WS_WGI_HOOK installed=0 reason=no_gamepad_in_180s");
  return 0;
}
void m8WgiHooks(std::uint64_t now) {
  static std::uint64_t lastLog = 0; static unsigned logs = 0;
  if (!killed.load() && GetModuleHandleW(L"Windows.Gaming.Input.dll") && !wgiStarted.exchange(true)) { if (HANDLE h = CreateThread(nullptr, 0, wgiSetup, nullptr, 0, nullptr)) CloseHandle(h); }
  if ((wgiReadTarget || wgiVibTarget) && now - lastLog >= 10000 && logs < 30) { lastLog = now; ++logs;
    msm2::writeLog("WS_WGI_STATUS reads=%llu a_presses=%llu vibration_calls=%llu hidden=%llu", (unsigned long long)wgiReads.load(), (unsigned long long)wgiPresses.load(), (unsigned long long)wgiVibs.load(), (unsigned long long)wgiHidden.load()); }
}
// M8i (2026-11-07): M8h could not hook WGI (our Gamepads list stayed empty for 180 s) and the WS jump still fired with
// XInput A hidden. HYPOTHESIS: the game reads the USB Xbox pad as a HID device (Raw Input and/or hid.dll parsing).
// Pass-through counting hooks on user32 GetRawInputData/GetRawInputBuffer (Raw Input types counted) and hid.dll
// HidP_*/HidD_* parsing/report functions; calls from the WS DLL are excluded. No value changed.
constexpr int kApiN = 9;
const char* const kApiMod[kApiN] = {"user32.dll", "user32.dll", "hid.dll", "hid.dll", "hid.dll", "hid.dll", "hid.dll", "hid.dll", "hid.dll"};
const char* const kApiName[kApiN] = {"GetRawInputData", "GetRawInputBuffer", "HidP_GetUsages", "HidP_GetUsagesEx", "HidP_GetData", "HidP_GetUsageValue", "HidP_GetScaledUsageValue", "HidD_GetInputReport", "HidP_GetButtonCaps"};
using Api8 = std::uintptr_t (WINAPI*)(std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t);
Api8 apiOrig[kApiN]{}; void* apiTarget[kApiN]{}; std::atomic<std::uint64_t> apiCalls[kApiN]{}, rawType[3]{};
template <int K> std::uintptr_t WINAPI apiDetour(std::uintptr_t a, std::uintptr_t b, std::uintptr_t c, std::uintptr_t d, std::uintptr_t e, std::uintptr_t f, std::uintptr_t g, std::uintptr_t h) {
  // Generic forwarder: <= 8 integer/pointer arguments; unused stack slots are passed through untouched.
  bool outside = !fromSelf(__builtin_return_address(0));
  auto r = apiOrig[K](a, b, c, d, e, f, g, h);
  if (outside) { apiCalls[K].fetch_add(1, std::memory_order_relaxed);
    if (K == 0 && b == 0x10000003u && c && r != std::uintptr_t(UINT(-1))) { auto type = reinterpret_cast<const RAWINPUTHEADER*>(c)->dwType; if (type < 3) rawType[type].fetch_add(1, std::memory_order_relaxed); } }
  return r;
}
template <int K> void apiHookOne() {
  if (apiTarget[K]) return;
  HMODULE m = GetModuleHandleA(kApiMod[K]); if (!m) return;
  auto t = reinterpret_cast<void*>(GetProcAddress(m, kApiName[K])); if (!t) return;
  if (MH_CreateHook(t, reinterpret_cast<void*>(&apiDetour<K>), reinterpret_cast<void**>(&apiOrig[K])) == MH_OK && MH_EnableHook(t) == MH_OK) apiTarget[K] = t;
}
void m8ApiHooks(std::uint64_t now) {
  static std::uint64_t lastTry = 0, lastLog = 0; static unsigned logs = 0;
  if (killed.load()) return;
  if (now - lastTry > 2000) { lastTry = now; apiHookOne<0>(); apiHookOne<1>(); apiHookOne<2>(); apiHookOne<3>(); apiHookOne<4>(); apiHookOne<5>(); apiHookOne<6>(); apiHookOne<7>(); apiHookOne<8>(); }
  if (now - lastLog >= 10000 && logs < 30) { lastLog = now; ++logs;
    char line[700]; int len = std::snprintf(line, sizeof line, "WS_INPUT_API_STATUS raw_mouse=%llu raw_keyboard=%llu raw_hid=%llu", (unsigned long long)rawType[0].load(), (unsigned long long)rawType[1].load(), (unsigned long long)rawType[2].load());
    for (int k = 0; k < kApiN && len > 0 && len < int(sizeof line) - 60; ++k) len += std::snprintf(line + len, sizeof line - std::size_t(len), " %s=%s%llu", kApiName[k], apiTarget[k] ? "" : "unhooked:", (unsigned long long)apiCalls[k].load());
    msm2::writeLog("%s read_only=1", line); }
}
// M9a (2026-11-07, read-only): find the hero velocity vector in memory. M8j: the WR jump config caps the jump-off
// speed (~22 m/s, InSpeedMax), so a wall-normal charge launch and gravity toward the wall need the velocity itself.
// M9b/M9c REJECTED both level-1 matches ([hero+0xBA0]+0x7FC, [hero+0xAE8]+0x1E0) as motion sources.
// M9f: search two pointer levels (hero/Transform 0x2000 -> targets 0x1000 -> their pointers in the first 0x400 bytes,
// 0x800 each; <= 1500 regions, rebuilt every 10 s), then an automatic bounded probe: for each slot with hit ratio
// >= 0.8 (max 16, two trials each), during a fall with no WS for 2 s add +4 m/s to its y once and compare the
// change of the measured vertical velocity with the previous sample. A motion source shows ~+4 m/s.
// INI WALLSPRINT_VELOCITY_PROBE (default 0, shipped 1). F8 stops it.
struct VelStat { std::uint32_t hits{}; float ex[3]{}; std::uint8_t trials{}; float effect[2]{}; };
std::unordered_map<std::uint64_t, VelStat> velStats;
struct VelRegion { std::uintptr_t addr; std::uint32_t size; std::uint64_t key; };
std::vector<VelRegion> velRegions; std::uint64_t velBuilt{}, velLastLog{}, velSamples{}; unsigned velLogs{};
bool velReadable(std::uintptr_t p) {
  if (p < 0x10000 || p > 0x7FFFFFFFFFFFull || (p & 7)) return false;
  MEMORY_BASIC_INFORMATION m{}; return VirtualQuery(reinterpret_cast<void*>(p), &m, sizeof m) && m.State == MEM_COMMIT && m.Type == MEM_PRIVATE &&
    (m.Protect & 0xff) == PAGE_READWRITE && !(m.Protect & PAGE_GUARD) && reinterpret_cast<std::uintptr_t>(m.BaseAddress) + m.RegionSize >= p + 0x800;
}
// key: bit63 root, bits48-62 level-1 slot (0x7FFF = direct), bits32-47 level-2 slot + 1 (0 = none), bits0-31 field
std::uint64_t velKey(unsigned root, unsigned s1, unsigned s2p1, unsigned field) { return (std::uint64_t(root) << 63) | (std::uint64_t(s1 & 0x7FFF) << 48) | (std::uint64_t(s2p1 & 0xFFFF) << 32) | field; }
bool velRejected(std::uint64_t k) { return k == velKey(0, 0xBA0, 0, 0x7FC) || k == velKey(0, 0xAE8, 0, 0x1E0); }
void velBuild(const msm2::Snapshot& hero) {
  velRegions.clear();
  std::uintptr_t roots[2] = {hero.hero, hero.transform};
  static std::uintptr_t q[0x400], q2[0x80];
  for (unsigned r = 0; r < 2; ++r) { if (!roots[r] || !velReadable(roots[r])) continue;
    velRegions.push_back({roots[r], 0x2000, velKey(r, 0x7FFF, 0, 0)});
    if (!msm2::readMemory(roots[r], q, sizeof q)) continue;
    for (unsigned i = 0; i < 0x400 && velRegions.size() < 1500; ++i) { auto t = q[i];
      if (t == roots[0] || t == roots[1] || !velReadable(t)) continue;
      velRegions.push_back({t, 0x1000, velKey(r, i * 8, 0, 0)});
      if (!msm2::readMemory(t, q2, sizeof q2)) continue;
      for (unsigned j = 0; j < 0x80 && velRegions.size() < 1500; ++j) { auto u = q2[j];
        if (u == t || u == roots[0] || u == roots[1] || !velReadable(u)) continue;
        velRegions.push_back({u, 0x800, velKey(r, i * 8, j * 8 + 1, 0)}); } } }
}
std::uintptr_t velResolve(const msm2::Snapshot& hero, std::uint64_t key) {
  unsigned root = unsigned(key >> 63), s1 = unsigned((key >> 48) & 0x7FFF), s2 = unsigned((key >> 32) & 0xFFFF), field = unsigned(key & 0xFFFFFFFFu);
  std::uintptr_t p = root ? hero.transform : hero.hero; if (!p) return 0;
  if (s1 != 0x7FFF) { std::uintptr_t t = 0; if (!msm2::readMemory(p + s1, &t, 8) || !velReadable(t)) return 0; p = t;
    if (s2) { std::uintptr_t u = 0; if (!msm2::readMemory(p + (s2 - 1), &u, 8) || !velReadable(u)) return 0; p = u; } }
  return p + field;
}
struct VelProbe { std::uint64_t key{}; float vyBefore{}, accBefore{}; std::uint64_t ms{}; int armed{}; } vprobe; float velPrevVy{}; bool velPrevOk{}; std::uint64_t velProbeLast{};
unsigned velProbes{}; std::vector<std::uintptr_t> velTestedAddr;   // M9g: aliases (same object via other paths) skipped
void m9VelocitySearch(const msm2::Snapshot& hero, const ws::diag::Motion& motion, std::uint64_t now) {
  if (killed.load() || !hero.hero || !ws::drive::velocityProbe.load()) return;   // M10a: route closed, off unless probing
  if (!velBuilt || now - velBuilt > 10000) { velBuild(hero); velBuilt = now; }
  if (!motion.valid) { velPrevOk = false; return; }
  float vy = motion.velocity.y; float acc = velPrevOk ? vy - velPrevVy : 0.f; bool accOk = velPrevOk; velPrevVy = vy; velPrevOk = true;
  // probe evaluation: the sample after the write
  // M9g: evaluated over two samples after the write: effect = (vy2 - vy0) - 2 * acc_before (gravity trend removed)
  if (vprobe.armed == 1) vprobe.armed = 2;
  else if (vprobe.armed == 2) { vprobe.armed = 0; auto& st = velStats[vprobe.key];
    // M9h: +15 m/s push; a sample at |vy| < 0.5 after the write means a landing -> trial discarded (not counted).
    bool landed = std::fabs(vy) < 0.5f;
    float effect = (vy - vprobe.vyBefore) - 2.f * vprobe.accBefore;
    if (!landed) { if (st.trials < 2) st.effect[st.trials] = effect; ++st.trials; }
    msm2::writeLog("WS_VEL_PROBE key=0x%016llx trial=%u vy_before=%.2f vy_after2=%.2f acc_before=%.2f effect=%.2f expected=+15 landed_discarded=%u",
      (unsigned long long)vprobe.key, unsigned(st.trials), vprobe.vyBefore, vy, vprobe.accBefore, effect, unsigned(landed)); }
  // probe arming: falling, no WS for 2 s, steady gravity trend
  if (ws::drive::velocityProbe.load() && velSamples >= 300 && !vprobe.armed && now - velProbeLast > 600 && velProbes < 120 && accOk &&
      now - lastWsMs.load() > 2000 && vy < -5.f && acc < -1.f && acc > -6.f) {
    std::uint64_t best = 0; std::uint32_t bestHits = 0; std::uintptr_t a = 0;
    for (int pass = 0; pass < 8; ++pass) { best = 0; bestHits = 0;
      for (auto& kv : velStats) if (!velRejected(kv.first) && kv.second.trials < 2 && double(kv.second.hits) / double(velSamples) >= 0.8 && kv.second.hits > bestHits) { best = kv.first; bestHits = kv.second.hits; }
      if (!best) break;
      a = velResolve(hero, best);
      bool alias = false; for (auto t : velTestedAddr) alias |= t == a;
      auto& st0 = velStats[best];
      if (alias && st0.trials == 0) { st0.trials = 2; st0.effect[0] = st0.effect[1] = -77.f; best = 0; continue; }   // -77 = alias of a tested address
      break; }
    if (best) { float v[3];
      MEMORY_BASIC_INFORMATION mb{};
      if (a && VirtualQuery(reinterpret_cast<void*>(a), &mb, sizeof mb) && (mb.Protect & 0xff) == PAGE_READWRITE && msm2::readMemory(a, v, 12) &&
          std::fabs(v[1] - vy) < 0.25f * std::fabs(vy) + 1.f) {
        *reinterpret_cast<volatile float*>(a + 4) = v[1] + 15.f; ++velProbes; velProbeLast = now;
        if (velStats[best].trials == 0) velTestedAddr.push_back(a);
        vprobe = {best, vy, acc, now, 1}; }
      else { auto& st = velStats[best]; ++st.trials; st.effect[st.trials > 1 ? 1 : 0] = -99.f; } } }
  float sp = std::sqrt(ws::dot(motion.velocity, motion.velocity)); if (!std::isfinite(sp) || sp < 4.f || sp > 120.f) return;
  float tol = 0.25f * sp + 0.3f; ++velSamples;
  static unsigned char buf[0x2000];
  for (auto& rg : velRegions) {
    if (!msm2::readMemory(rg.addr, buf, rg.size)) continue;
    for (std::uint32_t o = 0; o + 12 <= rg.size; o += 4) {
      float v[3]; std::memcpy(v, buf + o, 12);
      float dx = v[0] - motion.velocity.x, dy = v[1] - motion.velocity.y, dz = v[2] - motion.velocity.z;
      if (!(dx * dx + dy * dy + dz * dz <= tol * tol)) continue;   // also rejects NaN
      auto& st = velStats[rg.key | o]; ++st.hits; st.ex[0] = v[0]; st.ex[1] = v[1]; st.ex[2] = v[2];
    }
  }
  if (now - velLastLog >= 30000 && velLogs < 20 && velSamples >= 20) { velLastLog = now; ++velLogs;
    std::vector<std::pair<std::uint64_t, VelStat>> top(velStats.begin(), velStats.end());
    std::sort(top.begin(), top.end(), [](auto& x, auto& y) { return x.second.hits > y.second.hits; });
    msm2::writeLog("WS_VEL_SEARCH samples=%llu regions=%zu candidates=%zu probes=%u read_only_except_probe=1", (unsigned long long)velSamples, velRegions.size(), velStats.size(), velProbes);
    for (std::size_t i = 0; i < top.size() && i < 16; ++i) { auto k = top[i].first; auto& st = top[i].second;
      msm2::writeLog("WS_VEL_CANDIDATE rank=%zu key=0x%016llx root=%s s1=0x%x s2=0x%x field=0x%x hits=%u ratio=%.2f trials=%u effect=%.2f/%.2f rejected=%u last=%.2f,%.2f,%.2f",
        i + 1, (unsigned long long)k, (k >> 63) ? "transform" : "hero", unsigned((k >> 48) & 0x7FFF), unsigned((k >> 32) & 0xFFFF) ? unsigned((k >> 32) & 0xFFFF) - 1 : 0xFFFFu,
        unsigned(k & 0xFFFFFFFFu), st.hits, double(st.hits) / double(velSamples), unsigned(st.trials), st.effect[0], st.effect[1], unsigned(velRejected(k)), st.ex[0], st.ex[1], st.ex[2]); } }
}
// M10a (2026-11-07, read-only): gravity owner discovery. Static evidence (analysis/MOTION_GRAVITY_OWNER_CONTRACT_20261031.md):
// GravityOrientationOwner calls SetGravityVector with RCX = load64(load64(entry RCX + 0xF0) + 0xF8) + 0x5F4; the setter
// writes RCX+12..23 and +24..35 (and scalars at +0/+4/+8). The owner type is UNKNOWN, so this chain is evaluated from
// the hero actor, its Transform and every heap object they point to (first 0x2000 bytes); the 9 floats at the end of
// each complete chain are logged when one of the vectors looks like gravity (|v| ~1 or 5-60, pointing down).
unsigned gvLogs{}; std::uint64_t gvLast{}; std::vector<std::uintptr_t> gvSeen;
// M10b: also up-pointing vectors (an "orientation" may store up), and every complete chain is logged once.
bool gvVec(const float* v) { float l = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); if (!std::isfinite(l)) return false;
  return ((l > 0.95f && l < 1.05f) || (l > 5.f && l < 60.f)) && std::fabs(v[1]) > 0.9f * l; }
[[maybe_unused]] void m10GravityChain(const msm2::Snapshot& hero, std::uint64_t now) {   // M10c-r2: no longer called (worker load)
  if (killed.load() || !hero.hero || gvLogs >= 60 || now - gvLast < 5000) { return; }
  gvLast = now;
  std::vector<std::pair<std::uintptr_t, unsigned>> bases; bases.push_back({hero.hero, 0xFFFFu}); bases.push_back({hero.transform, 0xFFFEu});
  static std::uintptr_t q[0x400];
  for (int r = 0; r < 2; ++r) { auto root = r ? hero.transform : hero.hero; if (!root || !msm2::readMemory(root, q, sizeof q)) continue;
    for (unsigned i = 0; i < 0x400; ++i) if (velReadable(q[i])) bases.push_back({q[i], (unsigned(r) << 15) | (i * 8)}); }
  unsigned chains = 0, hits = 0;
  for (auto& b : bases) {
    std::uintptr_t p1 = 0, p2 = 0; if (!msm2::readMemory(b.first + 0xF0, &p1, 8) || !velReadable(p1)) continue;
    if (!msm2::readMemory(p1 + 0xF8, &p2, 8) || !velReadable(p2)) continue;
    auto g = p2 + 0x5F4; float f[9]; if (!msm2::readMemory(g, f, sizeof f)) { continue; }
    ++chains;
    bool like = gvVec(f + 3) || gvVec(f + 6) || gvVec(f); if (like) { ++hits; }
    bool seen = false; for (auto x : gvSeen) { seen |= x == g; }
    if (seen) { continue; }
    gvSeen.push_back(g);
    if (gvLogs++ < 60) msm2::writeLog("WS_GRAVITY_CHAIN like=%u base=%s0x%x g=0x%llx f=%.3f,%.3f,%.3f | %.3f,%.3f,%.3f | %.3f,%.3f,%.3f read_only=1",
      unsigned(like), b.second == 0xFFFFu ? "hero" : b.second == 0xFFFEu ? "transform" : (b.second >> 15) ? "transform_ptr@" : "hero_ptr@", (b.second >= 0xFFFEu) ? 0u : (b.second & 0x7FFF),
      (unsigned long long)g, f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8]);
  }
  static unsigned sum = 0; if (sum++ < 30) msm2::writeLog("WS_GRAVITY_CHAIN_STATUS bases=%zu complete_chains=%u gravity_like=%u read_only=1", bases.size(), chains, hits);
}
// M10c (2026-11-07): gravity block test. M10b found, for one hero instance, [Transform+0xA88] -> +0xF0 -> +0xF8 -> +0x5F4
// holding (0,-1,0 | 0,1,0 | 0,1,0): the SetGravityVector target layout (scalars/vector at +0, vectors at +12/+24).
// Discovery: same chain from the Transform, its level-1 targets (0x2000 bytes) and their level-2 targets (0x400 bytes),
// identity = exactly that vanilla pattern. Test (INI WALLSPRINT_WS_GRAVITY_TEST, default 0, shipped 1): when a WS jump
// starts, write (-n | n | n) with n = last WS wall normal (gravity toward the wall), re-write each tick for 1.5 s,
// then restore the vanilla pattern. Each tick logs whether the game kept the written values.
constexpr float kGravVanilla[9] = {0, -1, 0, 0, 1, 0, 0, 1, 0};
std::uintptr_t gbAddr{}; std::uint64_t gbStart{}, gbLastScan{}; bool gbActive{}; unsigned gbLogs{}, gbTests{};
bool gbIsVanilla(const float* f) { for (int i = 0; i < 9; ++i) if (std::fabs(f[i] - kGravVanilla[i]) > 1e-4f) return false; return true; }
bool gbChain(std::uintptr_t b, std::uintptr_t& g) { std::uintptr_t p1 = 0, p2 = 0;
  if (!msm2::readMemory(b + 0xF0, &p1, 8) || !velReadable(p1) || !msm2::readMemory(p1 + 0xF8, &p2, 8) || !velReadable(p2)) return false;
  g = p2 + 0x5F4; float f[9]; return msm2::readMemory(g, f, sizeof f) && gbIsVanilla(f); }
std::atomic<std::uintptr_t> gbHero{0}, gbTransform{0}, gbFound{0}; std::atomic<bool> gbThread{false};
// M10e (2026-11-07): observe the game's SetGravityVector (RVA 0x35132A0, no .pdata; accepted 79-byte identity
// sha256 7aaa6fef...31aa, analysis/MOTION_GRAVITY_OWNER_RESOLUTION_20261031.md). Pass-through: the original runs first
// with untouched registers; afterwards RCX (the gravity block) and the 9 floats it now holds are recorded. Distinct
// blocks are logged (<= 24); if the chain search finds nothing, a single vanilla block seen here is used for the test.
constexpr std::uintptr_t kSetGravityRva = 0x35132A0;
constexpr char kSetGravitySha[] = "7aaa6fefc1776035cffb9c3f291ac59c892444826dbd8b191092d71a910631aa";
using SetGravFn = std::uintptr_t (*)(std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t);
SetGravFn sgOrig{}; void* sgTarget{}; std::atomic<std::uint64_t> sgCalls{0}; std::atomic<unsigned> sgDistinct{0}; std::atomic<std::uintptr_t> sgLast{0};
std::uintptr_t sgSeen[24]{};
// M10j: per-block call counts, and input substitution: while active, calls for the hero's block get the wall normal
// as the up input instead of the game's (0,1,0). Integer copies only before the original call (no xmm use).
std::atomic<std::uint64_t> sgCount[24]{}; std::atomic<std::uintptr_t> sgSubBlock{0}; std::atomic<bool> sgSubActive{false};
std::atomic<std::uint32_t> sgSubBits[3]{}; std::atomic<std::uint64_t> sgSubCalls{0}; std::atomic<std::uintptr_t> sgHeroBlock{0}, sgHeroByUp{0}; std::atomic<bool> sgAirPhase{false}, sgLocked{false}, sgGameLanded{false}; std::atomic<float> sgLastVy{0}; std::atomic<unsigned> sgHeroLogs{0}, sgOtherLogs{0};
std::uintptr_t sgDetour(std::uintptr_t rcx, std::uintptr_t rdx, std::uintptr_t r8, std::uintptr_t r9) {
  std::uint32_t sub[4] = {0, 0, 0, 0};
  bool doSub = sgSubActive.load(std::memory_order_relaxed) && rcx == sgSubBlock.load(std::memory_order_relaxed) && rcx;
  // M10p: ground contact by the game itself. In the air phase the game passing an up that turns toward vertical while the
  // hero's vertical speed is ~0 means it landed: stop substituting in this frame and let the game blend to its gravity.
  // (also when locked: a landing ends the lock)
  if (doSub && sgAirPhase.load(std::memory_order_relaxed)) { std::uint32_t inb[3];
    if (msm2::readMemory(rdx, inb, sizeof inb)) { float in[3]; std::memcpy(in, inb, 12); float sb[3]; for (int i = 0; i < 3; ++i) { std::uint32_t b = sgSubBits[i].load(); std::memcpy(&sb[i], &b, 4); }
      if (std::isfinite(in[1]) && in[1] > sb[1] + 0.05f && std::fabs(sgLastVy.load(std::memory_order_relaxed)) < 2.f) { doSub = false; sgSubActive.store(false); sgGameLanded.store(true); } } }
  if (doSub) { sub[0] = sgSubBits[0].load(); sub[1] = sgSubBits[1].load(); sub[2] = sgSubBits[2].load(); sgSubCalls.fetch_add(1, std::memory_order_relaxed); }
  auto r = sgOrig(rcx, doSub ? reinterpret_cast<std::uintptr_t>(sub) : rdx, r8, r9);
  // M10l: M10k's chain block was not a setter block. Hero identification: the first block that receives a non-vertical up
  // (|y| < 0.9; only a character on a wall/ceiling, i.e. the crawling hero) becomes the hero block for the session.
  if (rcx && !doSub) { float in[3]{}; if (msm2::readMemory(rdx, in, sizeof in) && std::isfinite(in[1]) && std::fabs(in[1]) < 0.9f) {
      if (!sgHeroByUp.load()) { sgHeroByUp.store(rcx); sgHeroBlock.store(rcx); gbFound.store(rcx);
        msm2::writeLog("WS_SET_GRAVITY_HERO_ID block=0x%llx game_up=%.3f,%.3f,%.3f (first non-vertical up)", (unsigned long long)rcx, in[0], in[1], in[2]); }
      else if (rcx != sgHeroByUp.load() && sgOtherLogs.fetch_add(1) < 20) msm2::writeLog("WS_SET_GRAVITY_OTHER_NONVERTICAL block=0x%llx up=%.3f,%.3f,%.3f", (unsigned long long)rcx, in[0], in[1], in[2]); } }
  if (rcx && rcx == sgHeroBlock.load(std::memory_order_relaxed) && sgHeroLogs.load() < 80) { float in[3]{}; if (msm2::readMemory(rdx, in, sizeof in)) {
      static float last[3] = {9, 9, 9}; if (std::fabs(in[0] - last[0]) + std::fabs(in[1] - last[1]) + std::fabs(in[2] - last[2]) > 0.02f || doSub) { last[0] = in[0]; last[1] = in[1]; last[2] = in[2];
        sgHeroLogs.fetch_add(1); msm2::writeLog("WS_SET_GRAVITY_HERO game_up=%.3f,%.3f,%.3f substituted=%u ws=%u since_ws_ms=%lld", in[0], in[1], in[2], unsigned(doSub),
          unsigned(engagedManager.load() != 0 && GetTickCount64() - lastWsMs.load() < 150), lastWsMs.load() ? (long long)(GetTickCount64() - lastWsMs.load()) : -1ll); } } }
  sgCalls.fetch_add(1, std::memory_order_relaxed); sgLast.store(rcx);
  { unsigned nn = sgDistinct.load(); for (unsigned i = 0; i < nn && i < 24; ++i) if (sgSeen[i] == rcx) { sgCount[i].fetch_add(1, std::memory_order_relaxed); break; } }
  bool known = false; unsigned n = sgDistinct.load(); for (unsigned i = 0; i < n && i < 24; ++i) known |= sgSeen[i] == rcx;
  if (!known && n < 24) { sgSeen[n] = rcx; sgDistinct.store(n + 1); float f[9]{}; float in[3]{}; bool rf = msm2::readMemory(rcx, f, sizeof f); bool ri = msm2::readMemory(rdx, in, sizeof in);
    msm2::writeLog("WS_SET_GRAVITY call=%llu block=0x%llx input_read=%u input=%.3f,%.3f,%.3f block_read=%u f=%.3f,%.3f,%.3f|%.3f,%.3f,%.3f|%.3f,%.3f,%.3f thread=%lu",
      (unsigned long long)sgCalls.load(), (unsigned long long)rcx, unsigned(ri), in[0], in[1], in[2], unsigned(rf), f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], (unsigned long)GetCurrentThreadId()); }
  return r;
}
// M10i: MinHook refused this entry (MH_ERROR_UNSUPPORTED_FUNCTION: VEX prefix c5 fb 10 02 = vmovsd xmm0,[rdx];
// c5 fa 10 15 <disp32> = vmovss xmm2,[rip+disp32]). Exact-prefix manual hook: the first 12 bytes (these two
// instructions, verified byte-exact apart from disp32) move to a trampoline allocated within +-2 GiB, the RIP-relative
// displacement is recomputed, and the trampoline jumps back to entry+12. The entry gets `jmp rel32` to a near relay
// that jumps to sgDetour; 7 NOPs pad the 12 bytes. Restored byte-exact on stop.
unsigned char sgSaved[12]{}; std::uintptr_t sgEntry{}; unsigned char* sgNear{};
unsigned char* sgAllocNear(std::uintptr_t target) {
  SYSTEM_INFO si{}; GetSystemInfo(&si); const std::uintptr_t gran = si.dwAllocationGranularity;
  for (std::uintptr_t d = gran; d < 0x60000000ull; d += gran) for (int sgn = -1; sgn <= 1; sgn += 2) {
    std::uintptr_t a = sgn < 0 ? (target > d ? (target - d) & ~(gran - 1) : 0) : ((target + d) & ~(gran - 1)); if (!a) continue;
    if (void* p = VirtualAlloc(reinterpret_cast<void*>(a), 4096, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)) return static_cast<unsigned char*>(p); }
  return nullptr;
}
bool sgManualHook(std::uintptr_t t, void* detour, void** original, const char*& why) {
  const unsigned char* c = reinterpret_cast<const unsigned char*>(t);
  static const unsigned char kI1[4] = {0xc5, 0xfb, 0x10, 0x02}; static const unsigned char kI2[4] = {0xc5, 0xfa, 0x10, 0x15};
  if (std::memcmp(c, kI1, 4) || std::memcmp(c + 4, kI2, 4)) { why = "prefix_mismatch"; return false; }
  std::int32_t disp; std::memcpy(&disp, c + 8, 4); std::uintptr_t abs = t + 12 + std::intptr_t(disp);
  auto* n = sgAllocNear(t); if (!n) { why = "no_near_memory"; return false; }
  // trampoline at n+0: i1 (4) + i2 (8, new disp) + jmp [rip+0] abs64 (14) ; relay at n+64: jmp [rip+0] detour
  std::memcpy(n, c, 4); std::memcpy(n + 4, kI2, 4);
  std::intptr_t nd = std::intptr_t(abs) - std::intptr_t(n + 12); if (nd > INT32_MAX || nd < INT32_MIN) { VirtualFree(n, 0, MEM_RELEASE); why = "disp_range"; return false; }
  std::int32_t nd32 = std::int32_t(nd); std::memcpy(n + 8, &nd32, 4);
  n[12] = 0xff; n[13] = 0x25; std::memset(n + 14, 0, 4); std::uintptr_t back = t + 12; std::memcpy(n + 18, &back, 8);
  n[64] = 0xff; n[65] = 0x25; std::memset(n + 66, 0, 4); auto det = reinterpret_cast<std::uintptr_t>(detour); std::memcpy(n + 70, &det, 8);
  std::intptr_t rel = std::intptr_t(n + 64) - std::intptr_t(t + 5); if (rel > INT32_MAX || rel < INT32_MIN) { VirtualFree(n, 0, MEM_RELEASE); why = "relay_range"; return false; }
  unsigned char patch[12]; patch[0] = 0xe9; std::int32_t r32 = std::int32_t(rel); std::memcpy(patch + 1, &r32, 4); std::memset(patch + 5, 0x90, 7);
  DWORD old = 0; if (!VirtualProtect(reinterpret_cast<void*>(t), 12, PAGE_EXECUTE_READWRITE, &old)) { VirtualFree(n, 0, MEM_RELEASE); why = "protect"; return false; }
  std::memcpy(sgSaved, c, 12); *original = n;
  // write the tail first, then the 4 bytes that start the jump, so no thread sees a half-written jump target
  std::memcpy(reinterpret_cast<void*>(t + 4), patch + 4, 8); std::memcpy(reinterpret_cast<void*>(t), patch, 4);
  DWORD tmp = 0; VirtualProtect(reinterpret_cast<void*>(t), 12, old, &tmp); FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(t), 12);
  sgEntry = t; sgNear = n; why = "ok"; return true;
}
void sgManualUnhook() {
  if (!sgEntry) { return; }
  DWORD old = 0;
  if (VirtualProtect(reinterpret_cast<void*>(sgEntry), 12, PAGE_EXECUTE_READWRITE, &old)) { std::memcpy(reinterpret_cast<void*>(sgEntry), sgSaved, 12); DWORD tmp = 0;
    VirtualProtect(reinterpret_cast<void*>(sgEntry), 12, old, &tmp); FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(sgEntry), 12); }
  sgEntry = 0;   // trampoline memory stays allocated (a thread may still be inside it)
}
void sgInstall(std::uintptr_t image, std::size_t imageSize) {
  if (!image || imageSize <= kSetGravityRva + 79) return;
  auto t = image + kSetGravityRva; unsigned char code[79]{}; char hash[65]{};
  bool ok = msm2::readMemory(t, code, sizeof code) && sha256(code, sizeof code, hash) && !std::strcmp(hash, kSetGravitySha);
  const char* why = ok ? "" : "sha_mismatch";
  if (ok) ok = sgManualHook(t, reinterpret_cast<void*>(&sgDetour), reinterpret_cast<void**>(&sgOrig), why);
  if (ok) sgTarget = reinterpret_cast<void*>(t);
  msm2::writeLog("WS_SET_GRAVITY_HOOK installed=%u method=manual_vex_prefix result=%s rva=0x35132a0 sha256=%s pass_through=1", unsigned(ok), why, hash[0] ? hash : "none");
}
// M10c-r2: M10c ran this search on the 100 ms worker; ~10k bases every 3 s stalled that worker for tens of seconds and
// broke the WS camera/orientation. The search now runs on its own low-priority thread, yields every 64 bases, and
// only runs again when the stored block stops matching (or the hero/Transform changes).
DWORD WINAPI gbSearch(void*) {
  static std::uintptr_t q[0x400], q2[0x80];
  while (!killed.load()) {
    auto hero = gbHero.load(), tr = gbTransform.load();
    if (!hero || gbFound.load() || sgHeroByUp.load()) { Sleep(1000); continue; }
    unsigned bases = 0; std::uintptr_t found = 0; std::uintptr_t roots[2] = {tr, hero};
    for (int r = 0; r < 2 && !found && !killed.load(); ++r) { auto root = roots[r]; if (!root) continue; std::uintptr_t g = 0;
      if (gbChain(root, g)) { found = g; break; }
      if (!msm2::readMemory(root, q, sizeof q)) continue;
      for (unsigned i = 0; i < 0x400 && !found; ++i) { auto t = q[i]; if (!velReadable(t)) continue; if ((++bases & 63) == 0) Sleep(1);
        if (gbChain(t, g)) { found = g; if (gbLogs++ < 20) msm2::writeLog("WS_GRAVITY_BLOCK found root=%s slot=0x%x level=1 g=0x%llx", r ? "hero" : "transform", i * 8, (unsigned long long)g); break; }
        if (!msm2::readMemory(t, q2, sizeof q2)) continue;
        for (unsigned j = 0; j < 0x80; ++j) { auto u = q2[j]; if (!velReadable(u)) continue; if ((++bases & 63) == 0) Sleep(1);
          if (gbChain(u, g)) { found = g; if (gbLogs++ < 20) msm2::writeLog("WS_GRAVITY_BLOCK found root=%s slot=0x%x slot2=0x%x level=2 g=0x%llx", r ? "hero" : "transform", i * 8, j * 8, (unsigned long long)g); break; } } } }
    if (found && !sgHeroByUp.load()) { gbFound.store(found); sgHeroBlock.store(found); }
    else { static unsigned miss = 0; if (miss++ < 10) msm2::writeLog("WS_GRAVITY_BLOCK not_found bases=%u", bases); Sleep(10000); }
  }
  return 0;
}
void gbDiscover(const msm2::Snapshot& hero, std::uint64_t now) {
  if (gbHero.load() != hero.hero || gbTransform.load() != hero.transform) { gbHero.store(hero.hero); gbTransform.store(hero.transform); if (!gbActive) { gbFound.store(0); sgHeroByUp.store(0); sgHeroBlock.store(0); } }
  if (!gbThread.exchange(true)) { if (HANDLE h = CreateThread(nullptr, 0, gbSearch, nullptr, 0, nullptr)) { SetThreadPriority(h, THREAD_PRIORITY_LOWEST); CloseHandle(h); } }
  if (now - gbLastScan < 1000) { gbAddr = gbFound.load(); return; }
  gbLastScan = now;
  // M10k: the game itself sets other up vectors for the hero block (M10j: bursts of calls), so a found block stays until
  // the hero/Transform changes or the memory becomes unreadable; it no longer has to hold the vanilla pattern.
  auto g = gbFound.load(); if (g && !gbActive) { float f[9]; if (!msm2::readMemory(g, f, sizeof f)) { gbFound.store(0); g = 0; } }
  if (!g && sgDistinct.load() == 1) { float f[9]; auto c = sgSeen[0]; if (msm2::readMemory(c, f, sizeof f) && gbIsVanilla(f)) { g = c; gbFound.store(c);
    static bool once = false; if (!once) { once = true; msm2::writeLog("WS_GRAVITY_BLOCK from_setter g=0x%llx", (unsigned long long)c); } } }
  gbAddr = g;
}
void gbWrite(const float* v) { MEMORY_BASIC_INFORMATION mb{};
  if (!gbAddr || !VirtualQuery(reinterpret_cast<void*>(gbAddr), &mb, sizeof mb) || (mb.Protect & 0xff) != PAGE_READWRITE) return;
  auto* f = reinterpret_cast<volatile float*>(gbAddr); for (int i = 0; i < 9; ++i) f[i] = v[i]; }
// M10o (owner, log 20261107-231255): (1) upright frames at the jump: the hold started 180-250 ms after WallRun Deinit;
// (2) slow / wrong releases from the speed rule (also mid-air at the apex, C-3); (3) short WS sessions had no sampled
// normal (C-4); (4) the camera returned to vanilla at the jump (also flipped the stick mapping, C-2).
// Now: hold armed already during WS (setter input substitution with the WallRun normal; the game itself sets that up at WS
// start), so there is no gap at the jump; after the WS the block is also written each tick. Ends: new WS (continues with
// the new wall), landing (world vertical speed jumps from < -3 m/s to > -0.7 m/s between samples), or 3 s.
// gbHoldA keeps the WS camera for the whole hold.
std::atomic<bool> gbHoldA{false}; int gbState{0};   // 0 off, 1 armed during WS, 2 air hold, 3 blend back to vanilla
// M10p (owner): full hold 8 s, then a 2 s blend back (up turns from the wall normal to world up; body follows); web swing
// (RT held in the air) keeps the hold; L3 click during WS locks the gravity (no timeout / sprint-release end) until a new
// WS or a landing; ground landing = the game's own up change with vertical speed ~0 (setter detour, same frame) or the
// vertical-speed stop, both end at once.
std::uint64_t gbBlendStart{}; float gbN[3]{}; unsigned gbL3Seen{};
void gbSetUp(const float* up) { float want[9] = {-up[0], -up[1], -up[2], up[0], up[1], up[2], up[0], up[1], up[2]}; gbWrite(want);
  for (int i = 0; i < 3; ++i) { std::uint32_t bits; std::memcpy(&bits, &up[i], 4); sgSubBits[i].store(bits); } }
void gbEnd(const char* why, std::uint64_t now, bool writeVanilla = true) {
  sgSubActive.store(false); sgAirPhase.store(false); gbAirCam.store(false); bool wasLocked = sgLocked.exchange(false);
  if (gbState >= 2 && writeVanilla) { gbWrite(kGravVanilla); }
  gbActive = false; gbHoldA.store(false);
  msm2::writeLog("WS_GRAVITY_TEST restore test=%u reason=%s held_ms=%llu locked=%u", gbTests, why, (unsigned long long)(gbState >= 2 ? now - gbStart : 0), unsigned(wasLocked)); gbState = 0;
}
void m10GravityTest(const msm2::Snapshot& hero, const ws::diag::Motion& motion, std::uint64_t now) {
  if (killed.load() || !hero.hero) { if (gbState) gbEnd("killed_or_no_hero", now); return; }
  gbDiscover(hero, now);
  if (!ws::drive::wsGravityTest.load() || !gbAddr) { if (gbState) gbEnd("disabled_or_no_block", now); return; }
  bool wsNow = engagedManager.load() != 0 && now - lastWsMs.load() < 150;
  float n[3]; auto wnMs = ws::drive::wallNMs.load(); bool fromWr = wnMs && wnMs >= wsSessionStartMs.load();
  for (int i = 0; i < 3; ++i) n[i] = fromWr ? ws::drive::wallN[i].load() : m9N[i].load();
  float nl = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
  bool nOk = nl > 0.95f && nl < 1.05f && std::fabs(n[1]) < 0.3f && (fromWr || (m9NMs.load() && m9NMs.load() >= wsSessionStartMs.load()));
  static float prevVy = 0; static bool prevOk = false; float vy = motion.velocity.y; bool vOk = motion.valid; sgLastVy.store(vOk ? vy : 99.f);
  unsigned l3 = wgiL3Edges.load(); bool l3Press = l3 != gbL3Seen; gbL3Seen = l3;
  if (wsNow && nOk) {
    std::memcpy(gbN, n, sizeof gbN);
    if (gbState >= 2 && sgLocked.exchange(false)) msm2::writeLog("WS_GRAVITY_LOCK off reason=new_ws");
    for (int i = 0; i < 3; ++i) { std::uint32_t bits; std::memcpy(&bits, &n[i], 4); sgSubBits[i].store(bits); }
    sgSubBlock.store(gbAddr); sgSubActive.store(true); sgAirPhase.store(false); gbAirCam.store(false); gbHoldA.store(true); gbActive = true;
    if (gbState != 1) { if (gbState == 0) { ++gbTests; msm2::writeLog("WS_GRAVITY_TEST armed test=%u n=%.3f,%.3f,%.3f g=0x%llx", gbTests, n[0], n[1], n[2], (unsigned long long)gbAddr); }
      else { msm2::writeLog("WS_GRAVITY_TEST ws_again test=%u n=%.3f,%.3f,%.3f", gbTests, n[0], n[1], n[2]); }
      gbState = 1; }
    if (l3Press) { sgLocked.store(true); msm2::writeLog("WS_GRAVITY_LOCK on n=%.3f,%.3f,%.3f", n[0], n[1], n[2]); }
  } else if (gbState == 1 && !wsNow) {
    gbState = 2; gbStart = now; gbSetUp(gbN); prevOk = false; sgAirPhase.store(true); gbAirCam.store(true); sgGameLanded.store(false);
    msm2::writeLog("WS_GRAVITY_TEST start test=%u n=%.3f,%.3f,%.3f locked=%u", gbTests, gbN[0], gbN[1], gbN[2], unsigned(sgLocked.load()));
  } else if (gbState >= 2) {
    bool velStop = vOk && prevOk && now - gbStart >= 150 && prevVy < -3.f && vy > -0.7f;
    if (sgGameLanded.exchange(false)) gbEnd("landed_game", now, false);        // the game blends to its own gravity
    else if (velStop) gbEnd("landed_velocity", now, true);
    else if (gbState == 2) {
      if (wgiRt.load() > 0.5) gbStart = now;                                   // web swing keeps the hold
      if (!sgLocked.load() && now - gbStart >= 8000) { gbState = 3; gbBlendStart = now; msm2::writeLog("WS_GRAVITY_TEST blend_back test=%u", gbTests); }
      else gbSetUp(gbN);
    }
    if (gbState == 3) {
      if (wgiRt.load() > 0.5) { gbState = 2; gbStart = now; gbSetUp(gbN); }      // swinging again: back to full hold
      else { float t = std::fmin(1.f, float(now - gbBlendStart) / 2000.f), up[3] = {gbN[0] * (1 - t), gbN[1] * (1 - t) + t, gbN[2] * (1 - t)};
        float l = std::sqrt(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]); for (float& c : up) c /= l;
        if (t >= 1.f) gbEnd("blend_done", now, true); else gbSetUp(up); }
    }
  }
  prevVy = vy; prevOk = vOk;
}
void gbRestoreNow() { sgSubActive.store(false); sgAirPhase.store(false); gbAirCam.store(false); sgLocked.store(false); gbHoldA.store(false); if (gbActive) { gbWrite(kGravVanilla); gbActive = false; } gbState = 0; }
void sgStatus(std::uint64_t now) { static std::uint64_t last = 0; static unsigned logs = 0; if (!sgTarget || now - last < 20000 || logs >= 20) return; last = now; ++logs;
  char line[900]; int len = std::snprintf(line, sizeof line, "WS_SET_GRAVITY_STATUS calls=%llu distinct_blocks=%u hero_block=0x%llx substituted_calls=%llu per_block=",
    (unsigned long long)sgCalls.load(), sgDistinct.load(), (unsigned long long)gbFound.load(), (unsigned long long)sgSubCalls.load());
  unsigned nn = sgDistinct.load(); for (unsigned i = 0; i < nn && i < 24 && len > 0 && len < int(sizeof line) - 40; ++i)
    len += std::snprintf(line + len, sizeof line - std::size_t(len), "%s%llx:%llu", i ? "," : "", (unsigned long long)(sgSeen[i] & 0xFFFFFF), (unsigned long long)sgCount[i].load());
  msm2::writeLog("%s", line); }
struct ChargeMeasure { std::uint64_t release{}, hold{}; float y0{}, apex{}; bool active{}; } cm; unsigned cmLogs{};
void m8ChargeMeasure(const msm2::Snapshot& hero, std::uint64_t now) {
  auto rel = jiRelease.load(), pr = jiPress.load();
  if (rel && rel != cm.release && rel > pr && hero.hero && now - lastWsMs.load() > 1500 && now - rel < 300) {
    if (cm.active && cmLogs < 40) { ++cmLogs; msm2::writeLog("WS_CHARGE_MEASURE hold_ms=%llu rise_m=%.2f cut_by_next_release=1 read_only=1", (unsigned long long)cm.hold, cm.apex - cm.y0); }
    cm = {}; cm.release = rel; cm.hold = rel - pr; cm.y0 = cm.apex = hero.position[1]; cm.active = true; return; }
  if (!cm.active) return;
  if (hero.hero) cm.apex = std::fmax(cm.apex, hero.position[1]);
  if (now - cm.release > 3000) { cm.active = false; if (cmLogs < 40) { ++cmLogs;
    msm2::writeLog("WS_CHARGE_MEASURE hold_ms=%llu rise_m=%.2f read_only=1", (unsigned long long)cm.hold, cm.apex - cm.y0); } }
}
void m8JumpConfig(const msm2::Snapshot& hero, std::uint64_t now) {
  if (!killed.load() && hero.hero && !jiStarted.exchange(true)) { if (HANDLE h = CreateThread(nullptr, 0, jiPoll, nullptr, 0, nullptr)) CloseHandle(h); }
  if (!ws::drive::jumpConfigWrite.load() || killed.load()) return;
  int st = jcState.load();
  if (st == 0) { if (!hero.hero) return; jcState.store(1);
    if (HANDLE h = CreateThread(nullptr, 0, jcScan, nullptr, 0, nullptr)) { CloseHandle(h); } else jcState.store(3);
    return; }
  if (st != 2) return;
  bool ws = engagedManager.load() != 0 && now - lastWsMs.load() < 150;
  if (ws && !jcApplied && jcWrites < 2000) { jcWriteAll(true); jcApplied = true; }
  else if (!ws && jcApplied && now - lastWsMs.load() > 1500) { jcWriteAll(false); jcApplied = false; }
}
void jcRestoreNow() { if (jcState.load() == 2 && jcApplied) { jcWriteAll(false); jcApplied = false; } }
}
namespace msm2::camera_ref {
void configure(bool verified, std::uintptr_t image, std::size_t imageSize) {
  if (!verified || !image || imageSize <= kEntryRva + 64) { writeLog("WS_CAMERA_REF admitted=0 reason=build_or_image_unverified"); return; }
  sgInstall(image, imageSize);
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
  if (installed.load() && imageSize > kSetterRva + 64) {
    setTarget = image + kSetterRva; DWORD64 sb = 0; auto* sf = RtlLookupFunctionEntry(setTarget, &sb, nullptr);
    unsigned char se[64]{}; char sh[65]{};
    bool sok = sf && sb == image && sf->BeginAddress == kSetterRva && readMemory(setTarget, se, sizeof se) && sha256(se, sizeof se, sh);
    setYawLock = sok && !std::strcmp(sh, "8960cb933fa8809c7afd629212b2a7952230658ec7471a7279504d7a866ff55f");
    if (sok && MH_CreateHook(reinterpret_cast<void*>(setTarget), reinterpret_cast<void*>(&ws_camset_thunk), &ws_camset_original) == MH_OK)
      setInstalled = MH_EnableHook(reinterpret_cast<void*>(setTarget)) == MH_OK;
    writeLog("WS_CAMERA_SETTER admitted=%u rva=0x3350220 pdata_begin=%u entry_sha256=%s mode=capture_plus_ws_preserve_flags pinned_sha_admitted=%u", unsigned(setInstalled.load()), unsigned(sf && sf->BeginAddress == kSetterRva), sh[0] ? sh : "none", unsigned(setYawLock.load()));
  }
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
  if (engagedManager.load()) {
    auto since = now - lastWsMs.load(); auto start = exitBlendStart.load();
    if (gbHoldA.load()) since = 0;   // M10o: keep the WS camera while the wall gravity is held
    if (since < 150) { if (start) exitBlendStart.store(0); }
    else if (!start) exitBlendStart.store(now);
    else if (camUpSettled.load() || now - start > kExitBlendMs) { exitBlendStart.store(0); camUpValid = false; restore(camUpSettled.load() ? "wallsprint_ended_up_settled" : "wallsprint_ended_blend_timeout"); }
  }
  // (Disabled by default: actor_writer.cpp now starts with the accepted offset 0xD34, so the guard below returns at once.)
  // Read-only scan on the ground (no WS in the last second), moving: the camera forward is the unit vector whose
  // horizontal part is parallel to travel for a forward stick and perpendicular for a sideways stick
  // (travel = sign(sx) * h * cross(forward, up)). Accepted once per process; then used for camera-relative WS input.
  selectCamera(snapshot, now);
  m6GroundTurn(snapshot, motion, stick, now);
  m6Framing(snapshot, motion, now);
  m6SetterLog(snapshot, now);
  m6Exit(snapshot, now);
  m8JumpTrace(snapshot, motion, now);
  m8JumpConfig(snapshot, now);
  m8InputHooks(now);
  m8WgiHooks(now);
  m8ApiHooks(now);
  m8ChargeMeasure(snapshot, now);
  m9VelocitySearch(snapshot, motion, now);
  m10GravityTest(snapshot, motion, now);
  sgStatus(now);
  m5eDistance(snapshot, now);
  m5ePitch(snapshot, now);
  if (now - statusLastMs >= 5000 && statusLogs < 40) { statusLastMs = now; ++statusLogs;
    float f[3]{}; bool fOk = ws_camref_this && ws_camref_this <= UINTPTR_MAX - 0x1000 && msm2::readMemory(ws_camref_this + 0xD34, f, sizeof f);
    msm2::writeLog("WS_CAMERA_DIAG_STATUS installed=%u killed=%u manager=0x%llx raw=0x%llx callers_seq=%llu hero=0x%llx pos=%.1f,%.1f,%.1f cam_f_read=%u cam_f=%.3f,%.3f,%.3f dist_slots=%u pitch_samples=%u pitch_range=%.2f..%.2f pitch_logs=%u engaged=%u",
      unsigned(installed.load()), unsigned(killed.load()), (unsigned long long)ws_camref_this, (unsigned long long)ws_camref_raw, (unsigned long long)ws_camref_seq, (unsigned long long)snapshot.hero, snapshot.position[0], snapshot.position[1], snapshot.position[2],
      unsigned(fOk), f[0], f[1], f[2], distCount, pitchSamples, pitchMin, pitchMax, pitchLogs, unsigned(engagedManager.load() != 0)); }
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

void kill() noexcept { gbRestoreNow(); jcRestoreNow(); killed = true; ws_wallsprint_update_listener.store(nullptr); restore("kill"); }
void stop() noexcept {
  gbRestoreNow(); jcRestoreNow(); killed = true; ws_wallsprint_update_listener.store(nullptr); restore("stop");
  for (int m = 0; m < 4; ++m) { if (xgTarget[m]) MH_DisableHook(xgTarget[m]); if (xsTarget[m]) MH_DisableHook(xsTarget[m]); }
  if (sgTarget) sgManualUnhook();
  if (wgiReadTarget) MH_DisableHook(wgiReadTarget);
  if (wgiVibTarget) MH_DisableHook(wgiVibTarget);
  for (int k = 0; k < kApiN; ++k) if (apiTarget[k]) MH_DisableHook(apiTarget[k]);
  if (setInstalled.exchange(false)) MH_DisableHook(reinterpret_cast<void*>(setTarget));
  if (installed.exchange(false)) MH_DisableHook(reinterpret_cast<void*>(target));
}
}
