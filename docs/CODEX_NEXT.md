# Next task — WallSprint v0.1c10 Secondary Publication Race Repair

## Implementation base

Use published c9 exactly:

```text
base branch: codex/v01c9-secondary-path-b-thread-admission
implementation commit: 0c276126ccac8e3bd2a6dd1d850da63e80bc35ce
published c9 head: b865dabfa81626248ea8a8ae56a1dd1c992d7301
```

Pre-created branch:

```text
codex/v01c10-secondary-publication-race
```

Read first:

- `analysis/V01C9_RUNTIME_RESULT.md`
- `analysis/V01C9_E_RUNTIME_RESULT.md`
- `analysis/SECONDARY_PATH_B_THREAD.md`
- `docs/PROJECT_STATE.md`
- c9 `docs/CODEX_RESULT.md`

## Authoritative c9 E-wall result

The E regression confirmed generic diagonal orientation:

```text
N398 ≈ (-0.70710677, 0, +0.70710677)
target_up ≈ (-0.70710671, 0, +0.70710683)
```

Final lifecycle still passed:

```text
wallrun_duration_ms=4157
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
verified_deinit=1
path_a_writes=231
path_b_writes=228
retention_pass=1
last_actor_up_dot_surface=1
first_hard_kill=none
hard_kill_calls=0
```

The user visually saw stable orientation through the wall.

However, the log proves exactly two very short internal WorldUp flashes. The secondary-thread summary was:

```text
seen=112
admitted=110
overridden=110
rejected=2
unique_threads=16
```

Both rejected calls were:

```text
action=reject
reason=secondary_source_publication_changed
```

The first sequence was approximately:

```text
secondary Path B begins from active valid c9 publication
-> secondary_source_publication_changed reject
-> primary Path A still writes Surface Up in the same general interval
-> Actor observation ~10 ms later: actor_up=(0,1,0)
-> next common source call sees WorldUp source
-> LATE_COMMON_WORLDUP_OVERRIDE=1
-> Surface Up restored
```

The second sequence is equivalent near elapsed ~3.53 s.

There were exactly two `actor_up=(0,1,0)` observations and exactly two `secondary_source_publication_changed` secondary rejects.

## Current c9 code mechanism

`captureCopy()` reads an initial `Published p`, validates the secondary call, then later reads `Published final` and requires exact equality including:

```cpp
final.session == p.session
final.generation == p.generation
final.lifecycleEpoch == p.lifecycleEpoch
final.publishedMs == p.publishedMs
final.active
```

A normal worker/native Update may republish the same active WallSprint ownership identity with a newer `publishedMs` and updated `nativeUpdate` while the secondary callback is validating. c9 treats that benign refresh as `secondary_source_publication_changed` and allows the vanilla Path B payload to proceed. On the diagonal wall that vanilla payload is WorldUp and produces the observed short flash.

Do not simply remove the final validation or ignore publication changes.

## Objective

Make a secondary-thread Path B call tolerate a bounded, identity-preserving source publication refresh during validation, while continuing to reject or hard-close any real ownership/integrity change.

The result should eliminate the two WorldUp flashes without weakening c7/c8/c9 safety.

This task is still orientation-only. No Position correction or movement yet.

## Required work

### 1. Define semantic publication identity vs refreshable payload

Document and implement which `Published` fields define ownership identity and which may legitimately advance during one secondary callback.

At minimum, a refresh may be considered identity-preserving only if all safety-critical identity still matches:

```text
session
owner
hero
transform
generation
lifecycleEpoch
expected native thread
initMs / source generation origin
active ownership state
verified WallRun context identity
native lifecycle token owner/generation/thread/phase
accepted event epoch
collision epoch
```

The following may legitimately advance with a newer worker/native Update and should not by themselves force vanilla pass-through:

```text
publishedMs
nativeUpdate
elapsed
current interpolation/basis payload
```

Use the actual c9 structures and token semantics; do not rely on this list blindly if code proves a field has stronger meaning.

### 2. Bounded refresh/revalidation for secondary Path B

When the final publication no longer exactly equals the initial `p`, do not immediately reject.

Implement a bounded strategy such as one retry/refresh:

1. read latest publication;
2. prove it is the same active ownership identity;
3. re-run the existing token/freshness/current worker-context/source-integrity checks against that latest publication;
4. confirm RCX still equals the same active Transform;
5. confirm current callback caller/RSP/RDX/stack/compact source remain valid;
6. recompute the compact replacement using the **latest admitted basis**;
7. perform the guarded compact orientation store only if all checks still pass.

No loops without a strict bound. Prefer one refresh attempt unless tests demonstrate a need for a second bounded attempt.

Do not restart the 0.7 s delay or transition. Do not migrate the expected native thread. Do not synthesize an Update.

### 3. Preserve real-change rejection / hard-close

A refresh must NOT be accepted if any safety-critical identity actually changed, including:

```text
session/owner/Hero/Transform change
generation/lifecycle epoch change
verified Deinit / hard close
native token owner/generation/thread/phase change
accepted or collision epoch change
genuine stale publication under existing c7 rules
source/caller/writer/lifecycle patch integrity loss
compact proof loss
current callback caller or stack provenance failure
rollback failure
F8/script stop/shutdown
```

Unrelated foreign Path B traffic still gets no WallSprint payload and normally passes through without killing the active session unless it proves real owned-source corruption.

### 4. Recompute from latest basis before store

Do not validate `final` then store an `applied` block computed from stale `p.basis`.

After an identity-preserving refresh, recompute compact orientation from the latest `final.basis` and the current stack-local incoming block. Preserve row scales/metadata exactly as c9 does.

If the transition is already at t=1, latest basis should remain full Surface Up.

### 5. Add exact race telemetry

Add bounded diagnostics distinguishing:

```text
secondary_publication_refresh_seen
secondary_publication_refresh_admitted
secondary_publication_refresh_rejected_identity_change
secondary_publication_refresh_rejected_stale
secondary_publication_refresh_rejected_integrity
```

A first-event record should include old/new publication update numbers/timestamps and whether semantic identity matched, without dumping private full game bytes.

Extend final secondary summary with refresh counts.

### 6. Add a production-policy race test

Reproduce the c9 E failure deterministically:

1. active WallSprint session reaches full Surface Up;
2. secondary Path B callback starts with valid `p`;
3. before its final validation, primary worker publishes a newer snapshot for the same session/owner/Hero/Transform/generation/lifecycle identity;
4. only `publishedMs/nativeUpdate/elapsed/basis payload` advance normally;
5. secondary callback refreshes/revalidates and still overrides the compact source;
6. no WorldUp vanilla payload reaches the target;
7. no hard kill occurs;
8. expected native thread remains unchanged;
9. primary A/B continue afterward;
10. verified Deinit closes cleanly.

Also test negative races where the intervening publication changes:

- session;
- owner;
- Hero or Transform;
- generation;
- lifecycle epoch / Deinit;
- native token thread/owner/generation/accepted/collision epoch;
- source integrity;
- stale timeout.

Those must not receive secondary orientation stores.

### 7. Preserve c9/c8/c7 protections

Keep unchanged unless directly required by this race fix:

- exact executable/SHA admission;
- Path A thread/caller/stack policy;
- c7 stale suspend/resume;
- c8 immutable first hard-kill attribution;
- c9 secondary caller/target/current-thread-stack/compact proof checks;
- no secondary native-thread ownership migration;
- shared 4096 B store-attempt budget;
- only compact orientation bytes are written;
- Position writes remain zero;
- no direct Actor Transform write;
- no PAGE_GUARD/protection changes;
- no keepalive game call.

### 8. Human regression

Use the same diagonal E wall first.

Required log result after reaching full Surface Up:

```text
no actor_up=(0,1,0) observations while source lifecycle remains active
no secondary_source_publication_changed vanilla pass-through caused by a benign identity-preserving refresh
ORIENTATION_RETENTION_PASS=1
source_active_until_deinit=1
verified_Deinit clean close
first_hard_kill=none
Position writes=0
```

If a publication refresh race naturally occurs, it should show refresh-admitted telemetry and still override Surface Up.

After E passes with zero WorldUp flashes, run the large A wall once as a quick regression if practical.

## PASS criteria

Primary c10 PASS:

```text
E wall remains diagonal Surface Up
zero WorldUp actor observations after full orientation is reached
identity-preserving publication refreshes no longer cause vanilla Path B leakage
secondary Path B remains safe across multiple threads
verified Deinit is final clean close
source_active_until_deinit=1
hard_kill_calls=0
Position writes=0
```

Only after this should the next handoff define surface-relative movement input / movement-path ownership.

## Non-goals

Do not add:

- active adhesion correction;
- movement input rewriting;
- gravity replacement;
- animation;
- camera;
- wall jump;
- corners/ceilings;
- parkour;
- swing reconnect;
- combat.

## Required outputs

Update/create:

```text
analysis/V01C9_E_RESULT.md
analysis/SECONDARY_PUBLICATION_RACE.md
analysis/V01C10_GATE.md
README_TEST.md
docs/CODEX_RESULT.md
```

Package:

```text
WallSprint_v0.1c10_secondary_publication_race.zip
```

Follow `AGENTS.md` / `docs/DEV_RULES.md`. Do not commit raw user logs/video or private full captured game-function bytes.