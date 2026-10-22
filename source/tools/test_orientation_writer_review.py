from pathlib import Path
import tempfile,re
from review_orientation_writers import review
root=Path(__file__).resolve().parents[1];source=root/'logs/windows_orientation_trace_synthetic.log';text=source.read_text();result=review(source)
assert any(g['same_function_in_both_states'] for g in result['function_groups'])
assert result['writers'] and all(w['resolved_hits']==w['events'] for w in result['writers'])
assert any(w['same_store_observed_in_both_states'] for w in result['writers'])
assert any(w['Crawl_axis_aligned_count']>0 for w in result['writers'])
assert any(w['writes'] and any('source_register' in v for v in w['writes']) for w in result['writers'])
assert not any(result[k] for k in ['actual_game_capture','success_A','success_B_candidate','success_C','actor_write_permission'])
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'bad.log'
 p.write_text(re.sub(r' rip=0x[0-9a-f]+',' rip=0x1',text));r=review(p);assert not any(w['resolved_hits'] for w in r['writers'])
 def moved(m):return m[1]+','.join(f'{int(v,16)+4096:x}' for v in m[2].split(','))
 p.write_text(re.sub(r'(post_gpr_[A-Z0-9_]+=)([a-f0-9,]+)',moved,text));r=review(p);assert not any(w['resolved_hits'] for w in r['writers'])
 # A valid code boundary alone cannot turn NOPs or missing captures into writers.
 p.write_text(re.sub(r'(orientation_writer_code [^\n]* bytes=)([a-f0-9]+)',lambda m:m[1]+'90'*(len(m[2])//2),text));r=review(p);assert not any(w['resolved_hits'] for w in r['writers'])
 p.write_text('\n'.join(x for x in text.splitlines() if not ('orientation_writer_code ' in x and 'offset=0x0 ' in x)));r=review(p);assert not any(w['resolved_hits'] for w in r['writers'])
print('PASS: actual hardware fixture RIP-after -> anchored store/effective Up address/source XMM, shared WallRun/CrawlCandidate writer; wrong PC/address/NOP/incomplete code rejected; synthetic capture never game evidence or upstream permission')
