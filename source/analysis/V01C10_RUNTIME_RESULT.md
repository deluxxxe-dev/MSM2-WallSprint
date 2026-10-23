# v0.1c10 real-runtime A/E regression

Private user log/video/screenshots were analyzed; raw files are not committed.

Build:
```text
WallSprint v0.1c10 / package 0.1.28
Spider-Man2.exe 2.810.0.0
SHA256 c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9
target_build_gate=ACCEPTED
```

Session 1, diagonal:
```text
N398≈(-0.70710677,0,+0.70710677)
wallrun_duration_ms=4172
source_entry_elapsed=0.719000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
retention_pass=1
max_held_ms=3187
max_held_updates=204
last_actor_up_dot_surface=1
hard_kill_calls=0
secondary seen/admitted/overridden/rejected=102/102/102/0
Position writes=0
max_abs_standoff_error=0.0268774629 m
```

Session 2, cardinal:
```text
wallrun_duration_ms=3421
source_entry_elapsed=0.718000
source_exit_reason=verified_native_Deinit
source_active_until_deinit=1
retention_pass=1
max_held_ms=2453
max_held_updates=140
last_actor_up_dot_surface=1
hard_kill_calls=0
secondary seen/admitted/overridden/rejected=161/161/161/0
Position writes=0
max_abs_standoff_error=1.52587891e-05 m
```

Across this human log:
```text
post-transition actor_up=(0,1,0): 0 observed
secondary_source_publication_changed: 0 observed
PATH_B_SECONDARY_PUBLICATION_REFRESH_EVENT: 0 observed
secondary_publication_refresh_seen=0
```

Interpretation:
- cardinal and diagonal orientation stability is accepted through verified Deinit;
- the c9 WorldUp leak did not recur;
- no natural real-game c10 refresh race occurred in this test, so do not claim real refresh-path execution;
- deterministic c10 fixtures remain the direct evidence for the bounded refresh branch.

Deferred visual notes from user screenshots:
- wall-running body/root looks leaned toward the apparent ground side vs normal ground running;
- feet/body look too far from the wall and visually float.
These are later visual-alignment/contact tasks, not current lifecycle blockers.

Next: movement-path / Hero Position writer attribution. Keep movement and Position read-only in c11 until safe upstream ownership is proven.
