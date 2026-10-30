# Next task — correct scripts_proxy dependency validation blocking Update-WS

Branch: codex/v01c20-edi-value-provenance. Do not merge main.
Read AGENTS, required operational docs, PROJECT_STATE, CODEX_RESULT, docs/mod-tools/{README,OVERSTRIKE,PLAY_MODDED}.md and MANUAL.md; relevant Wisdoms shared environment rules. Scope is the existing shared-environment check conflict only.

## Owner evidence — 2026-10-30/31 JST
- Overstrike1.8.1.0 with script support disabled produced no new WS log/Mod Settings.
- After enabling script support, Install mods failed in MetaInstallerBase.SetupScripts File.Copy:
  Could not find <game>/Overstrike/scripts_proxy.dll.
- ChatGPT directed COPY of existing <game>/scripts_proxy.dll into <game>/Overstrike/scripts_proxy.dll, leaving root copy intact.
- Subsequent owner game/log capture loaded WS v0.1c38 successfully; no install-success log was separately supplied.
- Now Update-WS refuses: "Shared Overstrike must not contain scripts_proxy.dll; fix shared installation manually".
This is direct evidence that current validator prohibition conflicts with the observed installer dependency. Do NOT tell owner to delete/move the DLL again.

## Decision / implementation
Distinguish scripts_proxy.dll (installer source dependency) from winmm.dll (game loader that must not be beside Overstrike.exe).
Authenticate the exact source path contract from applicable Overstrike1.8.1.0 SetupScripts/source when available; report source/version limits. Then correct the named blanket scripts_proxy prohibition in source/tools/workspace.py or exact defining checker. Search only the specific error string and immediate shared checks/tests.
Allow a regular, non-reparse scripts_proxy.dll beside the shared Overstrike executable. Preserve regular-file/symlink protections. Retain rejection/quarantine policy for misplaced Overstrike/winmm.dll; do not generalize all DLLs.
Inspect environment checker/launcher for the SAME conflation and correct only reproduced equivalents needed for consistent existing updater/start/play operation. Preserve all unrelated safety checks.
No automatic DLL downloads/copies/deletes or profile/settings mutation. Preserve root scripts_proxy.dll and winmm.dll. If identity/integrity checks are already required, preserve them and document distinct installer source vs game loader, not a new unverified global equality requirement.
Update affected local manual and combined manual together; AGENTS/PROJECT_STATE stale placement statements may be corrected only for this fact. Record durable shared finding in WS; Wisdoms sync may remain pending rather than expanding this bounded patch.

## Tests / completion
Regression: valid installer-source scripts_proxy beside Overstrike accepted; misplaced winmm still refused; missing/wrong-type required inputs and reparse paths still refuse; root loader checks unchanged; SF/unrelated MODs untouched.
Run required existing workspace/environment tests, relevant PowerShell checks and diff checks. Runtime DLL remains v0.1c38r1/package0.1.57 byte-unchanged. No need to rerun unrelated native research chain for checker-only changes unless required by repository checks.
Commit/push, verify remote and update CODEX_RESULT/PROJECT_STATE with exact commands/results.
Check updater order: if this old checker stops before fetch, prepare the exact minimal one-time fetch/fast-forward or existing supported bootstrap to refresh checker without removing DLL or bypassing validation. Preserve dirty worktree and branch divergence; no reset/clean/main merge. Record exact owner bootstrap command in CODEX_RESULT; ChatGPT presents it.
No claim of real-PC Update-WS success until owner performs it.

## Forbidden
Gameplay/reversal/runtime/version changes; new movement/hook/watch/VEH/VCH/producer/c73 work; SF or ordinary MOD mutation; loader deletion; test/gate bypass; reinstall loop.
All existing C52 and native gates unchanged.

Reply:
STATUS: success / partial / blocked
CHANGED:
- paths
RESULT:
- checks, publication and whether one-time checker refresh is needed
NEED:
- exact bootstrap command or unresolved input only
