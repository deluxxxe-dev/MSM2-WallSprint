#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
python3 source/tools/check_production_vch_witness_review.py
