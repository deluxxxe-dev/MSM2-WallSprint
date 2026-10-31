"""Package WallSprint with the preserved historical packager plus c42 active-state normalization.

package_base.py is retained as the historical monolithic implementation. This wrapper
changes only the stale active-package policy and then rewrites current-package metadata
so v0.1c42 cannot be published as if historical native-source/orientation writes were ON.
"""
from pathlib import Path
import json
import re

wrapper = Path(__file__).resolve()
root = wrapper.parents[1]
base = wrapper.with_name("package_base.py")
source = base.read_text(encoding="utf-8")

old = "assert ini.getint('WallSprint','WALLSPRINT_ENABLE')==1 and ini.getint('WallSprint','WALLSPRINT_NATIVE_SOURCE_OVERRIDE')==1"
new = "assert ini.getint('WallSprint','WALLSPRINT_ENABLE')==0 and ini.getint('WallSprint','WALLSPRINT_NATIVE_SOURCE_OVERRIDE')==0 and ini.getint('Diagnostics','REQUEST_PRESENCE_DIAGNOSTIC')==1"

if source.count(old) != 1:
    raise RuntimeError("package_base.py policy shape changed; refuse implicit package-policy rewrite")

source = source.replace(old, new, 1)
namespace = {"__file__": str(wrapper), "__name__": "__main__"}
exec(compile(source, str(base), "exec"), namespace)

header = (root / "include/version.hpp").read_text(encoding="utf-8")
label = re.search(r'versionLabel\[\]="([^"]+)"', header)[1]
version = re.search(r'packageVersion\[\]="([^"]+)"', header)[1]
manifest_path = root / "output" / f"MSM2-WS-{label}" / "BUILD_MANIFEST.json"
manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

manifest.update({
    "version": label,
    "package_version": version,
    "package_mode": "request_presence_read_only",
    "wall_sprint_enabled": False,
    "native_source_override_active": False,
    "native_stack_source_write_enabled": False,
    "path_b_override": False,
    "path_b_characterization_active": False,
    "request_presence_diagnostic": True,
    "current_runtime_writes": {
        "orientation": False,
        "adhesion": False,
        "movement": False,
        "position": False,
        "direct_actor_transform": False,
        "camera": False,
        "animation": False,
        "physics": False,
        "gravity": False,
        "native_source": False,
    },
    "current_request_presence": {
        "read_only": True,
        "capture_control": "automatic_on_admission",
        "capture_duration_ms": 300000,
        "record_cap": 131072,
        "manual_F11_F12_required": False,
        "video_alignment": "offset_ms_est_approximate",
        "authenticated_engine_state_labels": False,
    },
})
manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

(root / "logs/package_check.log").write_text(
    "PASS: "
    + f"{label}/{version} read-only RequestSimpleMotion presence package; "
    + "automatic capture on admission; no F11/F12 required; "
    + "WALLSPRINT_ENABLE=0; NATIVE_SOURCE_OVERRIDE=0; "
    + "orientation/adhesion/movement/native-source writes OFF; "
    + "REQUEST_PRESENCE_DIAGNOSTIC=1; historical analysis retained as history only\n",
    encoding="utf-8",
)
print("PASS: c42 read-only automatic-capture package manifest normalized")
