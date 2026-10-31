"""Check the actual owned native fixture's diagnostic payload, never game acceptance."""
import math
import re
import sys
from collections import Counter
from pathlib import Path

text=Path(sys.argv[1]).read_text()
counts=Counter();sessions=set();late=False;off_seen=False
for line in text.splitlines():
    if not line.startswith('WALL_MOTION_WINDOW schema=2 '):
        continue
    fields=dict(re.findall(r'(\w+)=([^ ]+)',line))
    session=(fields['mode'],fields['session'],fields['generation'])
    key=(*session,int(fields['window']));counts[key]+=1;sessions.add(session)
    assert counts[key]<=16 and key[-1]<16
    assert int(fields['decision_ms'])<=int(fields['ms'])
    assert int(fields['decision_update'])<=int(fields['native_update'])
    def vector(name):
        v=tuple(map(float,fields[name].strip('()').split(',')))
        assert len(v)==3 and all(math.isfinite(x) for x in v)
        return v
    n=vector('normal');sampled=vector('sampled_normal');d=vector('delta_world');t=vector('tangent_world')
    assert abs(sum(x*x for x in n)-1)<1e-4 and .9<sum(x*x for x in sampled)<1.1
    dn=sum(x*y for x,y in zip(d,n))
    assert all(abs(t[i]-(d[i]-dn*n[i]))<1e-4 for i in range(3))
    for name in (['requested_heading','published_heading'] if fields['target_valid']=='1' else [])+['actor_right','actor_up','actor_forward']:
        vector(name)
    assert fields['input_intent']=='unobserved' and fields['Position_writes']=='0'
    if fields['mode']=='OFF':
        off_seen=True
        assert fields['target_valid']=='0' and fields['requested_heading']=='unavailable' and fields['published_heading']=='unavailable'
        assert all(fields[k]=='0' for k in ['source_admitted','publication_active','publications','a_commits','b_commits'])
    late|=key[-1]>=8
assert sessions and late and off_seen
assert all(sum(v for k,v in counts.items() if k[:3]==s)<=256 for s in sessions)
assert 'WALL_MOTION_WINDOW_SUMMARY ' in text and 'overwritten_total=' in text and 'budget_suppressed_total=' in text
assert 'original_may_include_prior_WS_output=1' in text
print('PASS: actual owned native diagnostic vectors project correctly, separate clocks/late windows/caps and input limits retained; not native game causality')
