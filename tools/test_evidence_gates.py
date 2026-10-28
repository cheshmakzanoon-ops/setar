#!/usr/bin/env python3
"""Mutation tests proving that missing/fake measurements and qualification cannot pass."""

from __future__ import annotations

import copy
import hashlib
import json
from pathlib import Path
import tempfile

from validate_measurement_campaign import validate as validate_measurement
from validate_product_qualification import validate as validate_product

ROOT = Path(__file__).resolve().parents[1]


def rejected(function, label: str) -> None:
    try:
        function()
    except ValueError:
        return
    raise AssertionError(f"must reject: {label}")


def write_json(path: Path, data: dict) -> None:
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def main() -> None:
    measurement_template = ROOT / "data" / "measurement-campaign-template.json"
    product_template = ROOT / "data" / "product-qualification-template.json"
    result = validate_measurement(measurement_template)
    assert result["critical_channels"] == 17 and result["review_ready"] == 0
    assert not result["scientific_truth_or_validity_proven_by_this_tool"]
    rejected(lambda: validate_measurement(measurement_template, require_ready=True),
             "empty specimen template cannot pass acquisition gate")
    result = validate_product(product_template)
    assert result["required_gates"] == 13 and result["passed"] == 0
    assert result["not_executed"] == 13
    rejected(lambda: validate_product(product_template, require_qualified=True),
             "empty product template cannot pass qualification")

    with tempfile.TemporaryDirectory(prefix="setar-evidence-gates-") as root_text:
        root = Path(root_text)
        specimen = root / "campaign.json"
        qualification = root / "qualification.json"
        original_measurement = json.loads(measurement_template.read_text(encoding="utf-8"))
        original_product = json.loads(product_template.read_text(encoding="utf-8"))

        cases = 0
        mutation = copy.deepcopy(original_measurement)
        mutation["instrument_family"] = "iranian_tar"
        write_json(specimen, mutation)
        rejected(lambda: validate_measurement(specimen), "setar cannot be confused with tar")
        cases += 1
        mutation = copy.deepcopy(original_measurement)
        mutation["channels"][1]["id"] = mutation["channels"][0]["id"]
        write_json(specimen, mutation)
        rejected(lambda: validate_measurement(specimen), "duplicate required measurement")
        cases += 1
        mutation = copy.deepcopy(original_measurement)
        mutation["channels"][0]["unit"] = "cm"
        write_json(specimen, mutation)
        rejected(lambda: validate_measurement(specimen), "silent unit mismatch")
        cases += 1
        mutation = copy.deepcopy(original_measurement)
        mutation["channels"][0]["status"] = "review_ready"
        write_json(specimen, mutation)
        rejected(lambda: validate_measurement(specimen), "claim review-ready without raw evidence")
        cases += 1
        mutation = copy.deepcopy(original_measurement)
        mutation["evidence_class"] = "specimen_acquisition_manifest"
        for field in ("specimen_id", "session_id", "operator_id", "owner_permission_evidence_id",
                      "instrument_conditioning_log_id", "coordinate_frame_record_id",
                      "sensor_chain_calibration_id", "validation_split_record_id"):
            mutation[field] = "SYNTHETIC-MANIFEST-TEST-ONLY"
        raw = root / "test-only-raw.bin"
        raw.write_bytes(b"synthetic schema test bytes, not an instrument recording\n")
        channel = mutation["channels"][0]
        channel["status"] = "acquired_unverified"
        channel["raw_sources"] = [{"relative_path": raw.name,
                                   "sha256": hashlib.sha256(raw.read_bytes()).hexdigest()}]
        write_json(specimen, mutation)
        record = validate_measurement(specimen)
        assert record["acquired"] == 1 and record["review_ready"] == 0
        cases += 1
        raw.write_bytes(b"tampered after hash computation")
        rejected(lambda: validate_measurement(specimen), "tampered raw evidence")
        cases += 1
        raw.write_bytes(b"synthetic schema test bytes, not an instrument recording\n")
        mutation["channels"][0]["raw_sources"][0]["relative_path"] = "../not-allowed.bin"
        write_json(specimen, mutation)
        rejected(lambda: validate_measurement(specimen), "path traversal")
        cases += 1
        mutation["channels"][0]["raw_sources"][0]["relative_path"] = raw.name
        channel = mutation["channels"][0]
        channel["status"] = "review_ready"
        for field in ("protocol_id", "sensor_calibration_id", "uncertainty_budget_id",
                      "conditions_id", "permission_evidence_id", "reviewer_id"):
            channel[field] = "SYNTHETIC-MANIFEST-TEST-ONLY"
        write_json(specimen, mutation)
        assert validate_measurement(specimen)["review_ready"] == 1
        rejected(lambda: validate_measurement(specimen, require_ready=True),
                 "one channel is not full 17-channel campaign")
        cases += 1

        mutation = copy.deepcopy(original_product)
        mutation["required_matrix"][0]["id"] = mutation["required_matrix"][1]["id"]
        write_json(qualification, mutation)
        rejected(lambda: validate_product(qualification), "duplicate host matrix gate")
        cases += 1
        mutation = copy.deepcopy(original_product)
        mutation["executions"] = [{"id": "linux.validator", "status": "skipped", "evidence_files": []}]
        write_json(qualification, mutation)
        assert validate_product(qualification)["blocked_or_skipped"] == 1
        rejected(lambda: validate_product(qualification, require_qualified=True),
                 "skipped host validator is not a pass")
        cases += 1
        mutation = copy.deepcopy(original_product)
        mutation["executions"] = [{"id": "linux.validator", "status": "passed", "evidence_files": []}]
        write_json(qualification, mutation)
        rejected(lambda: validate_product(qualification), "invented validator pass")
        cases += 1
        mutation["evidence_class"] = "actual_product_qualification_record"
        mutation["revision"] = "SYNTHETIC-SCHEMA-TEST"
        mutation["product_identifier"] = "SYNTHETIC-NOT-A-PLUGIN"
        mutation["plugin_binary_sha256"] = "0" * 64
        validator_log = root / "synthetic-validator.txt"
        validator_log.write_text("not a real validator execution\n", encoding="utf-8")
        run = mutation["executions"][0]
        run.update({
            "evidence_files": [{"relative_path": validator_log.name,
                                "sha256": hashlib.sha256(validator_log.read_bytes()).hexdigest()}],
            "os_version": "synthetic", "architecture": "test_fixture", "tester_id": "synthetic",
            "reviewer_id": "synthetic", "runner_identity": "synthetic",
            "test_protocol_id": "synthetic", "validator_version": "synthetic",
            "plugin_absolute_path": "/synthetic/plugin.vst3", "exit_code": 1,
        })
        write_json(qualification, mutation)
        rejected(lambda: validate_product(qualification), "nonzero validator exit code")
        cases += 1
        run["exit_code"] = 0
        write_json(qualification, mutation)
        assert validate_product(qualification)["passed"] == 1
        rejected(lambda: validate_product(qualification, require_qualified=True),
                 "one synthetically structured run is not full qualification")
        cases += 1
        run["evidence_files"][0]["sha256"] = "1" * 64
        write_json(qualification, mutation)
        rejected(lambda: validate_product(qualification), "tampered validator log")
        cases += 1
        run["evidence_files"][0]["sha256"] = hashlib.sha256(validator_log.read_bytes()).hexdigest()

        realtime = copy.deepcopy(run)
        realtime["id"] = "linux.realtime"
        realtime.update({
            "hardware_profile_id": "synthetic", "callback_trace_id": "synthetic",
            "sample_rate_hz": 48000, "block_size": 64, "worst_callback_ms": 5.0,
            "p99_callback_ms": 3.0, "declared_budget_ms": 1.0,
        })
        mutation["executions"] = [realtime]
        write_json(qualification, mutation)
        rejected(lambda: validate_product(qualification), "callback past deadline")
        cases += 1
        mock = root / "mock.svg"
        mock.write_text("<svg>mockup, not a host capture</svg>", encoding="utf-8")
        ui = copy.deepcopy(realtime)
        ui["id"] = "linux.ui"
        ui.update({"host_name": "synthetic", "host_version": "synthetic",
                   "capture_session_id": "synthetic", "visual_review_id": "synthetic",
                   "capture_method": "live_host_screen_capture",
                   "screenshots": [{"relative_path": mock.name,
                                    "sha256": hashlib.sha256(mock.read_bytes()).hexdigest()}]})
        mutation["executions"] = [ui]
        write_json(qualification, mutation)
        rejected(lambda: validate_product(qualification), "mockup cannot satisfy PNG screen capture")
        cases += 1

    print(f"PASS {cases} evidence-gate mutation/positive checks; templates remain unqualified")


if __name__ == "__main__":
    main()
