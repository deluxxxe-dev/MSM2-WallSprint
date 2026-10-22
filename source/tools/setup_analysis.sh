#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
analysis_venv=${WALLSPRINT_ANALYSIS_VENV:-/workspace/toolchains/analysis-python}
if [[ ! -x "$analysis_venv/bin/python" ]]; then python -m venv "$analysis_venv"; fi
"$analysis_venv/bin/python" -m pip install --require-hashes --only-binary=:all: --no-deps -r tools/analysis-requirements.txt
"$analysis_venv/bin/python" -c 'import capstone; assert capstone.__version__=="5.0.6"; print("PASS: pinned offline VEX/AVX decoder")'
