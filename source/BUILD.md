# WallSprint v0.1b6 build — package0.1.15

クラウドにはゲーム実行環境がありません。Windows x64 DLLをcross-buildし、Wineで独立native/hardware/ABI試験を行います。ゲーム内のreset特定はREADME_TEST.mdのA/Eテスト待ちです。

```bash
WALLSPRINT_WITH_WINE=1 bash tools/setup_cloud.sh
bash tools/setup_analysis.sh
bash tools/test.sh
bash tools/build_windows.sh
bash tools/smoke_windows.sh
bash tools/test_hooks_windows.sh
python tools/release.py
```

既存GCC14POSIX/Clang19 SEH leaf/MinHook1.3.4/Wine10/Capstone5.0.6を使用。追加runtime依存なし。新コードはsrc/NativeHooks/immediate_reset_trace.cpp。実際のDR/VEH低層は既存src/msm2/orientation_trace.cppを再利用。元writerはABI-neutral tailjumpのままです。CMakeはこのtested cross-buildを案内して終了します。

```bash
/workspace/toolchains/analysis-python/bin/python tools/review_reset_writers.py /path/to/WallSprint.log --a-session 1 --e-session 2 --output RESET_REVIEW.json
```

session番号は録画/実ログでA/E対応を確認して指定します。decoderはAMD64関数境界からの完全なprivate capture、Capstone命令長・実効書込先・source64byte・QPC・callerを突き合わせます。known-hook prefixの復元は584byte全体SHAが既知originalと一致する場合のみ。実機header/build gateとA/E再現が揃わなければRESET_WRITER_VERIFIED=0。

`tools/test_reset_trace.sh`は独自native writer/owned memoryへの実MinHookとhardware breakpoint、同一/別writer、短時間deadline、helper request/stop、source snapshot、debug register復元を試験。portable/source/ABI/lifecycle等の既存回帰も継続。private captured game fixtureがない公開sourceでは当該optional replayのみSKIPし、独立試験は実行します。

packageはversion/DLL/README/INI、PE32+/2exports/systemimports/no WriteProcessMemory、CRC/hash/source/licenses、private capture除外を検証。出力: WallSprint_v0.1b6_reset_writer_trace.zip / WallSprint_v0.1b6_source.zip。生のユーザーログやfull game functionは含めません。
