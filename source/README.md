# v0.1c6 planar adhesion diagnostics

Current version0.1.24: exact-build Path A and compact Path B consume one lock-free coherent orientation snapshot. Path B writes only its first nine floats and preserves row scale and all later metadata. See README_TEST.md and analysis/V01C6_GATE.md. Build with tools/build_windows.sh; run tools/test.sh, tools/smoke_windows.sh, tools/test_hooks_windows.sh and tools/release.py. c5 full-WallRun orientation is real-game verified. c6 performs startup Position candidate inspection and read-only standoff control evaluation; Position writes remain disabled because whole-path/W/exclusive-effect proof is incomplete; third paths remain unmodified. Original game captures stay private.

# WallSprint v0.1b8 — Dual-Source Orientation Ownership

Package0.1.17。既存 common-writer と、b7 で確認された reset-copy writer の exact caller-local source へ、単一の SurfaceFrame 補間姿勢を供給します。Position は変更せず、direct Actor / reset patch / adhesion / movement / animation / camera / physics は追加しません。**実機 b8 保持は未確認**。

導入・A/E 操作は [README_TEST.md](README_TEST.md)、再現手順は [BUILD.md](BUILD.md)。設計は `analysis/DUAL_SOURCE_OWNERSHIP.md`、認証は `analysis/RESET_COPY_WRITER_ANALYSIS.md`、gate は `analysis/V01B8_GATE.md`。GitHub 保存先と検証結果は `docs/CODEX_RESULT.md`。非公開実機ログ・full game functions・参考 MOD payload は配布しません。
