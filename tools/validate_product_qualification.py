#!/usr/bin/env python3
"""Reject unsupported product-qualification claims; hashes are not scientific proof."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
from pathlib import Path
import re
import struct

SHA256 = re.compile(r"[0-9a-f]{64}\Z")
PLATFORMS = ("linux", "windows", "macos")
KIND_BY_ID = {"validator": "vst3_validator", "real_host": "real_host",
              "realtime": "realtime_profile", "ui": "live_ui_capture"}
REQUIRED = {f"{platform}.{short}": (platform, KIND_BY_ID[short])
            for platform in PLATFORMS for short in KIND_BY_ID}
REQUIRED["global.perceptual"] = ("cross_platform", "blind_perceptual_study")


def demand(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def nonblank(value: object) -> bool:
    return isinstance(value, str) and bool(value.strip()) and len(value) <= 240


def digest_valid(digest: object) -> bool:
    return isinstance(digest, str) and SHA256.fullmatch(digest) is not None


def check_file(manifest: Path, entry: object, *, png: bool = False) -> None:
    demand(isinstance(entry, dict), "evidence must be a file/digest object")
    relative = entry.get("relative_path")
    demand(nonblank(relative) and not Path(relative).is_absolute() and
           ".." not in Path(relative).parts, "evidence path must be within manifest folder")
    demand(digest_valid(entry.get("sha256")), "evidence requires lowercase SHA-256")
    base = manifest.parent.resolve()
    target = (manifest.parent / relative).resolve()
    demand(target.is_relative_to(base) and target.is_file(),
           "evidence file absent or outside the manifest folder")
    hasher = hashlib.sha256()
    with target.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            hasher.update(block)
    demand(hasher.hexdigest() == entry["sha256"], "evidence SHA-256 mismatch")
    if png:
        with target.open("rb") as handle:
            header = handle.read(24)
        demand(len(header) >= 24 and header[:8] == b"\x89PNG\r\n\x1a\n" and
               header[12:16] == b"IHDR", "UI screenshot must be a real PNG payload")
        width, height = struct.unpack(">II", header[16:24])
        demand(width >= 640 and height >= 360, "UI screenshot dimensions are too small")


def require_fields(execution: dict, fields: tuple[str, ...]) -> None:
    for field in fields:
        demand(nonblank(execution.get(field)), f"passed {execution['id']} lacks {field}")


def validate(manifest: Path, *, require_qualified: bool = False) -> dict:
    doc = json.loads(manifest.read_text(encoding="utf-8"))
    demand(isinstance(doc, dict) and doc.get("schema_version") == 1,
           "unsupported product qualification manifest")
    demand(doc.get("evidence_class") in
           ("blank_qualification_plan_not_executed", "actual_product_qualification_record"),
           "evidence_class must distinguish empty template from actual execution")
    entries = doc.get("required_matrix")
    demand(isinstance(entries, list) and len(entries) == len(REQUIRED),
           "qualification matrix must list all 13 required gate cells")
    seen = set()
    for item in entries:
        demand(isinstance(item, dict), "qualification matrix row must be an object")
        identifier = item.get("id")
        demand(isinstance(identifier, str) and identifier in REQUIRED and identifier not in seen,
               f"missing/unknown/duplicate qualification gate {identifier}")
        seen.add(identifier)
        demand((item.get("platform"), item.get("kind")) == REQUIRED[identifier],
               f"gate {identifier} platform or kind was mislabeled")
    executions = doc.get("executions")
    demand(isinstance(executions, list), "execution results must be an array")
    status_by_id: dict[str, str] = {}
    for run in executions:
        demand(isinstance(run, dict), "execution entry must be an object")
        identifier = run.get("id")
        demand(isinstance(identifier, str) and identifier in REQUIRED and
               identifier not in status_by_id, "execution ID unknown or duplicated")
        status = run.get("status")
        demand(status in ("passed", "failed", "blocked", "skipped"),
               "execution must state passed, failed, blocked or skipped")
        status_by_id[identifier] = status
        files = run.get("evidence_files")
        demand(isinstance(files, list), "execution must explicitly list evidence files")
        for entry in files:
            check_file(manifest, entry)
        if status != "passed":
            continue
        demand(doc["evidence_class"] == "actual_product_qualification_record",
               "blank template cannot contain a passed claim")
        demand(nonblank(doc.get("revision")) and nonblank(doc.get("product_identifier")) and
               digest_valid(doc.get("plugin_binary_sha256")),
               "passed gate needs identified source revision and built binary hash")
        require_fields(run, ("os_version", "architecture", "tester_id", "reviewer_id",
                             "runner_identity", "test_protocol_id"))
        demand(files, "a passed gate requires retained logs and/or evidence")
        kind = REQUIRED[identifier][1]
        if kind == "vst3_validator":
            require_fields(run, ("validator_version", "plugin_absolute_path"))
            demand(run.get("exit_code") == 0 and not isinstance(run.get("exit_code"), bool),
                   "Steinberg validator must actually exit zero")
        elif kind == "real_host":
            require_fields(run, ("host_name", "host_version", "host_test_case_id",
                                 "scan_log_id", "state_roundtrip_id"))
            demand(run.get("host_name", "").strip().lower() != "steinberg validator",
                   "validator is not independent actual DAW qualification")
        elif kind == "realtime_profile":
            require_fields(run, ("hardware_profile_id", "callback_trace_id"))
            for field in ("sample_rate_hz", "block_size", "worst_callback_ms",
                          "p99_callback_ms", "declared_budget_ms"):
                value = run.get(field)
                demand(isinstance(value, (int, float)) and not isinstance(value, bool) and
                       math.isfinite(value) and value > 0, f"invalid realtime {field}")
            demand(run["worst_callback_ms"] >= run["p99_callback_ms"],
                   "worst-case timing must be >= p99")
            demand(run["worst_callback_ms"] <= run["declared_budget_ms"],
                   "worst-case measured callback exceeds the declared deadline")
        elif kind == "live_ui_capture":
            require_fields(run, ("host_name", "host_version", "capture_session_id",
                                 "visual_review_id"))
            demand(run.get("capture_method") == "live_host_screen_capture",
                   "mockup or rendered synthetic SVG does not qualify as UI screenshot")
            shots = run.get("screenshots")
            demand(isinstance(shots, list) and shots,
                   "live UI gate requires at least one PNG screenshot")
            for shot in shots:
                check_file(manifest, shot, png=True)
        else:
            require_fields(run, ("preregistration_id", "consent_ledger_id",
                                 "blind_randomization_record_id", "analysis_script_hash_id",
                                 "sample_size_rationale_id", "study_coordinator_id"))
            demand(type(run.get("participant_count")) is int and run["participant_count"] > 0,
                   "blind study requires an actual positive participant count")

    passed = sum(value == "passed" for value in status_by_id.values())
    failed = sum(value == "failed" for value in status_by_id.values())
    blocked = sum(value in ("blocked", "skipped") for value in status_by_id.values())
    unrun = len(REQUIRED) - len(status_by_id)
    if require_qualified:
        demand(passed == len(REQUIRED),
               f"product qualification blocked: {passed}/{len(REQUIRED)} passed; "
               f"{failed} failed, {blocked} blocked or skipped, {unrun} not executed")
    return {
        "required_gates": len(REQUIRED), "passed": passed, "failed": failed,
        "blocked_or_skipped": blocked, "not_executed": unrun,
        "schema_and_referenced_hashes_checked": True,
        "independent_execution_or_perceptual_truth_proven_by_schema": False,
        "qualified_for_declared_matrix": bool(require_qualified and passed == len(REQUIRED)),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--require-qualified", action="store_true")
    args = parser.parse_args()
    print(json.dumps(validate(args.manifest, require_qualified=args.require_qualified), indent=2))


if __name__ == "__main__":
    main()
