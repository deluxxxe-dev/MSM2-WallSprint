# v0.1b6 result — diagnostic not established

The repository's docs/PROJECT_STATE.md reports `orientation_trace_config enabled=0 admitted=0`, installed/armed/captured=0, and an attempted arm approximately 68–88 ms after the relevant source commit. Actor Up briefly became surface-relative, then World Up about 10–20 ms later. These are supplied project findings; the raw b6 user log/video are not present in this checkout and were not independently replayed here.

VERIFIED project baseline: the existing caller-local source override reaches Actor Up (b5). UNKNOWN: the intervening reset instruction/caller/source. The b6 inactive/late watch neither identifies nor disproves a reset writer. Do not redesign the successful source override or infer game success from Wine tests.
