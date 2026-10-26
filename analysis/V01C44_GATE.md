# c44 gate — conservative comparison, no native restoration certificate

C43 native remains RAW exact FAIL, localized to Dr6/Dr7 with lease cleanup and
Dr0-Dr3 exact. CPU vendor/stability were absent. C44 reports actual CPUID vendor,
separate RAW_EXACT/ARCHITECTURAL_CANONICAL and all3 readbacks before classification.
Only documented fixed groups may differ; meaningful/global/potential BLD/RTM
fields stay exact; current11/16=0 states additionally refuse the narrow candidate
boundary. Current fixed values must be valid and unknown bits/vendor
refuse. The c43 pair still fails both conservative vendor profiles; no all-one
DR6 blanket mask. Pinned primary manual editions and current-delivery limitations
are explicit in V01C44_DR_CANONICALIZATION.md. Wine raw regression PASS is not
architectural/native equivalence. No production restoration/watch change or game
release; runtime v0.1c38/package0.1.56. Publish a tested c44 standalone only, then
request one native JSON command. Native result and production integration remain
unproven; see V01C44_STATUS.json and CODEX_RESULT.md.
