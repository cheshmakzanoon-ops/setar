#!/usr/bin/env python3
"""Mutation tests for the engineering traceability auditor (no external packages)."""
from __future__ import annotations

from copy import deepcopy
from pathlib import Path
import sys
import unittest

from audit_implementation import audit, load_registries, validate_blank_specimen_template
import json


ROOT = Path(__file__).resolve().parents[1]


class ImplementationAuditorTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.phases, cls.requirements, cls.tasks = load_registries(ROOT)

    def inspect(self, change=None) -> list[str]:
        phases, requirements, tasks = (
            deepcopy(self.phases), deepcopy(self.requirements), deepcopy(self.tasks)
        )
        if change is not None:
            change(phases, requirements, tasks)
        return audit(ROOT, phases, requirements, tasks)["errors"]

    def test_real_registers_are_consistent(self) -> None:
        self.assertEqual([], self.inspect())

    def test_duplicate_requirement(self) -> None:
        self.assertTrue(any("duplicate identifier" in x for x in self.inspect(
            lambda p, r, t: r["requirements"].append(deepcopy(r["requirements"][0])))))

    def test_duplicate_task(self) -> None:
        self.assertTrue(any("duplicate identifier" in x for x in self.inspect(
            lambda p, r, t: t["tasks"].append(deepcopy(t["tasks"][0])))))

    def test_orphan_requirement(self) -> None:
        def mutate(p, r, t):
            for task in t["tasks"]:
                task["requirements"] = [x for x in task["requirements"] if x != "REQ-MNT-002"]
        self.assertTrue(any("no engineering task" in x for x in self.inspect(mutate)))

    def test_stale_task_reference(self) -> None:
        self.assertTrue(any("unknown requirement" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0]["requirements"].append("REQ-XXX-999"))))

    def test_missing_reading(self) -> None:
        self.assertTrue(any("missing source reading" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0]["read"].append("docs/does-not-exist.md"))))

    def test_path_escape(self) -> None:
        self.assertTrue(any("non-repository" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0]["touch"].append("../outside.md"))))

    def test_dependency_cycle(self) -> None:
        self.assertTrue(any("cycle" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0]["requires"].append("ENG-P19-T02"))))

    def test_unknown_dependency(self) -> None:
        self.assertTrue(any("unknown dependency" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0]["requires"].append("ENG-P18-T99"))))

    def test_unknown_phase(self) -> None:
        self.assertTrue(any("phase must be known" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0].update(phase="P88"))))

    def test_calendar_policy(self) -> None:
        self.assertTrue(any("calendar/estimate" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0].update(deadline="soon"))))

    def test_missing_verification(self) -> None:
        self.assertTrue(any("at least 2" in x for x in self.inspect(
            lambda p, r, t: t["tasks"][0].update(verify=["sounds plausible"]))))

    def test_duplicate_test_owner(self) -> None:
        self.assertTrue(any("duplicated test ownership" in x for x in self.inspect(
            lambda p, r, t: r["requirements"][1]["test_cases"].append(
                r["requirements"][0]["test_cases"][0]))))

    def test_public_template_cannot_gain_fake_measurements(self) -> None:
        template = json.loads((ROOT / "data/specimen-template.json").read_text(encoding="utf-8"))
        self.assertEqual([], validate_blank_specimen_template(template))
        template["strings"][0]["tension_n"] = 50.0
        self.assertTrue(any("tension_n" in x for x in validate_blank_specimen_template(template)))

    def test_public_template_cannot_self_certify(self) -> None:
        template = json.loads((ROOT / "data/specimen-template.json").read_text(encoding="utf-8"))
        template["validation_status"] = "fully_validated"
        self.assertTrue(any("validation has occurred" in x for x in
                            validate_blank_specimen_template(template)))


if __name__ == "__main__":
    unittest.main()
