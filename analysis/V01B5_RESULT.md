# v0.1b5 actual-game result

Source: user-uploaded WallSprint.log, 454808 bytes, SHA256
`b4cf534572e01d6369db6e9f950137d42061c0016ceb1195441b7f966786f4ab`.
Target: Spider-Man2.exe 2.810.0.0, SHA256
`c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`.

## Confirmed

| Session | Mapping | WallRun duration | Source writes | Source interval | Exit |
|---|---|---:|---:|---:|---|
| 1 | Control A | 1.776652 s | 20 | .265 s | verified_deinit |
| 2 | Face E | 2.314481 s | 15 | .265 s | verified_deinit |
| 3 | Additional E-like normal; video label unconfirmed | 1.955925 s | 16 | .265 s | verified_deinit |

All source overrides exited with `A_or_B_source_applied_but_not_retained`; held_updates=0, stability_observed=0. This is a retention failure, not evidence that the source never reached the Actor.

Session1 at 17:28:02.065Z: applied Up=(-2.98023224e-08,.556875587,.830595851), actual Actor Up=(-2.98023259e-08,.556875646,.830595970). World Up was next sampled at 17:28:02.076Z.

Session2 at 17:28:40.262Z: applied and actual Actor Up=(-.447294623,.774503112,.447294593). World Up was next sampled at 17:28:40.273Z.

Thus the verified caller-local source → original common writer → real Actor Transform pipeline works in both A and E. Keep its gates, interpolation, position preservation and fail-closed policy. Session3 did not sample a sufficiently tilted actual Up; its16 successful source writes alone cannot establish actual retention.

## Precision and remaining unknowns

The 11ms differences above are worker log-delivery intervals. Native GetTickCount64 differences are15ms/16ms. Neither identifies the exact reset store nor its exact latency. A later native write is the leading hypothesis; **its function/caller/thread/source remain unverified**. The same common writer with another caller, or with an unmodified invocation from the same caller, is still possible. A distinct writer must not be assumed.

The log does not establish a clean full telemetry shutdown. No claim of crash-free completion or visual stability is made. v0.1b6 uses QPC and actual data-write events to resolve the missing evidence. Sanitized observations are in V01B5_RESET_SUMMARY.json; no game function bytes are included here.
