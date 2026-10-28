#!/usr/bin/env python3
"""Prevent executable prototype tests being silently promoted into full acceptance."""

from __future__ import annotations

import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def audit() -> dict:
    requirements = json.loads((ROOT / "roadmap" / "requirements.json").read_text())
    coverage = json.loads((ROOT / "roadmap" / "executable-test-coverage.json").read_text())
    cmake = (ROOT / "CMakeLists.txt").read_text()
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
        ctest = item["ctest"]
        if ctest.startswith(("numerics.", "coupling.")):
            prefix, suffix = ctest.split(".", 1)
            assert f'NAME "{prefix}.${{case_name}}"' in cmake
            assert re.search(r"\b" + re.escape(suffix) + r"\b", cmake)
            assert re.search(r'\{"' + re.escape(suffix) + r'",\s*' + re.escape(suffix),
                             runner.read_text()), f"C++ dispatch missing {suffix}"
        else:
            assert f"NAME {ctest}" in cmake, f"CTest not registered: {ctest}"
    for ctest in coverage["additional_executable_prototype_tests"]:
        if ctest.startswith("numerics."):
            suffix = ctest.split(".", 1)[1]
            assert re.search(r"\b" + re.escape(suffix) + r"\b", cmake)
        else:
            assert f"NAME {ctest}" in cmake
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
