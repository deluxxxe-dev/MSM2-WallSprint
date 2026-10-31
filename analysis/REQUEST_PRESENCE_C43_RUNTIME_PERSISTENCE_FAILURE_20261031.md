# RequestSimpleMotion c43 runtime persistence failure — 2026-10-31

Status: **VERIFIED-RUNTIME admission success / persistence design failure / c44 correction published / c44 runtime pending**.

## Supplied c43 runtime evidence

The supplied `MSM2-WS-v0.1c43-20261031-221920.log` is from the pinned target build:

- runtime `v0.1c43`, package `0.1.68`;
- `Spider-Man2.exe` version `2.810.0.0`;
- on-disk SHA256 `c9b912eff96facb746a705ac8b0b83c29faa7bbe53d984a0c5eb45efd0b738c9`;
- target build gate accepted;
- `REQUEST_PRESENCE_DIAGNOSTIC=1`.

RequestSimpleMotion admission fully succeeded in the actual game:

- whole-site SHA256 matched;
- seven-byte prefix matched;
- stolen span accepted;
- exact `.pdata` begin/end/UnwindData matched (`0x11A1BD0..0x11A1CA2`, UnwindData `0x94697E0`);
- every split observer/restore unwind entry passed;
- MinHook trampoline validation passed;
- final `REQUEST_PRESENCE_ADMISSION admitted=1`;
- automatic capture armed successfully.

This is the first actual-game proof that the current exact Request hook site/admission path can be installed with the preserving observer and trampoline under the pinned game build.

## What failed

The c43 capture implementation retained records in one large in-memory phase and emitted `REQUEST_PRESENCE_RECORD` lines only during phase finalization. The phase timeout was 300000 ms / 5 minutes.

The supplied user session performed the requested gameplay sequence and then exited normally before that timeout. The log contains:

- no `REQUEST_PRESENCE_RECORD` lines;
- no `REQUEST_PRESENCE_PHASE_SUMMARY`;
- no `REQUEST_PRESENCE_SESSION_SUMMARY`;
- no `WallSprint exited reason=TelemetryStopped` marker.

Therefore the runtime did not reach the module's expected graceful finalization path before process/log termination. The absence of Request records is **not** evidence that RequestSimpleMotion had zero calls; the records may have existed only in process memory and were never serialized.

This is a persistence/lifecycle design failure, not a hook admission failure.

## Independent WallRun evidence in the same run

The verified WallRun lifecycle hooks recorded one complete main WallRun session:

- session 1;
- Init=1 / Entry=1 / Update=1177 / Deinit=1;
- duration approximately 15.401750 seconds;
- start UTC approximately `13:21:43.367Z`;
- end UTC approximately `13:21:58.769Z`.

This proves the requested gameplay reached a normal WallRun lifecycle during the c43 capture period. It does not establish Request argument meaning because no Request records were persisted.

## c44 correction

`v0.1c44 / package 0.1.69` keeps the c43-admitted exact hook identity and changes capture persistence only:

- capture still starts automatically immediately after exact Request admission;
- F11/F12 remain disabled/not required;
- no gameplay writes are enabled;
- hot callback still performs no file I/O, locks, allocations, pointer dereferences, argument edits or request suppression;
- callback buffer is reduced to 16 opaque records;
- worker-side drain interval is 500 ms;
- worker closes the capture gate, waits for active callbacks to quiesce, then logs the completed batch and resets it;
- overflow/contention are logged per batch;
- retained session cap is 65536 records;
- periodic batches are already on disk before shutdown, so graceful shutdown is no longer required to preserve prior batches;
- shutdown requests are synchronized with an Interlocked close flag so a periodic worker drain cannot re-arm capture after `script_disable`/F8 stop begins;
- normal stop still attempts a final partial-batch drain, but this is only best-effort cleanup rather than the sole persistence path.

The maximum loss on an abrupt termination is limited to the currently open <=500 ms batch; earlier periodic batches have already been serialized.

## Runtime acceptance still pending

The owner has not yet run the normal `Update-WS.cmd` full build/test/package/deploy pipeline for c44, and c44 has not yet run in the actual game.

For c44 acceptance, require:

1. full updater pipeline PASS and deployed `v0.1c44 / 0.1.69`;
2. actual-game `REQUEST_PRESENCE_ADMISSION admitted=1`;
3. `REQUEST_PRESENCE_CAPTURE_START ... periodic_worker_drain_ms=500 batch_record_cap=16`;
4. at least one `REQUEST_PRESENCE_BATCH_SUMMARY` during gameplay;
5. actual `REQUEST_PRESENCE_RECORD` lines before game exit;
6. correlate those records to the supplied video only after 1-5 pass.
