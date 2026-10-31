# r7 — comparable native-source ON/OFF observation (diagnostic only)

2026-10-31 JST; branch codex/v01c20-edi-value-provenance; baseline64056d4.
WALLRUN_SUCCESSOR_OWNER_RESULT_20261031.md accepts20-byte TEST-DL successor.
Stop scalar byte/callee traversal. No more offline capture request or down fix.

## Actual configuration / lifecycle findings

telemetry.cpp::settings reads WALLSPRINT_NATIVE_SOURCE_OVERRIDE ONCE from
WallSprint.ini adjacent to the loaded module (GetModuleFileName(instance)).
Master WALLSPRINT_ENABLE stays1. F8 calls native-source/trace/input/writer kill,
disables lifecycle publication and remains latched until process restart; it
cannot supply an equivalent OFF control. No new hotkey/toggle framework added.

Previously sourceOverride0 made admitted false, returning before armLifecycle;
worker tick also returned on killed/not-admitted before snapshots/window samples.
R6 ON-only windows therefore cannot be used as existing OFF evidence.

r7 separates readonly comparison readiness from source-store admission. The exact
same startup build/caller/lifecycle patch/input/compact guards remain necessary;
source admitted = readiness AND requested override. OFF can arm EXISTING native
lifecycle token observation while source enabled flag stays0, killed remains true,
PathB admission false and positive source publications/A-B commits stay0. Empty
revocation/token publications are not orientation permission; counter counts only
active orientation publications. Reset/watch diagnostic admission remains false
OFF. No new hooks, pointers, input API, original game invocation or memory fields.

## One buffer / comparable evidence

Reuse the existing value-only motion Window:100ms minimum/distinct native update,
16 retained samples/2s,16 windows/session, final partial flush, explicit overwrite/
throttle/budget suppression. Same buffer, eligibility checks and output in both
modes, not a second observer framework. ON supplies existing snapshot/token/N398
values already acquired by worker. OFF acquires those same already-authenticated
snapshot and owner+0x398 fields behind native context/code/input/freshness guards,
then rechecks lifecycle identity. No sample after Deinit/emergency/closed identity.

Common validation: active verified lifecycle/session/owner/thread/generation/init/
accepted-collision epoch, <=100ms native freshness, stable Hero/Transform identity,
finite .95..1.05 normal with absY<=.20 length and stable surface dot>=.995,
proper actual Actor axes. Normal is validated independently of Actor Up: vanilla
OFF Actor Up may remain world-up; never construct a WS basis or force Actor Up
onto wall. Invalid/missing/stale samples clear baseline or close current diagnostic
session with bounded reason records (64/process plus total counter). New sessions
reset identity/baseline/caps; same-session identity close never revives. Stale
recovery must rebase, never bridge a displacement across the gap. F8/script disable
stops observations too, not a treatment switch. Invalid startup yields no evidence.

WALL_MOTION_WINDOW schema2 includes mode, actual source admission, actual active
publication state/count (process scope), A/B commit counts, session/generation,
native update/update timestamp/init timestamp, monotonic worker time, dt, world
position delta/tangent, pinned projection normal AND current sampled normal,
actual Actor full basis. Target/published heading unavailable until actual ON
observer target is valid; always unavailable OFF, never interpreted as input.
Frame counters are as sampled, not promised future commits. The summary
legacy lifecycle_state field is SOURCE ownership (OFF is Dormant0), not native
WallRun phase; use verified native lifecycle logs/update identity for native state. Existing heading
logs remain independent; no extra heading sample/vote/retarget is introduced.

ON source epoch guard -> identityPublication/publication stores retain adjacency;
comparison work happens AFTER these stores, not between final guard and store.
Heading math/constants/entry timing, native stale recovery/Deinit, A/B proper
basis/scales/metadata/Position-W,4096/C52/ownership remain. Diagnostic bursts can
perturb timing; owned regression is not proof of zero observer effect in game.
Packet values come from one worker snapshot plus guarded normal/token reads,
not an atomic engine frame, post-writer/animation measurement or raw stick intent.

## Exact safe selection / restoration method

Use source/tools/select_wallrun_comparison.py, reusing existing workspace layout,
Windows quiet check and OS-held operation ownership. It acts ONLY on installed
<game>/scripts/WallSprint.ini, while game AND manager closed. Validate sole root
r7 script/metadata and installed DLL equality to that script; original packaged
INI must have one override key. Other INI edits/mismatched DLL/duplicate scripts/
reparse/unknown backup fail closed. No archive/script/profile/source edits.
Backup exact original bytes under WS/_archive/WallSprint-comparison-original.ini;
ON/OFF changes only existing key, RESTORE restores exact backup. Atomic same-file
replacement, preserve other keys/newlines and unrelated files. Old backup with
other package INI refuses, never discarded. No shared environment code changed.

Overstrike reference443ec468b5a7a922771dce6bfa96396f56896dcb ScriptInstaller installs
members to <game>/scripts; LaunchGame only starts game with scripts argument,
not Install mods. This is STATIC reference behavior, not proof of owner r7 UI.
Actual startup WALL_MOTION_CONFIG prints the loaded INI path/requested mode;
WALL_MOTION_COMPARISON logs observation and source admissions. Those must confirm
selection before comparison acceptance. Missing/different runtime path refuses
selector; do not guess or copy files elsewhere.

Workflow: Update/install r7 normally ON; close game/manager; run selector; reopen
manager and use Run modded game ONLY. Do not Install mods/Update-WS again after
selection, because install overwrites INI with packaged default ON. Game restart
is mandatory; never edit live config or press F8 for control. Restore via same tool.

```powershell
$game = "C:\SteamLibrary\steamapps\common\Marvel's Spider-Man 2"
$tool = Join-Path $game 'Mods Library\Mods-Developing\MSM2-WS\_source\source\tools\select_wallrun_comparison.py'
py -3 $tool --game-root $game --mode ON
```

Do NOT run all modes together. For the separate OFF attempt only, after fully
closing game/manager (same PowerShell variables):

```powershell
py -3 $tool --game-root $game --mode OFF
```

After comparison, close game/manager and restore exact original bytes:

```powershell
py -3 $tool --game-root $game --mode RESTORE
```

## Compact comparison specification for Chat (not an improvement test)

Separate fresh game processes/attempts, same character/flat wall/contact point,
starting body and camera orientation; same other mods/angles/speed settings.
Use identical owner-reported down and lower-diagonal holds after stable native
WallRun. Do not use F8 or a mid-attempt treatment switch. Input report is not raw
mapped intent proof. Record what the owner attempted and visible motion separately.
ON acceptance: startup modeON/observation admitted1; active source publications
and A/B commits positive in compared interval. OFF: modeOFF/observation admitted1,
source admitted0, active publication/commit counts0, target unavailable; confirmed
comparable packets actually present. Any config/path/admission/identity/stale/
Deinit/no-sample mismatch invalidates comparison, not evidence of a motion cause.
R7 does not keep RT-neutral stance, speed descent, tune angles or fix animation.

Return separate timestamped MSM2-WS-v0.1c38r7-YYYYMMDD-HHMMSS.log files (mode inside
log) plus compact owner observations/video if Chat requests it. No extra test run
from this implementation handoff; Chat supplies concise numbered owner comparison.
Actual Windows selector/loader/game and directional causality remain UNAVAILABLE.
