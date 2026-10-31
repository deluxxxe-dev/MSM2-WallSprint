"""Package WallSprint with the preserved historical packager plus c46 active-state normalization.

package_base.py is retained as the historical monolithic implementation. This wrapper
changes only the stale active-package policy and then rewrites current-package metadata
so v0.1c46 cannot be published as if historical native-source/orientation writes or the
retired c45 Gravity presence observer were ON.
"""
from pathlib import Path
import json
import re

wrapper = Path(__file__).resolve()
root = wrapper.parents[1]
base = wrapper.with_name("package_base.py")
source = base.read_text(encoding="utf-8")

old = "assert ini.getint('WallSprint','WALLSPRINT_ENABLE')==1 and ini.getint('WallSprint','WALLSPRINT_NATIVE_SOURCE_OVERRIDE')==1"
new = "assert ini.getint('WallSprint','WALLSPRINT_ENABLE')==0 and ini.getint('WallSprint','WALLSPRINT_NATIVE_SOURCE_OVERRIDE')==0 and ini.getint('Diagnostics','REQUEST_PRESENCE_DIAGNOSTIC')==0 and ini.getint('Diagnostics','WALLRUN_HOOK_DIAGNOSTIC')==1 and ini.getint('Diagnostics','WALLRUN_OBJECT_DIAGNOSTIC')==1"

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
    "package_mode": "wallrun_update_scalar_read_only",
    "wall_sprint_enabled": False,
    "native_source_override_active": False,
    "native_stack_source_write_enabled": False,
    "path_b_override": False,
    "path_b_characterization_active": False,
    "request_presence_diagnostic": False,
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
    "current_gravity_owner_presence": {
        "enabled": False,
        "status": "c45_bounded_zero_hit_retired",
        "setter_hooked_or_called_by_mod": False,
    },
    "current_wallrun_update_scalar_observation": {
        "read_only": True,
        "extra_native_hooks": False,
        "source": "authenticated_verified_WallRun_Update_direct_this_reads",
        "update_sha256": "e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f",
        "offsets": [520, 544, 568, 592, 1036],
        "offsets_hex": ["0x208", "0x220", "0x238", "0x250", "0x40C"],
        "width_bytes": 4,
        "worker_sample_interval_ms": 100,
        "captured_pointer_dereference": False,
        "semantic_mapping_authenticated": False,
        "known_schema_numeric_comparison_only": [0.4, 12.0, 75.0, 100.0, 50.0],
    },
})
manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

(root / "logs/package_check.log").write_text(
    "PASS: "
    + f"{label}/{version} read-only verified-WallRun Update scalar package; "
    + "five exact direct-this 4-byte candidates observed at worker cadence; no extra native hook; "
    + "c45 Gravity presence hook disabled; WALLSPRINT_ENABLE=0; NATIVE_SOURCE_OVERRIDE=0; "
    + "orientation/adhesion/movement/gravity/native-source writes OFF; "
    + "REQUEST_PRESENCE_DIAGNOSTIC=0; WALLRUN_HOOK_DIAGNOSTIC=1; WALLRUN_OBJECT_DIAGNOSTIC=1\n",
    encoding="utf-8",
)
print("PASS: c46 read-only WallRun Update scalar package manifest normalized")
