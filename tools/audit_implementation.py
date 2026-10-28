#!/usr/bin/env python3
"""Offline consistency check for proposed implementation registers, not physics QA."""
from __future__ import annotations

import argparse
import json
from pathlib import Path, PurePosixPath
import re
import sys


REQ_PATTERN = re.compile(r"REQ-[A-Z]{3}-\d{3}\Z")
TASK_PATTERN = re.compile(r"ENG-P\d{2}-T\d{2}\Z")
TEST_PATTERN = re.compile(r"[a-z][a-z0-9_]*(?:\.[a-z][a-z0-9_]*)+\Z")
FORBIDDEN_KEYS = {"deadline", "duration", "due_date", "estimate", "eta", "target_date"}


def load_registries(root: Path) -> tuple[dict, dict, dict]:
    """Read only repository-owned JSON; no dependencies or external calls."""
    return tuple(
        json.loads((root / name).read_text(encoding="utf-8"))
        for name in (
            "roadmap/phase-register.json",
            "roadmap/requirements.json",
            "roadmap/engineering-tasks.json",
        )
    )  # type: ignore[return-value]


def validate_blank_specimen_template(doc: dict) -> list[str]:
    """The public blank template must never silently become a measured asset."""
    errors: list[str] = []
    if doc.get("schema_version") != 1 or doc.get("instrument_family") != "iranian_setar":
        errors.append("specimen template: incorrect schema or instrument family")
    if doc.get("record_status") != "unmeasured_template":
        errors.append("specimen template: must be marked unmeasured_template")
    for field in ("specimen_id", "owner_permission_reference", "session_id", "setup_id",
                  "bowl_and_wooden_soundboard_geometry_source", "bridge_complex_mobility_source",
                  "nail_flesh_contact_source", "heldout_split_id", "training_split_id"):
        if doc.get(field) is not None:
            errors.append(f"specimen template: expected blank {field}")
    if doc.get("license_and_consent_status") != "unresolved":
        errors.append("specimen template: unreviewed license and consent must remain unresolved")
    if doc.get("physical_admissibility_status") != "not_evaluated" or doc.get(
            "validation_status") != "not_evaluated":
        errors.append("specimen template: no physical or measured validation has occurred")
    strings = doc.get("strings")
    if not isinstance(strings, list) or len(strings) != 4:
        errors.append("specimen template: must have four physical string placeholders")
    else:
        for index, item in enumerate(strings):
            if not isinstance(item, dict) or item.get("index") != index or item.get(
                    "status") != "unmeasured":
                errors.append(f"specimen template: string {index} must be unmeasured and indexed")
                continue
            for field in ("scale_length_m", "linear_density_kg_per_m", "tension_n",
                          "bending_stiffness_n_m2", "loss_model", "source_id", "uncertainty"):
                if item.get(field) is not None:
                    errors.append(f"specimen template: string {index} must not claim {field}")
    return errors


def validate_phase_crosswalk(root: Path, phase_doc: dict, tasks: dict, crosswalk: dict) -> dict:
    """Require every original phase task heading to have a real ENG work-order route."""
    errors: list[str] = []
    if crosswalk.get("schema_version") != 1 or not isinstance(crosswalk.get("coverage"), dict):
        return {"mapped_phase_tasks": 0, "errors": ["phase crosswalk: invalid schema or coverage"]}
    coverage = crosswalk["coverage"]
    discovered: set[str] = set()
    for phase in phase_doc.get("phases", []):
        doc_path = root / phase["document"]
        try:
            source = doc_path.read_text(encoding="utf-8")
        except OSError as exc:
            errors.append(f"phase crosswalk: cannot read {doc_path}: {exc}")
            continue
        headings = re.findall(r"^##\s+(P\d{2}-T\d{2})\b", source, flags=re.M)
        if not headings:
            errors.append(f"phase crosswalk: {phase['id']} has no task headings")
        for heading in headings:
            if not heading.startswith(phase["id"] + "-"):
                errors.append(f"phase crosswalk: wrong-phase heading {heading} in {phase['id']}")
            if heading in discovered:
                errors.append(f"phase crosswalk: duplicate legacy task heading {heading}")
            discovered.add(heading)
    task_ids = {t["id"] for t in tasks.get("tasks", []) if isinstance(t, dict) and "id" in t}
    used: set[str] = set()
    for heading in sorted(discovered):
        assigned = coverage.get(heading)
        if not isinstance(assigned, list) or not assigned or not all(isinstance(x, str) for x in assigned):
            errors.append(f"phase crosswalk: no engineering work order for {heading}")
            continue
        if len(assigned) != len(set(assigned)):
            errors.append(f"phase crosswalk: duplicate route for {heading}")
        for implementation in assigned:
            if implementation not in task_ids:
                errors.append(f"phase crosswalk: unknown engineering work order {implementation} for {heading}")
            used.add(implementation)
        if not any(x.startswith("ENG-" + heading[:3] + "-") for x in assigned):
            errors.append(f"phase crosswalk: {heading} has no work order in its own phase")
    for stale_heading in sorted(set(coverage) - discovered):
        errors.append(f"phase crosswalk: stale/nonexistent heading {stale_heading}")
    for orphan in sorted(task_ids - used):
        errors.append(f"phase crosswalk: engineering task {orphan} has no parent phase task")
    _record_errors(crosswalk, "phase crosswalk", errors)
    return {"mapped_phase_tasks": len(discovered), "errors": errors}


def _record_errors(record: object, at: str, errors: list[str]) -> None:
    if isinstance(record, dict):
        for key, value in record.items():
            if key in FORBIDDEN_KEYS:
                errors.append(f"{at}: calendar/estimate key is not allowed: {key}")
            _record_errors(value, f"{at}.{key}", errors)
    elif isinstance(record, list):
        for index, item in enumerate(record):
            _record_errors(item, f"{at}[{index}]", errors)


def _text(record: dict, key: str, at: str, errors: list[str]) -> None:
    value = record.get(key)
    if not isinstance(value, str) or not value.strip():
        errors.append(f"{at}: {key} must be nonempty text")


def _list(record: dict, key: str, at: str, errors: list[str], *, minimum: int = 1) -> list:
    value = record.get(key)
    if not isinstance(value, list) or len(value) < minimum:
        errors.append(f"{at}: {key} must have at least {minimum} items")
        return []
    if not all(isinstance(item, str) and item.strip() for item in value):
        errors.append(f"{at}: {key} must contain only nonempty strings")
        return []
    if len(value) != len(set(value)):
        errors.append(f"{at}: {key} contains duplicates")
    return value


def _valid_relative_path(value: str) -> bool:
    if "\\" in value or value.startswith("/") or ":" in value or "//" in value:
        return False
    # A trailing slash intentionally marks a proposed package directory.
    normalized = value[:-1] if value.endswith("/") else value
    p = PurePosixPath(normalized)
    return not p.is_absolute() and bool(p.parts) and all(
        part not in ("", ".", "..") for part in normalized.split("/"))


def audit(root: Path, phases: dict, requirements: dict, tasks: dict) -> dict:
    errors: list[str] = []
    for name, value in (("phases", phases), ("requirements", requirements), ("tasks", tasks)):
        if not isinstance(value, dict) or value.get("schema_version") != 1:
            errors.append(f"{name}: schema_version must be 1")
            return {"errors": errors, "requirements": 0, "tasks": 0, "by_phase": {}}
        _record_errors(value, name, errors)

    phase_ids = {p.get("id") for p in phases.get("phases", []) if isinstance(p, dict)}
    req_list, task_list = requirements.get("requirements"), tasks.get("tasks")
    if not isinstance(req_list, list) or not isinstance(task_list, list):
        errors.append("requirements and tasks must have lists")
        return {"errors": errors, "requirements": 0, "tasks": 0, "by_phase": {}}

    req_ids: set[str] = set()
    all_test_names: set[str] = set()
    for item in req_list:
        if not isinstance(item, dict):
            errors.append("requirements: every item must be an object")
            continue
        rid = item.get("id")
        at = f"requirement {rid}"
        if not isinstance(rid, str) or not REQ_PATTERN.fullmatch(rid):
            errors.append(f"{at}: malformed requirement identifier")
        elif rid in req_ids:
            errors.append(f"{at}: duplicate identifier")
        else:
            req_ids.add(rid)
        if item.get("phase") not in phase_ids:
            errors.append(f"{at}: unknown phase {item.get('phase')}")
        for field in ("title", "statement", "evidence"):
            _text(item, field, at, errors)
        if item.get("status") not in requirements.get("status_vocabulary", []):
            errors.append(f"{at}: unknown status {item.get('status')}")
        for test_name in _list(item, "test_cases", at, errors):
            if not TEST_PATTERN.fullmatch(test_name):
                errors.append(f"{at}: invalid test identifier {test_name}")
            if test_name in all_test_names:
                errors.append(f"{at}: duplicated test ownership {test_name}")
            all_test_names.add(test_name)

    task_ids: set[str] = set()
    by_phase: dict[str, int] = {p: 0 for p in phase_ids if isinstance(p, str)}
    req_coverage: set[str] = set()
    graph: dict[str, list[str]] = {}
    for item in task_list:
        if not isinstance(item, dict):
            errors.append("tasks: every item must be an object")
            continue
        tid = item.get("id")
        at = f"task {tid}"
        if not isinstance(tid, str) or not TASK_PATTERN.fullmatch(tid):
            errors.append(f"{at}: malformed task identifier")
            continue
        if tid in task_ids:
            errors.append(f"{at}: duplicate identifier")
        task_ids.add(tid)
        phase = item.get("phase")
        if phase not in phase_ids or phase != tid.split("-")[1]:
            errors.append(f"{at}: phase must be known and match ID")
        else:
            by_phase[phase] += 1
        for field in ("title", "deliver"):
            _text(item, field, at, errors)
        for field, minimum in (("steps", 3), ("verify", 2), ("read", 1), ("touch", 1)):
            values = _list(item, field, at, errors, minimum=minimum)
            if field in {"read", "touch"}:
                for path in values:
                    if not _valid_relative_path(path):
                        errors.append(f"{at}: non-repository or malformed {field} path {path}")
                    elif field == "read" and not (root / path).is_file():
                        errors.append(f"{at}: missing source reading {path}")
        references = _list(item, "requirements", at, errors)
        for rid in references:
            if rid not in req_ids:
                errors.append(f"{at}: unknown requirement {rid}")
            req_coverage.add(rid)
        graph[tid] = _list(item, "requires", at, errors, minimum=0)
        if tid in graph[tid]:
            errors.append(f"{at}: self-dependency")
        if item.get("status") not in tasks.get("status_vocabulary", []):
            errors.append(f"{at}: unknown status {item.get('status')}")

    for tid, deps in graph.items():
        for dependency in deps:
            if dependency not in task_ids:
                errors.append(f"task {tid}: unknown dependency {dependency}")
    visiting: set[str] = set()
    visited: set[str] = set()

    def visit(tid: str) -> None:
        if tid in visiting:
            errors.append(f"task dependency cycle through {tid}")
            return
        if tid in visited:
            return
        visiting.add(tid)
        for parent in graph.get(tid, []):
            if parent in graph:
                visit(parent)
        visiting.remove(tid)
        visited.add(tid)

    for tid in graph:
        visit(tid)
    for rid in sorted(req_ids - req_coverage):
        errors.append(f"requirement {rid}: no engineering task covers it")
    for phase in sorted(phase_ids - {p for p, count in by_phase.items() if count > 0}):
        errors.append(f"phase {phase}: no engineering task")
    return {"schema_version": 1, "purpose": "Planning consistency only; not code or physical validation",
            "requirements": len(req_list), "tasks": len(task_list), "by_phase": dict(sorted(by_phase.items())),
            "planned_test_cases": len(all_test_names), "errors": sorted(set(errors))}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--json", type=Path, help="Optional JSON output destination")
    args = parser.parse_args()
    try:
        phase_doc, requirement_doc, task_doc = load_registries(args.root)
        report = audit(args.root, phase_doc, requirement_doc, task_doc)
        specimen = json.loads((args.root / "data/specimen-template.json").read_text(encoding="utf-8"))
        crosswalk = json.loads((args.root / "roadmap/phase-task-crosswalk.json").read_text(encoding="utf-8"))
        mapping = validate_phase_crosswalk(args.root, phase_doc, task_doc, crosswalk)
        report["mapped_phase_tasks"] = mapping["mapped_phase_tasks"]
        report["errors"] = sorted(set(report["errors"] + validate_blank_specimen_template(specimen)
                                     + mapping["errors"]))
    except (OSError, json.JSONDecodeError) as exc:
        print(f"Failed to load implementation registers: {exc}", file=sys.stderr)
        return 1
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Engineering plan: {report['requirements']} requirements; {report['tasks']} work orders; "
          f"{report['planned_test_cases']} planned test cases; "
          f"{report['mapped_phase_tasks']} original phase tasks mapped")
    print("The audit checks traceability and file structure, not implementation or scientific correctness.")
    for error in report["errors"]:
        print(f"ERROR: {error}", file=sys.stderr)
    print(f"Engineering register errors: {len(report['errors'])}")
    return 1 if report["errors"] else 0


if __name__ == "__main__":
    raise SystemExit(main())
