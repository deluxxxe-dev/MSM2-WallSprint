"""Generate the version-only, WS-only local helper from its reviewed template."""
from pathlib import Path
import re
root=Path(__file__).resolve().parents[1]
label=re.search(r'versionLabel\[\]="([^"]+)"',(root/'include/version.hpp').read_text())[1]
assert re.fullmatch(r'v[0-9A-Za-z.]+',label)
name='MSM2-WS-'+label+'.zip'
body=(root/'tools/UpdateForTest.cmd.in').read_text().replace('@WS_PACKAGE_NAME@',name)
assert '@WS_PACKAGE_NAME@' not in body
for target in [root/'UpdateForTest.cmd']+([root.parent/'UpdateForTest.cmd'] if (root.parent/'AGENTS.md').is_file() else []):
 target.write_bytes(body.replace('\n','\r\n').encode('ascii'))
print('Generated UpdateForTest.cmd for',name)
