# Reset smoke snapshot timing — real-PC evidence and test isolation

## VERIFIED user evidence (delivery6547ec4)

The focused diagnostic failed in synthetic session1 with all printed admission/identity/alignment checks passing and no debugger. elapsed_from_commit_ms=44.706700; window start=44.692600ms after commit; deadline=30ms. One window was allocated, but installed=0, skipped_busy=0, failed=0, hits=0, drops=0. No primary watch was installed and no lease remained. This is a real deadline refusal in the owned test, before the test writer ran. The stop reason is the subsequent generic cleanup reason, not proof of identity loss. LastError18 is unattributed and consistent with enumeration completion; it is not a proven installation error.

The production begin() sequence enumerates system threads before stamping window.start and before its deadline-guarded installation loop. The44.69ms elapsed therefore occurred before that stamp; no installation attempt was made. Individual CreateToolhelp32Snapshot / Thread32First / Thread32Next costs were not separately measured, so the exact slow OS call is UNKNOWN. winedevice messages alone do not identify the cause. No game code was executed and no user raw terminal dump is committed.

## Test-only correction

The legacy positive synthetic tests now acquire a real OS thread snapshot before stamping the synthetic source-commit QPC. A translation-unit-only API adapter hands that real handle to the unchanged production enumeration/filter/CloseHandle path. It does not fabricate thread entries, suppress filtering, replace QPC, alter thread APIs/VEH/MinHook, or extend the30ms deadline. Actual thread debug-register installation, store/exception handling and restoration still occur against the real deadline. The prepared handle is atomic for the asynchronous coordinator and must be consumed once. All fixture threads already exist before the snapshot. Current prearmed and multithread cases continue calling the real snapshot API through the adapter fallback; no prepared snapshot is supplied there. No adapter is compiled into the production DLL.

Two required cases distinguish setup from the deadline: --slow-setup delays each real snapshot preparation by50ms before the commit and must still complete the existing positive hardware/capture/cleanup assertions. --slow-snapshot delays handing the real snapshot to the legacy engine by50ms after the commit; it must reject the arm with zero installed watches/hits/leases, unchanged owned memory and original debug registers. Failure still terminates the suite. These replace the invalid assumption that cold host enumeration always fits in30ms, while keeping an explicit actual-time rejection test for delayed setup. Other scheduler or installation failures remain fatal and emit existing diagnostics.

## Scope and acceptance

Production source, headers, game admission, deadlines, write policy, version and gameplay state remain unchanged. The observed failure cause is bounded test setup latency before installation. Success on the cloud does not prove the user's corrected focused run, full migration, single WallSprint UI entry, SF visibility or actual runtime log. Keep the real-PC gate open and EDI research paused.
