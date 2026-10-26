# Source provenance

- Public game API headers and Transform accessor: hbgda/SM2ScriptTemplate, commit 999d0c7437720be5378049455209e69447f4a930. The files retained under vendor/SM2ScriptTemplate are unmodified; their hashes are in analysis/sdk_sources.json. No prebuilt DLL or .lib is reused. No separate LICENSE file was present in that upstream snapshot.
- Tkachov/MSMR-Scripting, commit 994ad7042452f892bef3a338731f9e7da69c855e, documents that its SDK is based on the above template and is GPLv3. Its license text is retained under vendor/MSMR-Scripting/LICENSE.txt. Its MSMR-specific RVA and runtime DLL are not used.
- Loader ABI/package behavior was checked against Tkachov/Overstrike commit 9f906ca09ea5ce4b84528410171c235bc4b0bfe6. No loader binary/code is included in this package.
- Reference ActorRelativeCamera/WalkOnWalls/SwingCore binaries/assets are not included in the new distribution.
- The new WallSprint source is distributed under GPL-3.0-or-later. Complete source for this experimental build is supplied alongside the diagnostic binary. GCC runtime use is subject to its upstream runtime library exception. Toolchain packages are obtained from signed Debian repository metadata and are not included in the mod ZIP.

- v0.1a4 adds statically linked TsudaKageyu/MinHook v1.3.4, commit c3fcafdc10146beb5919319d0683e44e3c30d537. Full upstream notices, including the HDE decoder notices, are retained in vendor/minhook/LICENSE.txt and vendor/minhook/src/hde. No separate MinHook DLL is required.

- Capstone 5.0.6, https://github.com/capstone-engine/capstone, is used for offline analysis and, since v0.1c2, statically linked for the bounded startup x86-64 proof. No external Capstone DLL is required. The build fetches upstream tag 5.0.6 source with archive SHA256 240ebc834c51aae41ca9215d3190cc372fd132b9c5c8aa2d5f19ca0c325e28f9. Complete upstream BSD and LLVM notices are included in source/vendor/capstone/LICENSE.TXT and LICENSE_LLVM.TXT (vendor/capstone/ inside the source ZIP). The fetch/build recipe is tools/setup_runtime_decoder.sh. Private game-function captures used for proof regression are not redistributed.
