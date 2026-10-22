# WallSprint v0.1b7 — Pre-Armed Reset Writer Capture

Package 0.1.16。既存 source override を保持し、native WallRun の0.60秒で primary thread の Up write-watch を準備、0.70秒の source 変更より前に監視が入ったかを記録します。current Hero common writer の全 caller を bounded read-only 記録し、reset 候補と照合します。**RESET_WRITER_VERIFIED=0、実機 A/E 待ち**。

導入・操作は [README_TEST.md](README_TEST.md)、ビルドは [BUILD.md](BUILD.md)。診断は1.10秒または候補/Deinit/F8/identity loss/予算終了まで。reset patch・direct Actor・Position・adhesion/movement/camera/animation/physics は追加しません。非公開実機ログ・game capture・参考 MOD の payload は配布しません。
