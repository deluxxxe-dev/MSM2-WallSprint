# Source provenance

- Public game API headers and Transform accessor: hbgda/SM2ScriptTemplate, commit 999d0c7437720be5378049455209e69447f4a930. The files retained under vendor/SM2ScriptTemplate are unmodified; their hashes are in analysis/sdk_sources.json. No prebuilt DLL or .lib is reused. No separate LICENSE file was present in that upstream snapshot.
- Tkachov/MSMR-Scripting, commit 994ad7042452f892bef3a338731f9e7da69c855e, documents that its SDK is based on the above template and is GPLv3. Its license text is retained under vendor/MSMR-Scripting/LICENSE.txt. Its MSMR-specific RVA and runtime DLL are not used.
- Loader ABI/package behavior was checked against Tkachov/Overstrike commit 9f906ca09ea5ce4b84528410171c235bc4b0bfe6. No loader binary/code is included in this package.
- Reference ActorRelativeCamera/WalkOnWalls/SwingCore binaries/assets are not included in the new distribution.
- The new WallSprint source is distributed under GPL-3.0-or-later. Complete source for this experimental build is supplied alongside the diagnostic binary. GCC runtime use is subject to its upstream runtime library exception. Toolchain packages are obtained from signed Debian repository metadata and are not included in the mod ZIP.

- v0.1a4 adds statically linked TsudaKageyu/MinHook v1.3.4, commit c3fcafdc10146beb5919319d0683e44e3c30d537. Full upstream notices, including the HDE decoder notices, are retained in vendor/minhook/LICENSE.txt and vendor/minhook/src/hde. No separate MinHook DLL is required.

- v0.1a8 offline analysis uses Capstone5.0.6 (BSD license), https://github.com/capstone-engine/capstone. Installed only in the cloud analysis venv with the pinned wheel checksum; no Capstone library is linked or redistributed inside WallSprint.dll. The tiny native-code fixtures are checksum-identified excerpts supplied by the user telemetry, not executable tools or a whole game binary.
