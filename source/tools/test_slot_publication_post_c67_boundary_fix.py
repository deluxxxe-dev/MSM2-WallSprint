"""Run the complete c69 focused suite under the checkpoint correction layer."""
import runpy

import slot_publication_post_c67_boundary_fix as fix
import slot_publication_post_c67_boundary as base

assert base.make_checkpoint is fix.make_checkpoint
assert base.validate_checkpoint is fix.validate_checkpoint
assert base.sources is fix.sources

runpy.run_path('source/tools/test_slot_publication_post_c67_boundary.py',run_name='__main__')
print('PASS: c69 checkpoint correction preserves the complete focused c69 contract')
