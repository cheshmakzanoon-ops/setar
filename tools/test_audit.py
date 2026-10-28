#!/usr/bin/env python3
"""Test auditing, not the scientific correctness of the handbook."""
import json
from pathlib import Path
import tempfile
import unittest
from audit_docs import audit, local_links, phase_errors, prose_words, strip_fences


class AuditTests(unittest.TestCase):
    def test_fences_and_prose(self):
        text = "# A useful heading\n\nTwo prose words.\n```cpp\nignored code\n```\n"
        self.assertNotIn("ignored", strip_fences(text))
        self.assertEqual(prose_words(text), 6)

    def test_links_ignore_code(self):
        self.assertEqual(local_links("[valid](child.md)\n```md\n[x](not-real.md)\n```"), ["child.md"])

    def test_broken_link(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "README.md").write_text("[missing](missing.md)", encoding="utf-8")
            self.assertEqual(len(audit(root)["errors"]), 1)
            (root / "missing.md").write_text("# Found\n", encoding="utf-8")
            self.assertEqual(audit(root)["errors"], [])

    def test_cycle(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "roadmap").mkdir()
            (root / "phase.md").write_text("Phase", encoding="utf-8")
            phases = [{"id": "P00", "document": "phase.md", "requires": ["P01"]},
                      {"id": "P01", "document": "phase.md", "requires": ["P00"]}]
            (root / "roadmap/phase-register.json").write_text(json.dumps({"phases": phases}), encoding="utf-8")
            self.assertIn("cycle", phase_errors(root)[0])

    def test_calendar_field(self):
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / "roadmap").mkdir()
            (root / "phase.md").write_text("Phase", encoding="utf-8")
            phase = {"id": "P00", "document": "phase.md", "requires": [], "deadline": "none"}
            (root / "roadmap/phase-register.json").write_text(json.dumps({"phases": [phase]}), encoding="utf-8")
            self.assertIn("calendar", phase_errors(root)[0])


if __name__ == "__main__":
    unittest.main()
