#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build logs

# Fail fast on the active c48 read-only WallRun scalar/threshold/config/version/package contract
# (including the dormant c45 presence-hook identity/safety contract) before long suites.
python3 tools/test_face_physics.py
python3 tools/test_state_snapshots.py
python3 tools/test_face_frame.py
python3 tools/test_face_landing.py
python3 tools/test_face_control.py
python3 tests/request_presence_static.py
python3 tests/wallrun_scalar_writer_tool_static.py
python3 tests/wallrun250_consumer_v8_static.py
python3 tests/wallrun250_threshold_probe_static.py
python3 tools/test_build_wallrun_m1_experiment.py
python3 tools/test_ws_assets.py

g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/core_tests.cpp -o build/core_tests
./build/core_tests | tee logs/core_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/pattern_tests.cpp -o build/pattern_tests
./build/pattern_tests | tee logs/pattern_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/diagnostic_tests.cpp -o build/diagnostic_tests
./build/diagnostic_tests | tee logs/diagnostic_tests.log

g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/wallrun_observer_tests.cpp -o build/wallrun_observer_tests
./build/wallrun_observer_tests replay < tools/fixtures/v01a4_events.txt | tee logs/wallrun_observer_tests.log

g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/surface_calibration_tests.cpp -o build/surface_calibration_tests
./build/surface_calibration_tests | tee logs/surface_calibration_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/four_face_tests.cpp -o build/four_face_tests
./build/four_face_tests | tee logs/four_face_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/static_field_tests.cpp -o build/static_field_tests
./build/static_field_tests | tee logs/static_field_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/wallrun_scalar_observation_tests.cpp -o build/wallrun_scalar_observation_tests
./build/wallrun_scalar_observation_tests | tee logs/wallrun_scalar_observation_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/wallrun_direction_scan_tests.cpp -o build/wallrun_direction_scan_tests
./build/wallrun_direction_scan_tests | tee logs/wallrun_direction_scan_tests.log

g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/surface_pair_tests.cpp -o build/surface_pair_tests
./build/surface_pair_tests | tee logs/surface_pair_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/planar_mvp_tests.cpp -o build/planar_mvp_tests
./build/planar_mvp_tests | tee logs/planar_mvp_tests.log

g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/source_orientation_tests.cpp -o build/source_orientation_tests
./build/source_orientation_tests | tee logs/source_orientation_tests.log

g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/NativeHooks/planar_standoff_smoke.cpp -o build/planar_standoff_smoke
./build/planar_standoff_smoke | tee logs/planar_standoff_tests.log
g++ -std=c++17 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined -g -Iinclude src/movement_delta_tests.cpp -o build/movement_delta_tests
./build/movement_delta_tests | tee logs/movement_delta_tests.log

bash tools/test_reversal_heading.sh
bash tools/test_wall_motion_window.sh

python3 tools/test_select_wallrun_comparison.py
