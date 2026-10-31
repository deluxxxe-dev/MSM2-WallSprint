"""Validate actual owned fixture trace coverage and unchanged primary verdicts."""
import json
from pathlib import Path
import unittest

BUILD = Path(__file__).resolve().parents[1] / 'build'


class DiagnosticsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.fixed = json.loads((BUILD / 'presence-owned-fixed/result.json').read_text())
        cls.legacy = json.loads((BUILD / 'presence-owned-legacy-counterexample/result.json').read_text())

    def test_all_55_primary_checks_and_four_independent_variants(self):
        for report in (self.fixed, self.legacy):
            traces = report['stackwalk_diagnostics']
            self.assertEqual(len(traces), 220)
            assertions = {x['name']: x['pass'] for x in report['assertions'] if x['name'].endswith(' StackWalk64 caller')}
            self.assertEqual(len(assertions), 55)
            primary = [x for x in traces if x['variant'] == 'baseline_zero_context_frame_rsp']
            self.assertEqual(len(primary), 55)
            for trace in primary:
                self.assertEqual(assertions[trace['sample'] + ' StackWalk64 caller'], trace['predicate_pass'])
                variants = {x['variant'] for x in traces if x['sample'] == trace['sample']}
                self.assertEqual(variants, {'baseline_zero_context_frame_rsp', 'captured_context_frame_rsp', 'captured_context_frame_rbp', 'baseline_context_genuine_call_return'})

    def test_unhooked_controls_run_before_minhook(self):
        names = [x['name'] for x in self.fixed['assertions']]
        hook = names.index('real MinHook initialize')
        controls = [i for i, x in enumerate(names) if 'original+' in x]
        self.assertEqual(len(controls), 36)
        self.assertTrue(all(i < hook for i in controls))
        self.assertTrue(self.fixed['stackwalk_diagnostics'][0]['sample'].startswith('request original+0'))

    def test_bounded_step_callback_metadata_and_backing_stack(self):
        for report in (self.fixed, self.legacy):
            self.assertEqual(report['diagnostic_revision'], 2)
            self.assertIn('known', report['dbghelp_file_version'])
            self.assertIn('revision', report['dbghelp_api'])
            for trace in report['stackwalk_diagnostics']:
                self.assertLessEqual(trace['teb_low'], trace['stack_low'])
                self.assertLessEqual(trace['stack_high'], trace['teb_high'])
                self.assertLessEqual(trace['stack_low'], trace['initial_sp'])
                self.assertLess(trace['initial_sp'], trace['stack_high'])
                self.assertLessEqual(len(trace['callbacks']), 24)
                self.assertGreaterEqual(trace['callback_dropped'], 0)
                self.assertGreaterEqual(len(trace['steps']), 1)
                self.assertLessEqual(len(trace['steps']), 3)
                for step in trace['steps']:
                    self.assertIn('last_error_after_clear', step)
                    self.assertIn('return_pc', step)
                    self.assertIn('context_sp', step)
                for event in trace['callbacks']:
                    self.assertIn(event['kind'], (1, 2))

    def test_independent_virtual_unwind_and_legacy_failure_preserved(self):
        self.assertEqual((self.fixed['pass'], self.fixed['fail']), (201, 0))
        failed = [x['name'] for x in self.legacy['assertions'] if not x['pass']]
        self.assertEqual(len(failed), 32)
        self.assertTrue(all(x.startswith('observer restore ') for x in failed))
        for report in (self.fixed, self.legacy):
            self.assertFalse(report['production_admitted'])
            virtual = report['virtual_unwind_diagnostics']
            self.assertEqual(len(virtual), 55)
            self.assertEqual(sum(x['available'] for x in virtual), 48)
        self.assertTrue(all(x['pc'] == x['expected_pc'] and x['sp'] == x['expected_sp'] for x in self.fixed['virtual_unwind_diagnostics'] if x['available']))


if __name__ == '__main__':
    unittest.main()
