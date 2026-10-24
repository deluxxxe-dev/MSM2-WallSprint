# Overstrike archive discovery and WS duplicate prevention

## Evidence

The user completed the real Windows/WSL migration build/test/package/deploy pipeline (131 public PASS /5 optional private-input SKIP), then supplied two shared-Overstrike screenshots showing ordinary mods and24 checked `WallSprint by WallSprint project` entries, rows24..47. No SF entry is identifiable in that supplied list. Install/game/runtime-log acceptance has not been reported. Automated physical-script validation was insufficient; the UI gate remains blocked.

A normal independent checkout of the current repository contains44 tracked release ZIPs. Independent inspection finds22 `.script` payloads inside install ZIPs. The expected22 source-contained entries +1 returned build-cache install ZIP +1 root active script equals24, matching the screenshot. Individual GUI entry paths were not captured; the counts and upstream discovery mechanism explain the observed failure.

Upstream reference inspected: Tkachov/Overstrike commit `ab61de27a4ba76d92780178f600f73f15bcddd81`, [ArchiveDetector.cs](https://github.com/Tkachov/Overstrike/blob/ab61de27a4ba76d92780178f600f73f15bcddd81/Overstrike/Detectors/ArchiveDetector.cs) and [ModsDetection.cs](https://github.com/Tkachov/Overstrike/blob/ab61de27a4ba76d92780178f600f73f15bcddd81/Overstrike/Detectors/ModsDetection.cs). Discovery dispatches on extensions, recursively visiting ZIP/RAR/7z members and handing contained `.script` entries to script detection. A container's `.disabled` suffix excludes it from that dispatch. No upstream source/binary was copied into WS; regression fixtures/model are independently implemented and contain owned data only.

## Correction

- Keep one root versioned active script and zero visible ZIP/RAR/7z anywhere under WS. Checks cover nested/mixed-case extensions and run after upgrade, after the full build, and before launching Overstrike.
- `_source` remains a regular independent clone with all code/documentation and Git objects/history. Managed non-cone sparse patterns include all paths except archive extensions. Clean tracked ZIP working-tree files are excluded by Git, not deleted from history; fast-forward pulls preserve the policy. Dirty/untracked archive state and custom sparse configuration refuse without discarding work.
- Validate old generated install/source ZIP metadata/CRC/manifest/script hashes and disable only recognized WS build outputs directly in `_archive/_build`. Move them to collision-safe `.disabled` names under `_archive`, preserving bytes/mtime. Refuse unrelated archives or unexpected locations.
- Full build/test/release stays in external Linux staging. Returned archives are `.zip.disabled`; a direct release call anywhere in Mods Library refuses before producing ZIPs. Public normally named ZIP downloads remain outside the scanned workspace.
- Never alter SF, ordinary mods, the old D environment or shared Overstrike binaries/cache/profiles. Root helpers keep deriving/validating the exact game/workspace shape. Gameplay/version/EDI pause and native safety gates remain unchanged.

## Verification and acceptance

Existing8 workspace groups remain. Five additional groups exercise source archive exclusion while retaining Git bytes/history/code, clean ff-only updates, old full-checkout/build-cache upgrade, an independent recursive-ZIP discovery model reducing3 candidates to1, disabled container handling, unknown archives, custom sparse protection, direct-release refusal and new raw archive output refusing deployment. Protected sibling snapshots retain hashes/size/names/mtime. These are cloud fixtures, not GUI proof.

The user must update the preparation checkout and rerun its Migrate-WS.cmd, then confirm that the same shared Overstrike list contains only one WallSprint. SF/ordinary-mod visibility, Install mods / Run modded game and a new versioned runtime log remain required. The previous helper pipeline PASS is not whole-workspace acceptance.
