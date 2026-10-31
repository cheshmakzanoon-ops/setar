#!/usr/bin/env python3
"""Prevent executable prototype tests being silently promoted into full acceptance."""

from __future__ import annotations

import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def instrument_registrations(cmake: str) -> dict[str, str]:
    """Map each CTest name registered by setar_instrument_tests(...) to its runner source."""
    registered: dict[str, str] = {}
    for call in re.finditer(r"setar_instrument_tests\(([^)]*)\)", cmake):
        words = call.group(1).split()
        if len(words) < 3 or "TESTS" not in words:
            continue
        source = words[1]
        for name in words[words.index("TESTS") + 1:]:
            assert name not in registered, f"CTest registered twice: {name}"
            registered[name] = source
    return registered


def dispatch_target(runner_text: str, ctest: str) -> str | None:
    """Return the function a full-name dispatch entry {"ctest", function} points to."""
    match = re.search(r'\{\s*"' + re.escape(ctest) + r'"\s*,\s*([A-Za-z_][\w:]*)', runner_text)
    return match.group(1) if match else None


def check_registered(ctest: str, cmake: str, instrument: dict[str, str],
                     runner: Path | None) -> None:
    """Accept the three registration styles used in CMakeLists.txt; reject stubs."""
    if ctest in instrument:
        source = ROOT / instrument[ctest]
        if runner is not None:
            assert runner.resolve() == source.resolve(), (
                f"{ctest}: coverage runner {runner} differs from CMake source {source}")
        assert source.is_file(), f"missing runner {source}"
        assert re.search(r"(?<![\w.])" + re.escape(ctest) + r"(?![\w.])", cmake)
        target = dispatch_target(source.read_text(), ctest)
        assert target is not None, f"C++ dispatch missing {ctest}"
        assert target != "not_implemented", f"{ctest} still dispatches to a not_implemented stub"
        return
    prefix, suffix = ctest.split(".", 1)
    if f'NAME "{prefix}.${{case_name}}"' in cmake:
        assert re.search(r"\b" + re.escape(suffix) + r"\b", cmake), f"CTest not registered: {ctest}"
        if runner is not None:
            assert re.search(r'\{"' + re.escape(suffix) + r'",\s*' + re.escape(suffix),
                             runner.read_text()), f"C++ dispatch missing {suffix}"
        return
    assert f"NAME {ctest}" in cmake, f"CTest not registered: {ctest}"


def audit() -> dict:
    requirements = json.loads((ROOT / "roadmap" / "requirements.json").read_text())
    coverage = json.loads((ROOT / "roadmap" / "executable-test-coverage.json").read_text())
    cmake = (ROOT / "CMakeLists.txt").read_text()
    instrument = instrument_registrations(cmake)
    planned = [test for req in requirements["requirements"] for test in req["test_cases"]]
    assert len(planned) == len(set(planned)), "planned test names must be unique"
    assert coverage["schema_version"] == 1
    assert coverage["coverage_status"] == "executable_partial"
    entries = coverage["entries"]
    ids = [item["planned_test_id"] for item in entries]
    assert len(ids) == len(set(ids)), "duplicate partial acceptance mapping"
    assert set(ids).issubset(set(planned)), "invented acceptance test identifier"
    for item in entries:
        assert item["status"] == "executable_partial", "no signed full acceptance recorded"
        assert len(item["verified_subset"]) >= 50, "must identify what was actually checked"
        assert len(item["remaining_gap"]) >= 50, "must record substantive unfinished coverage"
        runner = ROOT / item["runner"]
        assert runner.is_file(), f"missing runner {runner}"
        check_registered(item["ctest"], cmake, instrument, runner)
    additional = coverage["additional_executable_prototype_tests"]
    assert len(additional) == len(set(additional)), "duplicate additional prototype test"
    for ctest in additional:
        check_registered(ctest, cmake, instrument, None)
    assert len(entries) < len(planned), "partial prototype implementation is not a completed full matrix"
    return {
        "planned_full_acceptance_tests": len(planned),
        "executable_partial_acceptance_mappings": len(entries),
        "not_yet_mapped_to_partial_executable_prototype": len(planned) - len(entries),
        "full_acceptance_requirements_declared_passed": 0,
        "audit_status": "passed_no_overclaims",
    }


if __name__ == "__main__":
    print(json.dumps(audit(), indent=2))
