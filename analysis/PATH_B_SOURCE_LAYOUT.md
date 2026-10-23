# Path B source layout: candidates only

`[RDX,RDX+0x80)` is a bounded root snapshot, not an assumed matrix. Raw RCX/RDX/R8/R9/RSP, TEB bounds, signed RDX-RSP, pointer-in-stack, full128-in-stack and expected RSP+0x28 relation are independent fields. Readability is checked page by page and with ReadProcessMemory; guard/noaccess/unmapped/overflow ranges refuse. Pointer-looking qwords are listed offline and never followed.

Capture: verified current Hero, exact caller, accepted/owned hooks, fresh native lifecycle/owner/generation/thread and worker identity; elapsed0.60..1.50, max8 sessions,128 entries/session,256 queued. Missing capture is not proof of absence. Log contention/budget drops explicitly. Unknown layout never aborts Path A. B never calls the stack write helper.

Offline review lists every finite float at4-byte alignment, every finite Vec2/3/4 observation, aligned pointer-looking qwords, Vec3 lengths and normalized dot scores against WorldUp, N398, valid current Path A interpolated Up, Hero before, and anchored watched Hero after. `--peer` compares identical offsets across A/E, retaining candidate labels. No matching vector is given a semantic field name by resemblance.

Actual source offsets/semantics: UNKNOWN. The b8 supplied excerpt proves neither Right/Up/Forward/Position at0/10/20/30 nor a packed/quaternion representation. A/E b9 captures and static confirmation of field use are pending. PATH_B_FIELDS_VERIFIED=0, PATH_B_WRITE_PERMISSION=0.
