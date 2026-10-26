# c49 native VCH boundary observation — stage2 only

From this folder in native Windows PowerShell, run one game-free command:

```powershell
.\continuation-witness.exe --position --mixed-search-vch --json
```

Return the compact JSON/output. If this folder is missing, first `git pull
--ff-only` in the independent `_source` clone on
`codex/v01c20-edi-value-provenance`. No Update-WS, game, Overstrike, scan, A/E,
WallRun, game log or mitigation-setting changes are required.

This is the c49 observation-only standalone, **not** the c47 executable with a
new filename or a game MOD. It uses allocated owned memory, an owned thread,
the unchanged production handler and a separate synthetic foreign B2 owner.
The exact remaining OS question is whether actual native hardware-DB dispatch
calls VCH AFTER the foreign owner clears B2/chooses EXECUTION, and whether that
callback sees the same entire final CONTEXT and pre-reserved immutable identity.
No observer changes saved RIP or DRs. Only the actual engine clears B0 and the
separate foreign owner clears its B2 cause. Registration alone is not proof.

Cloud Wine: six reset/Position x close/deadline/kill runs registered real VCH but
callback count stayed0. These remain UNAVAILABLE. Older c40 callback0 and c48
mixed-SEARCH resurrection counterexamples remain unchanged, independently run.

This binary deliberately has **no VCH-to-c41 witness bridge**: CODEX_NEXT permits
that only after stage2's boundary succeeds; no native evidence is available yet.
If actual VCH dispatch/order/context checks succeed, JSON reports
`native_boundary_observed=true` and reason
`boundary_observed_bridge_not_attempted_native_stage2_review_required`.
Primary still stays `VCH_SEARCH_CONTINUATION_BRIDGE_UNAVAILABLE`, with
`bridge_implemented=false`, binding/acknowledgment/post-witness result=null.
Return that evidence for the next reviewed stage; it is not a feasibility PASS.

If VCH is not dispatched, reason is
`registered_VCH_not_dispatched_for_mixed_SEARCH_DB`. If it is dispatched but exact
ordering/identity/full-CONTEXT comparison fails, primary is
`VCH_SEARCH_CONTINUATION_BRIDGE_FEASIBILITY_FAIL` with the rejection reason and
observed stages. No unknown field is fabricated. Exit3 denotes an observed
contract/assertion failure,77 an unsupported/unknown prerequisite; exit0 can mean
UNAVAILABLE and is never itself a native success certificate.

Debt remains pending1 throughout close request and return; no live final
production poll/retirement is attempted without a witness. Only actual worker
termination on its retained kernel handle allows terminal test cleanup. No
post-witness cleanup/no-resurrection result is claimed. Native observation of
this single context does not prove nested/multiple/multithread coverage or that
another third-party VCH cannot run/mutate later. Synthetic contract tests are
separate from native runtime evidence.

Optional `--timeout` or `--kill` changes only the owned cleanup request scenario;
omitting `--position` selects the reset fixture. The first requested owner run
is the single command above, not a matrix or game test. All waits retain2000ms
bounded fixture holds. This intentionally schedules an exception interval and
is not a production handler synchronization pattern.

PROBE_MANIFEST.json pins the exact tested PE and source/tool inputs. Reproduce:

```bash
bash source/tools/build_windows.sh
bash source/tools/test_vch_search_bridge.sh
```

The tested binary is source/build/windows/vch_search_bridge_smoke.exe. The
published name follows the owner's preferred command; SHA/byte identity is
verified after the full hook chain. Warnings-as-errors, existing mitigations and
all historical native evidence are preserved. Licenses are included. Runtime
v0.1c38/package0.1.56; production_integrated/exact_slot_watch_enabled/game_used=false.
Detailed evidence: ../../V01C49_VCH_SEARCH_BRIDGE.md and task status/gate/report.
