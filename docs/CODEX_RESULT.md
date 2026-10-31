# Codex result — downward intent / native limit design

2026-10-31 JST. Branch `codex/v01c20-edi-value-provenance`.
Baseline `02db844c4b472f9fc2250ad6eb85df1b45e9b548`.

## STATUS / scope

Focused source/static research and reviewable design **completed**. Behavior
implementation **BLOCKED on input provenance and native selection/consumer proof**;
not a gameplay success. No runtime/version/package/config/updater/SF changes,
new pointer reads/hooks/writes, ABI guesses, scalar traversal or game test.

## CHANGED / findings

Added `analysis/DOWNWARD_INTENT_IMPLEMENTATION_DESIGN.md`; prepended current
PROJECT_STATE without deleting history; this result updated. CODEX_NEXT unchanged.
Accepted r7 owner comparison supersedes older pending claims: OFF zero source
publications/A-B commits but descent still fails. Current heading only follows
actual displacement, not intent. TurnMaxDown12 versus retained -12degree slopes
is correlation, not selected native field consumption or units proof.

Design enumerates actual runtime source touchpoints, independent intent versus
velocity/pose/animation, file schema versus native selection, required exact-build
ABI/ownership/frame/freshness gates, working lateral/upper fallback and repeated
turn lifecycle. Latest task expansion53655ba and owner steering incorporated: compare A native
ground Sprint with wall-relative support/up/gravity, B WS surface-relative native
collision/contact request, C WR constraints. Preferred evidence-gated research
route is A, arbitrary-up UNKNOWN; historical posture tests did not reject it.
Mapped-intent design retained across all routes, no motion injection/WSU copying. Includes
later authorized patch's ON/default restoration and exact-version/log acceptance
requirements. No present support claimed.

Accessible primary Overstrike source pinned at
`443ec468b5a7a922771dce6bfa96396f56896dcb` establishes serialization only.
Bounded available ZIP inventory102 found no matching WSU members; no new WSU
disassembly performed. Existing supplied report remains a lead. ONE proposed
first discriminator is a bounded current-build ground-Sprint support/up/gravity
consumer review, not initiated or requested automatically. SwingCore input-context
producer remains a missing intent prerequisite, not a second immediate request. No new owner capture/download required by this task.

## Exact checks / expected / actual

Static/source checks below are research checks, not runtime regression tests.
Counts are check groups, not assertions. Expected empty `rg` matches use exit1.

1. `git status --short` and `git rev-parse HEAD`: expected preserved task branch
   state; actual baseline above, only five pre-existing untracked archives/bundle.
   `git fetch origin codex/v01c20-edi-value-provenance` followed by
   `git merge --ff-only origin/codex/v01c20-edi-value-provenance` had advanced
   clean prior baseline to02db844; no main merge.
2. `cat source/include/msm2/sdk_bridge.hpp`;
   `sed -n '1,95p' source/src/msm2/sdk_bridge.cpp`;
   `sed -n '447,537p' source/src/NativeHooks/native_source_override.cpp`;
   `cat source/include/Math/reversal_heading.hpp`;
   `rg -n 'heading.sample|readNormal|snapshot|Transition|writeStack|comparisonSample' source/src/NativeHooks/native_source_override.cpp`:
   expected trace actual producer->heading->orientation source; actual no input
   producer/physical velocity in Snapshot, displacement target and native gates
   confirmed. PASS, no claim of executable game behavior from source inspection.
3. `rg -n 'TurnMaxDown|SteeringSetupStandard|SteeringSetupIndoor' source/src source/include source/vendor`:
   expected inspect existing bindings; actual no matches (exit1), no proven
   current native field consumer. PASS as inventory, not proof no consumer exists
   elsewhere in the game.
4. `git -C /tmp/ws-steering-overstrike-review rev-parse HEAD`;
   `rg -n 'TurnMaxDown|SteeringSetupStandard|SteeringSetupIndoor' /tmp/ws-steering-overstrike-review -g '*.cs' -g '*.json' -g '*.xml'`;
   read `DAT1/Files/Config.cs`, `DAT1/Sections/Config/ConfigBuiltSection.cs`,
   `ConfigReferencesSection.cs`, `DAT1/Sections/Generic/SerializedSection_I30.cs`:
   expected separate serialized schema from consumption; actual pinned source
   identity, no named consumer matches (exit1), schema/typed-read proof only. PASS.
5. Read-only archive inventory: exact discovery command
   `rg --files /workspace /tmp -g '*.zip' -g '*.modular' -g '*.script'`;
   Python `zipfile.ZipFile` directory inspection excludes >512MiB files and
   >16384-member archives; case-insensitive member suffixes SwingCore.dll,
   wallslide.dll, sc_core.script. Expected locate available reference input;
   actual102 archives,0 matching archives. PASS inventory. New reference-producer
   disassembly **UNAVAILABLE**, files not identified by bounded inventory; opaque
   renamed/deeper nested/private-chat files not excluded.
6. `git diff --check` and the inline Python docs-scope/link check (recorded below):
   expected whitespace clean, only three intended Markdown files, existing local
   analysis link targets and task text unchanged; actual PASS.

7. `cat source/vendor/SM2ScriptTemplate/game/Actor.h`;
   `cat source/vendor/SM2ScriptTemplate/game/Transform.h`;
   `rg -n -i 'gravity|sprint|support|requestsimplemotion|ground|contact' source/src source/include source/vendor/SM2ScriptTemplate/game -g '*.cpp' -g '*.hpp' -g '*.h'`;
   read `analysis/INITIAL_WALL_AS_GROUND_REVIEW_20261031.md`,
   `docs/FEATURE_ROADMAP.md` and Wisdoms reference architecture section:
   expected distinguish typed controller proof from transform/generic component/
   fixture/label evidence; actual no identified authenticated ground support-Up/
   gravity consumer. Three-route comparison and first discriminator completed,
   arbitrary-up remains UNKNOWN. PASS research check, not ground-route feasibility.

PASS7 check groups / SKIP0 / UNAVAILABLE1 (new WSU producer trace) / BLOCKED1
(behavior implementation admission, outside authorized implementation scope).
Native consumer/active-selection proof remains UNKNOWN; static searches do not
convert that missing proof into PASS. Native Windows/game acceptance not run:
this task authorizes documentation only. Build/portable/native/Wine/package
suites not run because no code/helper/runtime/package change; prior suite results
are not presented as freshly executed. No new offline helper added.

Reproducible docs check used:
```bash
python3 - <<'PY'
from pathlib import Path
import re, subprocess
expected={'analysis/DOWNWARD_INTENT_IMPLEMENTATION_DESIGN.md',
          'docs/PROJECT_STATE.md','docs/CODEX_RESULT.md'}
changed=set(subprocess.check_output(['git','diff','--name-only','HEAD']).decode().splitlines())
new=Path('analysis/DOWNWARD_INTENT_IMPLEMENTATION_DESIGN.md')
assert changed | {str(new)} == expected, changed
for target in re.findall(r'\]\(([^)]+)\)',new.read_text()):
    assert (new.parent / target).is_file(), target
assert not subprocess.check_output(['git','diff','HEAD','--','docs/CODEX_NEXT.md'])
print('PASS docs-only scope, analysis links, unchanged task')
PY
```

## Shared knowledge / publication

Wisdoms `docs/WALLSPRINT_REFERENCE_MODS.md` explicitly qualifies the historical
preset-only evidence and outdated experiment sequence; accepted r7 comparison,
input unknowns and schema/consumer separation preserved. Commit
`f9df414c9e99c40f7d8f0d4faacf4ced6b44b9a8` pushed; independent `ls-remote` agrees.
No raw user logs, proprietary function bodies, binaries or personal paths added.

Wisdoms follow-up architecture qualification also pushed; final SHA recorded
with readback below.

WallSprint implementation/documentation commit and independent publication
readback recorded after commit below. Runtime remains v0.1c38r7/package0.1.63;
no new artifact/output package. Intended output is the analysis document above.

## NEED

No Update-WS, Spider-Man2 launch, unchanged r7 run, OFF video or scalar/config
probe. ONE future evidence proposal is documented, not automatically authorized.
No current owner operation required. Future behavior remains gated on both
independent intent and native capability evidence.
