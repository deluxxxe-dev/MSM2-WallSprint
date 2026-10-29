#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import io
import json
import zipfile

import build_wallrun_steering_probe as probe


def expect_raises(fn, text: str) -> None:
    try:
        fn()
    except ValueError as exc:
        assert text in str(exc), (text, str(exc))
        return
    raise AssertionError(f"expected ValueError containing {text!r}")


def test_exact_one_byte_patch() -> None:
    data = bytes(range(32))
    sha = hashlib.sha256(data).hexdigest()
    out = probe.patch_one_byte(
        data,
        expected_sha256=sha,
        expected_size=len(data),
        offset=7,
        expected_old=7,
        new_value=99,
    )
    assert len(out) == len(data)
    assert out[7] == 99
    assert [i for i, (a, b) in enumerate(zip(data, out)) if a != b] == [7]
    print("PASS: authenticated one-byte patch changes exactly the requested offset")


def test_fail_closed_identity_checks() -> None:
    data = bytes(range(16))
    sha = hashlib.sha256(data).hexdigest()

    expect_raises(
        lambda: probe.patch_one_byte(
            data + b"x",
            expected_sha256=sha,
            expected_size=len(data),
            offset=3,
            expected_old=3,
            new_value=4,
        ),
        "unexpected input size",
    )
    expect_raises(
        lambda: probe.patch_one_byte(
            data,
            expected_sha256="0" * 64,
            expected_size=len(data),
            offset=3,
            expected_old=3,
            new_value=4,
        ),
        "unexpected input SHA256",
    )
    expect_raises(
        lambda: probe.patch_one_byte(
            data,
            expected_sha256=sha,
            expected_size=len(data),
            offset=3,
            expected_old=8,
            new_value=4,
        ),
        "unexpected byte",
    )
    print("PASS: probe builder fails closed on size, SHA256, and original-byte mismatch")


def test_stage_layout() -> None:
    payload = b"synthetic-config"
    for spec in probe.PROBES.values():
        stage_bytes = probe.make_stage(payload, spec)
        with zipfile.ZipFile(io.BytesIO(stage_bytes), "r") as zf:
            expected_member = f"0/{probe.ASSET_PATH}"
            assert zf.namelist() == [expected_member, "info.json"]
            assert zf.read(expected_member) == payload
            info = json.loads(zf.read("info.json"))
            assert info["game"] == "MSM2"
            assert info["name"] == spec.mod_name
            assert info["format_version"] == 2
    print("PASS: both steering probes package only the intended Standard asset plus info.json")


def test_outer_zip_layout() -> None:
    stage = b"synthetic-stage"
    name = "probe.stage"
    outer = probe.make_outer_zip(stage, name)
    with zipfile.ZipFile(io.BytesIO(outer), "r") as zf:
        assert zf.namelist() == [name]
        assert zf.read(name) == stage
    print("PASS: outer package contains exactly one stage archive")


def test_production_contract_constants() -> None:
    assert probe.EXPECTED_SIZE == 544
    assert probe.EXPECTED_SHA256 == "32418c218d2feb62fb9227caec321c00278e4500bc75db6c4015888359537b7b"
    assert probe.ASSET_PATH == "configs/hero/hero_WallRunSteeringSetup_Standard.config"

    fast = probe.PROBES["fast10"]
    assert (fast.field, fast.offset, fast.vanilla, fast.probe) == ("TurnSpeedFast", 0x218, 100, 10)
    slow = probe.PROBES["slow5"]
    assert (slow.field, slow.offset, slow.vanilla, slow.probe) == ("TurnSpeedSlow", 0x219, 75, 5)
    assert fast.offset != slow.offset
    print("PASS: production contract pins Fast 100->10 and Slow 75->5 as separate one-byte probes")


def main() -> int:
    test_exact_one_byte_patch()
    test_fail_closed_identity_checks()
    test_stage_layout()
    test_outer_zip_layout()
    test_production_contract_constants()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
