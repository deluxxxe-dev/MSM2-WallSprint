"""Run the complete c67 focused suite under the converted-c64 correction layer."""
from contextlib import redirect_stderr, redirect_stdout
import io
from pathlib import Path
import runpy

import slot_publication_post_c65_boundary_fix as fix
import slot_publication_post_c65_boundary as base

assert base.View is fix.View
assert base.sources()['slot_publication_post_c65_boundary_fix_py']

# The historical focused file deliberately imports the canonical base module.
# Importing the correction first patches only c67's observer globals, so every
# existing c67 assertion executes against the corrected observer while c56-c66
# modules/evidence remain unchanged.
runpy.run_path(str(Path(__file__).with_name('test_slot_publication_post_c65_boundary.py')),run_name='__main__')

assert base.View is fix.View
print('PASS: c67 converted-c64 correction layer preserves the complete focused c67 contract')
