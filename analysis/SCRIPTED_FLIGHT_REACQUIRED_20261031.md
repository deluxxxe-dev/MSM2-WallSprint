# Scripted Flight reacquired — 2026-10-31 JST
Owner supplied Scripted Flight2598 1.2 2026-10-16T23-35Z VTCZvsyOI.zip.
Chat actually opened nested ZIPs: ScriptedFlight_WSU_Compatible.modular -> scripts/ScriptedFlight.script -> ScriptedFlight.dll.
Archive SHA c1f43885f6138143ad54cf0d6d3be86b1e51b0abc27cc51155bb9de6e93794f7.
DLL SHA7330c68fa87da58a50a6ee4c477d17339e5deb28a82cf77f7b16c3c2092b7662;162816bytes. Exact prior reference accepted. No binary executed/installed/vendored.
Chat fetched current reference_motion_evidence.py, ground_support_code_refs.py, discover_ground_support.py and executed the first helper locally with Capstone5.0.6 against exact DLL. CODE_REFERENCE_CANDIDATES,2labels,2refs,1runtime function,62exported bytes. No further owner scanner run necessary.
DLL-relative runtime function79792..87466 (0x137B0 start), hash757641f6d104aab74f4db544942048d677e841889ff9594b785dfca5a940463d.
GravityOrientationOwner.call->SetGravityVector label RVA122005; ref85104.
UpdateMotion.call->RequestSimpleMotion label RVA122550; ref85142.
Both label refs use RDX=label, RCX=stack buffer rbp+0x308, R8/R9=two candidate values, call DLL RVA0x16920.
Chat separately disassembled0x16920 onward with objdump: saves four arguments; constructs local string stream; inserts label and two values, changes stream flags, builds string/output. This call is a diagnostic formatting helper, NOT actual RequestSimpleMotion/SetGravityVector. Exact formatting/output semantics not exhaustively classified; no native ABI inferred.
These references had candidate_prefix_byte_cap, not complete CFG-entry boundary proof. Existing scanner alone cannot establish actual verifier/native target. Need trace preceding candidate-value production and real compare/decode checks in initializer, and later uses of validated results.
No game-native current binding, mover/contact semantics or gravity ownership proved. No arbitrary-up Sprint feasibility conclusion.
Binary available in owner current chat; separate Codex environment needs explicit ZIP attachment or real identified local copy. Do not pretend it can fetch Chat scratch paths, publish third-party binary into source, or send owner through more filename scanners.
