# V8 WallRun `this+0x250` consumer result — 2026-11-01

Branch: `codex/v01c20-edi-value-provenance`.
Runtime remains `v0.1c47`, package `0.1.72` for the accepted owner run.

Owner returned `MSM2-WS-wallrun250-consumers-20261101-134046.json`.

## Admission

Schema/result:

- schema `8`;
- `WALLRUN_0X250_AUTHENTICATED_UPDATE_CONSUMER_SLICES`;
- exact EXE SHA `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- exact authenticated Update SHA `e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f`;
- Capstone `5.0.6`.

Both known `WallRun.this+0x250` reads were rediscovered as 4-byte reads through the authenticated RSI owner alias.

## Consumer A — `0x1424585`

Static instruction relation:

- `0x142457D`: `vmovss xmm11, dword ptr [rip+...]`;
- file-backed initial float at target RVA `0x5303DE8`: `0.5f` (`0x3F000000`);
- `0x1424585`: `vcomiss xmm11, dword ptr [rsi+0x250]`;
- intervening `MOV` instructions do not overwrite comparison flags;
- `0x1424596`: `setb byte ptr [rbp+0x120]`.

For ordinary finite operands, this stores true when:

`WallRun.this+0x250 > 0.5f`.

For unordered/NaN operands, `VCOMISS` also sets CF and therefore `SETB` is true. The exact stack-local meaning of `[rbp+0x120]` is still UNKNOWN because V8 did not follow the local value to its later use.

Classification:

- field is consumed by a `0.5f` threshold comparison: **VERIFIED-STATIC**;
- local boolean semantic name/purpose: **UNKNOWN**.

## Consumer B — `0x142460E`

Static instruction relation:

- `0x14245FB`: `vmovss xmm0, dword ptr [rip+...]`;
- file-backed initial float at target RVA `0x5337098`: `0.7099999785f` (`0x3F35C28F`), i.e. authored approximately `0.71f`;
- `0x142460E`: `vcomiss xmm0, dword ptr [rsi+0x250]`;
- `0x1424616`: `jae 0x142464F`.

For ordinary finite operands, the branch to `0x142464F` is taken when:

`WallRun.this+0x250 <= 0.71f`.

The fallthrough block is therefore entered only when the field is greater than `0.71f` (or the compare is unordered).

That fallthrough block is additionally gated by other state, calls `0x29BD5F0`, and computes `r15b` from another scalar relation ending in a comparison against a file-backed initial `2.5f`. V8 does not establish semantic names for any of those values.

Classification:

- field is consumed by an approximately `0.71f` threshold/control-flow gate: **VERIFIED-STATIC**;
- downstream block semantics: **UNKNOWN**.

## Relation to c47 runtime

The retained c47 `+0x250` samples were mostly zero/negative, with observed pulses down to about `-0.208` and only a very small positive sample near `+0.0057` before zero. Therefore every retained finite c47 sample is below both `0.5` and `0.71`.

This means the existing 100 ms samples do not show either threshold becoming active during the reported reversal/down-oscillation sequence. However, c47 sampled asynchronously at 100 ms, so a short positive excursion above either threshold between worker samples is not excluded.

Thus:

- timing correlation of negative pulses with reversal/down oscillation remains **OBSERVED-CORRELATION**;
- the claim that those pulses toggle either authenticated threshold consumer is **NOT PROVEN**;
- simple causation from the retained pulses to these gates is weakened, but not rejected until exact Update-entry crossing counts are observed.

## Semantic hypothesis

The pair `0.5` and approximately `0.71` is numerically compatible with common vector-alignment/dot-product angle thresholds (for example, cosine-like cutoffs), and the runtime field range around zero/negative values is also compatible with a signed alignment quantity. This remains only a **HYPOTHESIS**. V8 did not prove a dot product, an angle, units, producer identity, or field name.

Do not rename the field or authorize a write on this basis.

## Decision

Do not continue the exhausted V3-V7 RCX-owner producer chain and do not return to global writer scans.

Next minimum runtime discriminator:

- keep all gameplay writes OFF;
- at the already-installed authenticated WallRun Update pre-original callback, read only `this+0x250` on each actual Update call;
- count exact finite/unordered comparison outcomes for the two V8 thresholds;
- capture live threshold globals at admission and recheck them from the worker so the on-disk constants are not silently treated as runtime constants;
- log cumulative/final counts only from the worker thread;
- no field write, no new hook, no pointer chase, no game call.

Purpose: determine whether `+0x250` ever crosses either authenticated consumer threshold during the exact reversal/down-oscillation sequence, eliminating the 100 ms sampling-gap ambiguity with one bounded runtime run.
