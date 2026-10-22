# WallSprint v0.1b6 — Immediate Reset Writer Trace

Package0.1.15。v0.1b5の実機A/Eログでsource matrix→Actor Up反映を確認したため、その限定source変更を維持し、直後にWorldUpへ戻すwriterを30msのhardware watchで調べる診断版です。**RESET_WRITER_VERIFIED=0、実機確認待ち**。

導入・操作は[README_TEST.md](README_TEST.md)。A/E各1回の左→右WallRunのみ、Crawl・マーカー不要。外側ZIPを展開し、中のWallSprint.scriptを解凍せずOverstrikeへ追加します。既存Script support/`-scripts`を使用、参考MOD・WSUは不要です。

[前回結果](analysis/V01B5_RESULT.md)、[短時間監視](analysis/IMMEDIATE_RESET_TRACE.md)、[候補](analysis/RESET_WRITER_CANDIDATES.md)、[gate](analysis/V01B6_GATE.md)、[build](BUILD.md)。source48byteの既存変更以外にActor/Camera/Physics/Animation/Positionは書き込みません。reset patch・新規未知ABI・新規native hookは追加していません。F8で停止、再試行は完全再起動。

独自NativeHooks/GameInterop実装と既存hardware trace基盤を使用。参考MODのcode/binary/assets、private game function capturesは配布しません。変更前backup: `/workspace/backups/WallSprint_before_v0.1b6.tar.gz`。
