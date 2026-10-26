# c40 gate — bounded retirement not established; fail-closed fallback

The positive c40 implementation acceptance remains NOT MET. Existing production
watch code is unchanged and retains the c39 limitation. Owned Wine tests extend
the original real held-return counterexample to timeout/kill/repeated readbacks;
all can restore current DRs and still resurrect the retired settings afterward.
Normal acknowledged return restores correctly. VCH registration succeeds but
callback is not reached in2 probes; VCH return-hold coverage is UNAVAILABLE.
No ordering guarantee is invented; no c39 slot mode/retirement certificate or
new game release. Runtime v0.1c38/package0.1.56 retained. Fourteen regression
PASS records establish these observations/recovery, not a fixed retirement
contract. See V01C40_RETIREMENT_REVIEW.md / V01C40_RETIREMENT_STATUS.json and
CODEX_RESULT.md for commands, expected/actual outcomes and unperformed tests.
