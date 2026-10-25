> Accepted c27 actual: short JA0x2CF7FE7 disp42 -> taken0x2CF8013/fall0x2CF7FE9, both forward edges admitted for fan-out; first fallthrough MOV memory effect refuses,0 returns/identity. See V01C27_RUNTIME_RESULT.md and successor V01C28_MOV_CLASSIFICATION.md. Earlier pending-edge statements below are preparation history.

# C27 bounded JA gate

Accepted c26: CMP r8d,0x3FFF0x2CF7FE0 flags-only/tag0; next JA0x2CF7FE7,
condition CF0&&ZF0, numerical outcome unknown, identity/effect0.
Prepared c27: exact short/near edges, current-call tag-disjoint proof only;
1 split/2 paths/128 bytes/32 unique instructions/64 visits. Both paths required
normal RET with no prohibited effect and final code/runtime rechecks before
identity/hash; parent call effects repeat per-state tags and merge both outcomes.

Actual edges/next barrier remain PENDING/Outcome C, not supplied by owned fixtures.
Seven new owned groups (28 identity total), existing21 call/read and full pipeline
preserved. See V01C27_JA_FANOUT.md / CODEX_RESULT.md / V01C27_RELEASE.json.
No ownership/writability/XYZ-W or game-write gate opened; producer0x351EB00 deferred.

Human minimum: close game/Overstrike; Update-WS.cmd; Install mods -> Run modded
game; A ~3s -> fully end WallRun -> move/fast-travel -> E ~3s, each below5s.
Newest MSM2-WS/logs/MSM2-WS-v0.1c27-YYYYMMDD-HHMMSS.log; branch edges/CFG/next
barrier plus existing safety, no body or memory dump. Do not repeat by habit
once accepted current-code evidence settles the bounded question.
