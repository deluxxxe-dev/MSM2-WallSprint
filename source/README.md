# v0.1b9 diagnostic update

Current version0.1.18: Path B is read-only, Path A interpolation is a reference. See README_TEST.md and analysis/V01B9_GATE.md. Historical version claims below do not grant Path B layout/ownership proof. Build with tools/build_windows.sh; run tools/test.sh, tools/smoke_windows.sh, tools/test_hooks_windows.sh and tools/release.py. Original game captures stay private.

# WallSprint v0.1b8 — Dual-Source Orientation Ownership

Package0.1.17。既存 common-writer と、b7 で確認された reset-copy writer の exact caller-local source へ、単一の SurfaceFrame 補間姿勢を供給します。Position は変更せず、direct Actor / reset patch / adhesion / movement / animation / camera / physics は追加しません。**実機 b8 保持は未確認**。

導入・A/E 操作は [README_TEST.md](README_TEST.md)、再現手順は [BUILD.md](BUILD.md)。設計は `analysis/DUAL_SOURCE_OWNERSHIP.md`、認証は `analysis/RESET_COPY_WRITER_ANALYSIS.md`、gate は `analysis/V01B8_GATE.md`。GitHub 保存先と検証結果は `docs/CODEX_RESULT.md`。非公開実機ログ・full game functions・参考 MOD payload は配布しません。
