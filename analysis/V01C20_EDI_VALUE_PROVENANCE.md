# c20 local EDI reaching definitions

**ACTUAL-GAME RESULT ACCEPTED:** see `V01C20_RUNTIME_RESULT.md`. The prepared bounded local EDI analysis succeeded on the authenticated actual-game callee. EDI at `0x2B2358E` has one exact local reaching definition from a memory load at `0x2B2356F`, using analyzer `root=16` (`globalRoot`, exact RIP-relative executable-image address) at image-relative offset `0x0C323788`. Local value coverage is complete for the analyzed call input. Whole writer/effect coverage is still blocked by an earlier decoded read at `0x2B23578`; XYZ/W causality, ownership and writability remain unproven.

## Accepted actual c20 result

```text
site=0x2B2358E
value register=EDI
producer=0x2B2356F
producer kind=memory_load
origin kind=memory_load
root=16 (globalRoot / exact RIP-relative image address)
offset=0x0C323788
reaching definitions=1
all local reaching states examined=1
all local reaching states same origin=1
value provenance exact=1

whole writer call states examined=0
relevant Transform effects resolved=0
next exact dependency=0x2B23578
classification=earlier_read_provenance_dependency
```

The semantic meaning of the image-global location and `Transform+0x10C` remains **UNKNOWN**. This result is value provenance, not an engine field-name recovery.

## VERIFIED input

C19 actual A3844ms/E4094ms passed verified Deinit/retention/Up1/stale0/hard0/Position0,8entries/8complete maps each, XYZ7/sourceW0/destW1 and clean cleanup. Exact `0x2B2358E` is MOV dword `[RCX+0x10C],EDI`, original writer RCX/Transform root; direct Position/identity overlap0.

C20 actual A/E-style capture also preserved the safety/orientation baseline: session1 ~4.22s and session2 ~4.39s, verified Deinit/retention, Position writes by MOD0, hard0, cleanup restored. Session1 had one bounded 103ms stale suspend/resume that recovered; session2 had none.

## Exact local slice

Existing exe2.810.0.0/SHA, writer/caller/direct call, callee0x2B23560..0x2B235E9/hash2d62e54b270c46f4babc8120a7e56123e3fe467ca383895d57a989f853095a9d/full current code/boundary and c18 CFG guards remain. C20 opts in only the authenticated MOV dword from EDI at0x2B2358E.

Starting at the exact target, a reverse pass over the already validated forward local CFG marks instructions on paths to it. A bounded path-sensitive evaluation follows relevant local Jcc legs from callee entry and stops each path at the target, never expanding a call, following an indirect edge/backedge or traversing another function. Unsupported prior instruction/stack/value/chain/cap effects refuse exactly.

## Raw bits, not pointer types

The sidecar gives each callee-entry GPR its own raw low32 input token. These are value identities, never address roots or guessed ABI types. MOV32/MOV64 register copies preserve low32 identity. Existing legacy State/Slice address provenance is unchanged and never receives these tokens. No cross-function caller/producer reaching definition is inferred.

Immediate MOV/XOR-self yields constants; supported64bit ADD/SUB-immediate and simple64bit LEA retain exact affine low32 expressions modulo2^32. Opaque/partial-register/vector-derived/unmodeled computations remain unresolved. Stack saves/loads preserve low32 origin only for exact matching bounded spans.

Untracked memory origins are symbolic loads at exact decoded addresses. For the actual accepted c20 result, `root=16` is not a register root: source implementation defines it as `globalRoot`, the exact RIP-relative executable-image address class. The accepted origin offset is `0x0C323788`. No semantic name is assigned to that image-global value.

## Coverage and continuation

`MOVEMENT_CALLEE_VALUE_CONTRACT` local coverage and whole writer coverage remain deliberately separate. The actual c20 value contract passed locally, while `all_writer_call_states_examined=0` because protected-effect analysis stopped at `0x2B23578`.

The current analyzer labels the blocker `earlier_read_provenance_dependency`: the read is before the native write at `0x2B2358E`. The next diagnostic must classify the read's exact address/root/offset/width and value carrier, prove path ordering, and determine whether its pre-write value can affect post-callee XYZ/W-relevant state or whether cross-invocation persistence must be modeled. Ordering alone must not be used to guess irrelevance or causality.

Only after the read/effect question is resolved may the existing protected-effect/XYZ/W proof continue to the next exact blocker or completion.

## Safety boundary

Native lifecycle/orientation, raw entry thunk/ABI/pairing/accounting/finalization/cleanup, exact guards and logger remain. Current gate:

```text
writable_source_proven=0
caller_owned=0
ownership_exclusive=0
source_lifetime_exclusive=0
downstream_conflicts_proven_absent=0
Position/source/input/movement writes=0
```

No producer0x351EB00 traversal, movement/adhesion/camera/animation/physics, PAGE_GUARD/data-page mutation, guessed game call or raw private body publication is authorized.

## Accepted workspace

Use the accepted independent clone under `<game>/Mods Library/Mods-Developing/MSM2-WS/_source`, shared Overstrike, versioned root script and `MSM2-WS/logs` path. Public ZIPs stay outside Mods Library. `main` is not routinely merged into the current implementation line.

Next bounded work is specified by `docs/CODEX_NEXT.md`.