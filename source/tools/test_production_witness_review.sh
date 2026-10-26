#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
gpp=${WALLSPRINT_TOOLCHAIN_ROOT:-/workspace/toolchains/windows}/usr/bin/x86_64-w64-mingw32-g++-posix
"$gpp" -std=c++17 -O2 -g -Wall -Wextra -Werror -DNOMINMAX -Iinclude -static src/NativeHooks/production_witness_review_smoke.cpp build/windows/guard.o build/windows/path_a_position_proof.o build/windows/movement_lineage.o build/windows/rdx_source_contract.o build/windows/writer_entry_snapshot.o build/windows/writer_entry_thunk.o build/deps/capstone-5.0.6/capstone.lib build/windows/minhook_buffer.o build/windows/minhook_hook.o build/windows/minhook_trampoline.o build/windows/minhook_hde64.o -lbcrypt -o build/windows/production_witness_review_smoke.exe
 : > logs/windows_production_witness_review.log
for scenario in close --timeout --kill; do
 for mode in reset position; do
  args=();if [[ "$scenario" != close ]];then args+=("$scenario");fi;if [[ "$mode" == position ]];then args+=(--position);fi
  WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/production_witness_review_smoke.exe" "${args[@]}" | tee -a logs/windows_production_witness_review.log
 done
done
for mode in reset position; do
 args=(--foreign-only);if [[ "$mode" == position ]];then args+=(--position);fi
 WINEPREFIX=/workspace/toolchains/wine-prefix WINEDLLPATH=/workspace/toolchains/wine/usr/lib/x86_64-linux-gnu/wine WINEDEBUG=-all WINEDLLOVERRIDES='mscoree,mshtml=' /workspace/toolchains/wine/usr/lib/wine/wine64 "$PWD/build/windows/production_witness_review_smoke.exe" "${args[@]}" | tee -a logs/windows_production_witness_review.log
done
python3 - <<'CHECK'
import json
from pathlib import Path
rows=[json.loads(s) for s in Path('logs/windows_production_witness_review.log').read_text().splitlines() if s.startswith('{')]
assert len(rows)==8
mixed=[r for r in rows if not r['foreign_only']];foreign=[r for r in rows if r['foreign_only']]
assert {(r['mode'],r['scenario']) for r in mixed}=={(m,s) for m in ['reset','Position'] for s in ['close','deadline','kill']}
assert {r['mode'] for r in foreign}=={'reset','Position'}
for r in rows:
 assert r['production_result']=='CONTINUE_SEARCH' and r['foreign_events']==1 and r['inflight_while_held']==0 and r['observer_context_writes']==0
 assert r['owned_hits']==(0 if r['foreign_only'] else 1)
 for key in ['saved_context_armed','bookkeeping_inactive_after_poll','current_slots_restored_while_held']:assert r[key] is True
 assert r['engine_context_unchanged'] is r['foreign_only']
 assert r['owned_slots_resurrect_after_return'] is (not r['foreign_only'])
 assert r['production_integrated'] is False and r['game_used'] is False
 for reg in ['Dr0','Dr1','Dr2','Dr3','Dr7']:assert r['while_held'][reg]==r['saved'][reg]
 assert r['after_return']['Dr2']==r['saved']['Dr2']
 assert int(r['after_return']['Dr7'],16)&0xf==(0 if r['foreign_only'] else 5)
print('PASS: c48 six real mixed-cause SEARCH resurrection cases and two foreign-only non-resurrection observations stay distinct; no general certificate')
CHECK
