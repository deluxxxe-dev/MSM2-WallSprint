# Reference MOD WallRun review — 2026-10-29

## Purpose and scope

This is a static review of owner-supplied third-party MSM2 MOD archives while the c72 OFFLINE owner replay remains the current bounded task.

The reference MODs are research hints only. Nothing in this review authorizes copying third-party code, binaries, assets, offsets, ABI assumptions, or gameplay writes into WallSprint. Any candidate technique must be independently re-established against the current authenticated game build before use.

Evidence labels used here:

- **VERIFIED-STATIC (reference artifact)**: directly decoded or observed in the supplied archive/binary.
- **HYPOTHESIS**: plausible implication for WallSprint that is not yet independently proven on the current game build.
- **UNKNOWN**: not established by the supplied artifacts.

No game/runtime test was performed for this review.

## Supplied archive identities

```text
wallrun jump buff-771-1-2-1742407616.zip
SHA256 5e0ba87b0b233bc078876eebb8279d23dcd555d23e974b7a85b0c8158381cc1f

wallrun speed-771-1-1741985277.zip
SHA256 c583b4a4f209fb91ed5c6403b770921c4863b0efff31c81fca7a1042d140ee65

WSU Default Presets 2123 3 2026-10-17T09-09Z 9AZUqYX61.zip
SHA256 87832ad38a854ea1f6ae92e76f9ac5269fb35bb9cd9d5c3d07caea8a32e4e2f7

G.W.E.N. ... X1SMm8yVk.zip
SHA256 242e5aa416cb93a3a627c51831b423d757bc382ea5b362a418e9cb002894e8fe

G.W.E.N. ... X1SMm8yVk (1).zip
SHA256 242e5aa416cb93a3a627c51831b423d757bc382ea5b362a418e9cb002894e8fe
(the two supplied GWEN archives are byte-identical)

Scripted Flight 2598 1.2 ... VTCZvsyOI.zip
SHA256 c1f43885f6138143ad54cf0d6d3be86b1e51b0abc27cc51155bb9de6e93794f7

Spiderman Unnerfed 2-107-1-5-1742311780.rar
SHA256 2d6adc0c677f61380ce37c202287b8081a3fc1c0c4e0ff3a6c73f6db7e1bd3ce
```

## Config decoding basis

The supplied `.config` assets were decoded using the public MSM2/I30 DAT1 format implemented by `Pcniado/DAT1-GUI`, specifically the public definitions for:

- `OverstrikeShared/STG/STG.cs`
- `OverstrikeShared/STG/Files/Config.cs`
- `DAT1/Files/Config.cs`
- `DAT1/DAT1.cs`
- `DAT1/Sections/Generic/SerializedSection_I30.cs`

Relevant public format facts include:

```text
Config_I30 type magic = 0x35F7AFA5
Config Type section tag = 0x4A128222
Config Built section tag = 0xE501186F
```

This establishes the decoding method only. Field semantics beyond their names remain subject to runtime/current-build verification.

## Finding 1 — WallRun speed and jump behavior are represented by config assets

**VERIFIED-STATIC (reference artifact).**

The `wallrun speed` archive contains no native DLL. Its payload is:

```text
configs/hero/hero_wallrunspeedsetup_standard.config
```

Decoded type:

```text
WallRunSpeedSetupConfig
```

SHA256 of supplied config:

```text
f775b159821af0b63cb04b35f749884dae8fbe2a13426ee5c2b9d7a11ddd22e1
```

The decoded `SpeedSetupList` contains named setup classes including:

```text
kVerticalDefault
kVerticalIndoor
kHorizontalGround
kHorizontalIndoor
```

and many explicit speed/acceleration/deceleration fields, including:

```text
InitSpeedFloor
SettleAccel
SettleActiveSpeedMax / Min
SettleDecelDelay / Gap / Max / Min / RampTime
SettleSpeedMax / Min
TimingInSpeedMax / Min
TotalSpeedMax / Min
```

Representative values from the supplied modded `kHorizontalGround / SpeedSetupStickMax` entry are:

```text
InitSpeedFloor = 65
SettleAccel = 12
SettleActiveSpeedMax = 97
SettleActiveSpeedMin = 38
SettleSpeedMax = 52
SettleSpeedMin = 30
TotalSpeedMax = 120
TotalSpeedMin = 65
```

These are the supplied MOD values. Their difference from vanilla is **UNKNOWN** until the current-build vanilla asset is extracted and compared.

The `wallrun jump buff` archive likewise contains a config asset rather than a native DLL:

```text
configs/hero/hero_wallrunjumpsetup_standard.config
```

Decoded type:

```text
WallRunJumpSetupConfig
```

SHA256:

```text
4070e5d7596b3e6f4e1d53905ae7781411bbc10a1177c1032e212072c564bac5
```

Decoded groups include:

```text
JumpDirectionData
JumpOffData_00
JumpOffData_45
JumpOffData_90
ReleaseData
```

with explicit fields such as:

```text
GenBoostCap
GenBoostMax / Min
SpeedMax / Min
TimeToPeakMax / Min
YBoostMax / Min
```

Conclusion: at least a meaningful portion of WallRun speed/jump behavior is data-driven through config assets in the supplied MOD ecosystem. This does **not** prove that every movement/orientation component is config-driven.

## Finding 2 — WallRun steering is an explicit independent config family

**VERIFIED-STATIC (reference artifact). This is the highest-value finding in this review.**

`Spider-Man Unnerfed II 1.5` contains a traversal config whose decoded `WallRunConfig` explicitly references four separate WallRun setup resources:

```text
JumpSetupDefault
  -> configs/hero/hero_WallRunJumpSetup_Standard.config

SpeedSetupConfig
  -> configs/hero/hero_WallRunSpeedSetup_Standard.config

SteeringSetupIndoor
  -> configs/hero/hero_WallRunSteeringSetup_Indoor.config

SteeringSetupStandard
  -> configs/hero/hero_WallRunSteeringSetup_Standard.config
```

Traversal config SHA256:

```text
244c3be77700df599c2c0584b60a38223f22298d54ed9d776534857afe2ae6d3
```

The same decoded WallRun section also exposes named fields/groups associated with engage direction, horizontal/vertical speed families, wall fall, and corner-jump behavior, including examples:

```text
GroundEngage.WallRunAngleTime
StandardEngage.DirectionData.HorzMomentumAngleFast
StandardEngage.DirectionData.HorzMomentumAngleSlow
StandardEngage.DirectionData.HorzMomentumSpeedFast
StandardEngage.DirectionData.HorzMomentumSpeedSlow
WallFallGravityMax / Min
WallFallInSpeedMax
WallFallStartSpeedMin
CornerJumpParams
```

The supplied artifact therefore establishes that WallRun steering has dedicated `Standard` and `Indoor` config resources, separate from WallRun speed and jump setup.

It does **not** establish what fields exist inside the current-build vanilla steering configs, nor whether those fields can express arbitrary surface-relative WallSprint motion.

### Derived asset IDs for current-build extraction

Using the public DAT1-GUI normalized CRC64 asset hashing algorithm, the resource paths above derive to:

```text
configs/hero/hero_WallRunSteeringSetup_Standard.config
  asset id 948FA063808F7E18

configs/hero/hero_WallRunSteeringSetup_Indoor.config
  asset id 83DF4507531061B1

configs/hero/hero_WallRunSpeedSetup_Standard.config
  asset id 80D25C57AC09BE31

configs/hero/hero_WallRunJumpSetup_Standard.config
  asset id B78A8FA3857CFB17

configs/hero/hero_WallRunConfig.config
  asset id 86DF98B17F4D766D
```

These IDs are derived from the reference paths and public hash algorithm. They are intended only to locate and inspect the corresponding current-build vanilla assets.

## Finding 3 — Scripted Flight demonstrates a higher-level motion/gravity/orientation design pattern

**VERIFIED-STATIC (reference binary strings/import structure), with semantic interpretation limited to HYPOTHESIS where noted.**

Supplied `ScriptedFlight.dll` SHA256:

```text
7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662
```

Package metadata identifies it as Scripted Flight by ButtersTM, version 0.8.11, with ModSettings dependency. The DLL links MinHook per its included third-party notice.

The DLL contains diagnostic/dependency labels naming a movement stack that includes:

```text
UpdateFirst
UpdateInput
UpdateSteering
ApplyInputToPitch
UpdateMotion
UpdateAnimVars
StateDeinit
OnTransition
GravityOrientationOwner
SetGravityVector
RequestSimpleMotion
```

It also contains internally asserted relationships/labels such as:

```text
UpdateMotion.call->RequestSimpleMotion
GravityOrientationOwner.call->SetGravityVector
UpdateAnimVars.unitVelocity agrees
UpdateMotion.velocityStart agrees with OnTransition
UpdateMotion.pointerCache==StateDeinit.pointerCache
```

and field/dependency names including:

```text
unitVelocity
velocityFrame
velocityBlend
actorMatrix
gravityUp
inputX
inputY
pitchBlend
```

The binary contains explicit failure/status language about:

```text
arbitrary-up flight
mover orientation
mover gravity chain
travel facing
flight velocity
```

**HYPOTHESIS for WallSprint:** a viable high-level movement implementation may exist around a motion-request path plus mover gravity/up-vector and actor facing/orientation, rather than requiring WallSprint to first resolve the current exact-slot publisher chain completely.

This is **not** proof that the named functions, field offsets, calling conventions, or exact addresses are valid/safe on WallSprint's authenticated build. Any such route must be independently located and verified. Third-party offsets must not be copied as production truth.

The Scripted Flight package also carries separate traversal animation asset replacements, suggesting its motion control and visual animation presentation are distinct concerns.

## Finding 4 — GWEN shows a reusable fail-closed hook-acquisition pattern

**VERIFIED-STATIC (reference binary strings).**

The two supplied GWEN archives are identical. The nested `animation_studio.dll` SHA256 is:

```text
85d7a2e01f626613184a1e04540c2199873a81b2aea1e4e37006b745dd4cab5f
```

Reference-binary strings show a dynamic game-thread acquisition strategy with multiple tick candidates:

```text
TickB
TickC
TickD
TickE
```

and fail-closed diagnostics for:

```text
signature matched N times (need exactly 1)
falling back to RVA
no hookable candidate
no game-thread tick acquired
```

The binary also contains Hero/animation discovery labels such as:

```text
HeroSystem
animComponent+0xF8 (from StateInit)
[animComponent+0x70]+0x88
partial states
```

**Reusable lesson:** an established MSM2 script MOD can use signature-authenticated candidate rotation/fallback and refuse when hook identity is ambiguous. The specific signatures/RVAs/offsets remain third-party reference data and are not WallSprint evidence.

## Finding 5 — WSU preset archive exposes wall-traversal feature switches, but not implementation

**VERIFIED-STATIC (preset text only).**

The supplied WSU default preset archive contains INI presets with settings including examples:

```text
WebWallRun=1
WallSlingshots=1
WallChargeJump=1
WallChargeSpeedPercent=...
WallCrawlChargeDirection=...
WebGroundRun=...
```

The supplied archive does not contain the WSU implementation binary/source, so no hook, field, or engine-interface conclusion can be drawn from these settings alone.

## Implication for the current WallSprint strategy

Current c72 remains the bounded direct-CFG task and is not superseded by this review.

However, if c72 completes with no usable writer/publication candidate, this review establishes two materially different investigation routes that should be compared before automatically extending the same direct-CFG chain again:

1. **Current-build vanilla WallRun config route**
   - extract the five assets listed above from the authenticated current game build;
   - decode `WallRunSteeringSetup_Standard` and `Indoor` first;
   - compare the current-build vanilla speed/jump configs with the supplied MOD configs;
   - determine which steering fields actually exist and whether an asset-only minimal experiment can affect wall-relative direction safely.

2. **Higher-level runtime motion route**
   - use the Scripted Flight labels only as search hints;
   - independently identify/authenticate the current-build motion-request, gravity/up-vector, mover-orientation and actor-facing paths;
   - prefer the smallest runtime proof showing one controllable quantity at a time;
   - do not import reference offsets or ABI declarations.

### Proposed decision rule after c72

If c72 returns:

```text
next direct-CFG unknown boundary count = 0
candidate count = 0
```

then treat the direct-CFG route as closed for this investigation and prioritize vanilla steering-config extraction before opening another boundary-continuation version.

If c72 still leaves only a small direct-CFG remainder, compare the cost/value of one final closure pass against the config route rather than continuing automatically.

## What remains unknown

- Current-build vanilla contents of `hero_WallRunSteeringSetup_Standard.config` and `Indoor`.
- Which steering-config fields, if any, can produce the intended surface-relative WallSprint direction.
- Whether vanilla runtime selects/overrides those configs differently across WallRunSide/WallRunUp/Mixed states.
- Exact current-build identities/ABIs of the Scripted Flight-labelled native functions.
- Whether a high-level `RequestSimpleMotion`/gravity/orientation route can coexist safely with vanilla traversal state and restoration requirements.
- Whether the current exact-slot publisher chain is required at all for an eventual WallSprint implementation.

## Safety / ownership

This review made no gameplay/native changes and does not alter current c72 authorization. No SocialFeed or other MOD repository was modified. Third-party binary/assets remain external reference material only and are not vendored into WallSprint.
