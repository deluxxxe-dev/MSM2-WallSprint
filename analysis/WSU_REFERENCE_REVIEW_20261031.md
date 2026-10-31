# WSU supplied reference review — 2026-10-31

Scope: offline inspection of owner-supplied Nexus 2123 WSU 2.1 archive and Nexus 771 speed/jump archives. No DLL executed, installed, copied into production, or new runtime permission granted. Current CODEX_NEXT ON/OFF diagnostic task is unchanged; this note is parallel research, not an instruction to interrupt that task.

## Identities / reproducibility
Nested archive: Web-Swinging Unlimited 2.0.modular -> scripts/sc_core.script (ZIP) -> SwingCore.dll and components/wallslide.dll.
SwingCore.dll SHA256 c8657d0fdbec3afb49fe6873ba736696cc9035eb9dd8aa1737867fc272b5a1e9
wallslide.dll SHA256 74846e789ee0480de67da20a3d760c63394a73d06448a4848a478994c10907e1
Native x64 PE DLLs, not managed .NET assemblies. Inspection used Python ZIP/struct and GNU objdump Intel disassembly. DLL RVAs below refer ONLY to reference DLLs, never the game image.

## VERIFIED-STATIC: steering asset
stages/wallslide_down.stage contains the 544-byte configs/hero/hero_wallrunsteeringsetup_standard.config and wall_slide_dn_spiderman.animclip.
Compare authenticated vanilla decoded values in VANILLA_WALLRUN_STEERING_CONFIG_20261029.md:
- TurnMaxDown 12 -> 88 (byte at file offset 0x213).
- TurnSpeedFast 100 -> 200 (0x218).
- TurnSpeedSlow 75 -> 45 (0x219).
Other decoded values match the documented vanilla values, including MoveSpeedFast16, damping -40/-20, gain -3.5/-2.25 and scale~0.72.
These are reference tuning values, not proven units or a remedy for WS. Earlier Fast10/Slow5 owner experiments showed no visible response; do not restart serial config probes on this evidence alone.

## VERIFIED-STATIC: wallslide is also a runtime controller
wallslide.dll contains ComponentRegistry, ActorComponentSmall/Large, HeroCache, HeroSystem, HeroStateWallRunMoveLocal, WallRunEntry/Update/Init and SC_FindTargetSlot/SC_CheckTarget references.
Worker begins RVA0x1BD0; loop Sleep(8) at0x1CE0. It queries SC_Setting with argument4 (setting-index semantics not independently established here), resolves a current state, and uses scalar history/latches.
RVA0x2078 reads an intermediate object pointer at+0xF8 and then a scalar at+0x114 via guarded helpers. This scalar must NOT be called input direction or position without further origin proof.
At0x20B8 onward it subtracts prior scalar, accumulates/clamps history, uses entry/exit thresholds, logs Slide ON/OFF.
RVA0x2235 stores constant0xF9726543 to state-relative+0x1E4 after VirtualProtect. RVA0x2279/0x22BC writes the same evolving scalar to+0x258/+0x25C; increment~0.0784 each worker pass at0x220A. Original fields are saved and conditional restoration occurs at0x1070 and worker cleanup.
Thus this is NOT merely an animation asset or steering-config change. Field meanings and game-version compatibility remain UNKNOWN. Do not port offsets/writes.
Relevant floating constants RVA0x1B7E4 onward:0.0784,0.1,0.3,0.5,5,15,-0.055,-0.2,-0.5. These are reference constants, not WS tuning recommendations.

## VERIFIED-STATIC: SwingCore run-vector selection
A real code branch, not strings alone, exists in SwingCore.dll function RVA0x36770.
It initially loads a 3-vector from its incoming CONTEXT argument+0x98/+0x9C/+0xA0, checks finite/nontrivial length, and chooses label velocityPure (RVA0x183258) at0x3686F.
If invalid/too small it instead loads context+0x8C/+0x90/+0x94 at0x36878 and label inputDirection (0x183268). Subsequent code validates/uses run and wall-normal vectors and logs wall orientation resolved/unavailable.
This establishes a velocity-first / inputDirection-fallback selection inside this reference context. It does NOT establish where the context is populated, whether inputDirection is raw/mapped/current intent, or any game's writable ABI. It also does NOT mean WSU reacts to reversed input before velocity reverses: valid velocity has priority here.
The prior chat's suggestion that these strings alone supply earlier-input reversal is therefore only a research lead, not an established solution.

## Useful next investigation after current diagnostic task
1. Trace callers/context population for RVA0x36770, especially context+0x8C vector origin and native query validation; identify independent read-only intent evidence if it truly exists.
2. Keep reference body facing/animation variables separate from movement direction. WSU includes webrun_drivers.stage replacing Spiderman_Traversal.animset and separate web-wallrun/wallslide clips; no proof of arbitrary downward wall sprint.
3. Use current ON/OFF result to prioritize orientation feedback vs native descending limits; do not infer causality from WSU names.
4. No source/binary/asset copying; independently verify current game build, object lifetime/identity/field semantics before even proposing production reads or writes.

Nexus771 archives contain only hero_wallrunspeedsetup_standard.config and hero_wallrunjumpsetup_standard.config plus metadata; lower priority for facing/intent research.
Presets are settings only. No additional owner download or game test is needed for this offline review.
