"""Package WallSprint using the preserved monolithic implementation plus the current c40 read-only policy.

The large historical packaging implementation is kept byte-for-byte in package_base.py.
This wrapper changes only the configuration admission assertion that became stale when
v0.1c40 switched to the dedicated RequestSimpleMotion presence-only package.
"""
from pathlib import Path

wrapper = Path(__file__).resolve()
base = wrapper.with_name("package_base.py")
source = base.read_text(encoding="utf-8")

old = "assert ini.getint('WallSprint','WALLSPRINT_ENABLE')==1 and ini.getint('WallSprint','WALLSPRINT_NATIVE_SOURCE_OVERRIDE')==1"
new = "assert ini.getint('WallSprint','WALLSPRINT_ENABLE')==0 and ini.getint('WallSprint','WALLSPRINT_NATIVE_SOURCE_OVERRIDE')==0 and ini.getint('Diagnostics','REQUEST_PRESENCE_DIAGNOSTIC')==1"

if source.count(old) != 1:
    raise RuntimeError("package_base.py policy shape changed; refuse implicit package-policy rewrite")

source = source.replace(old, new, 1)
namespace = {"__file__": str(wrapper), "__name__": "__main__"}
exec(compile(source, str(base), "exec"), namespace)
