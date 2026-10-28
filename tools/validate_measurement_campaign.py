#!/usr/bin/env python3
"""Fail-closed acquisition-manifest lint plus checksums; NOT scientific validation."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re

EXPECTED_UNITS = {
    "geometry.external_scale": "m",
    "geometry.bowl_internal": "m",
    "geometry.wooden_soundboard": "m",
    "materials.orthotropic_compliance": "SI_tensor",
    "strings.length_tension_density": "SI_mixed",
    "strings.stiffness_losses": "SI_mixed",
    "frets.contact_geometry": "m",
    "bridge.multi_axis_mobility": "(m/s)/N",
    "bridge.string_support": "SI_mixed",
    "excitation.nail_flesh": "SI_mixed",
    "gestures.performer": "events_s",
    "cavity.acoustic_modes": "Pa",
    "radiation.directivity": "Pa",
    "observation.calibrated_mics": "Pa",
    "environment.conditions": "SI_mixed",
    "interactions.cross_string": "(m/s)/N",
    "inference.heldout_split": "record",
}

GLOBAL_IDS = (
    "specimen_id", "session_id", "operator_id", "owner_permission_evidence_id",
    "instrument_conditioning_log_id", "coordinate_frame_record_id",
    "sensor_chain_calibration_id", "validation_split_record_id",
)
CHANNEL_IDS = (
    "protocol_id", "sensor_calibration_id", "uncertainty_budget_id", "conditions_id",
    "permission_evidence_id", "reviewer_id",
)
SHA256 = re.compile(r"[0-9a-f]{64}\Z")


def demand(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def has_id(value: object) -> bool:
    return isinstance(value, str) and bool(value.strip()) and len(value) <= 240


def verified_file(path: Path, relative: object, declared_digest: object) -> None:
    demand(isinstance(relative, str) and relative != "" and not Path(relative).is_absolute(),
           "raw file needs nonempty relative path")
    demand(".." not in Path(relative).parts, "raw evidence cannot traverse to parent folders")
    demand(isinstance(declared_digest, str) and SHA256.fullmatch(declared_digest) is not None,
           "raw evidence requires lowercase SHA-256 hex")
    root = path.parent.resolve()
    actual_path = (path.parent / relative).resolve()
    demand(actual_path.is_relative_to(root) and actual_path.is_file(),
           "raw evidence missing or resolves outside the manifest folder")
    hasher = hashlib.sha256()
    with actual_path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            hasher.update(chunk)
    demand(hasher.hexdigest() == declared_digest,
           f"raw evidence checksum mismatch for {relative}")


def validate(path: Path, *, require_ready: bool = False) -> dict:
    document = json.loads(path.read_text(encoding="utf-8"))
    demand(type(document) is dict, "acquisition manifest must be JSON object")
    demand(document.get("schema_version") == 1, "unsupported acquisition schema")
    demand(document.get("instrument_family") == "iranian_setar",
           "instrument family must be Iranian setar, not tar or a membrane instrument")
    demand(document.get("evidence_class") in (
        "blank_acquisition_plan_not_measurements", "specimen_acquisition_manifest"),
        "manifest must explicitly distinguish blank template from acquisition")
    channels = document.get("channels")
    demand(isinstance(channels, list) and len(channels) == len(EXPECTED_UNITS),
           "all 17 critical acquisition channels are required")
    seen = set()
    acquired = ready = 0
    for channel in channels:
        demand(isinstance(channel, dict), "every channel must be an object")
        identifier = channel.get("id")
        demand(isinstance(identifier, str) and identifier in EXPECTED_UNITS and identifier not in seen,
               f"unknown or duplicate acquisition channel: {identifier}")
        seen.add(identifier)
        demand(channel.get("unit") == EXPECTED_UNITS[identifier],
               f"unexpected channel unit for {identifier}")
        demand(has_id(channel.get("quantity")), f"missing quantity name for {identifier}")
        status = channel.get("status")
        demand(status in ("missing", "acquired_unverified", "review_ready", "rejected"),
               f"unrecognized acquisition status for {identifier}")
        sources = channel.get("raw_sources")
        demand(isinstance(sources, list), f"raw_sources must be a list for {identifier}")
        if status == "missing":
            demand(not sources, f"missing channel cannot attach apparently measured files: {identifier}")
        elif status == "rejected":
            # Preserve repudiated provenance for forensic investigation.
            pass
        else:
            acquired += 1
            demand(document.get("evidence_class") == "specimen_acquisition_manifest",
                   f"cannot populate real measurements into blank template: {identifier}")
            demand(len(sources) > 0, f"acquired channel requires raw evidence: {identifier}")
            for source in sources:
                demand(isinstance(source, dict), "raw evidence entry must be an object")
                verified_file(path, source.get("relative_path"), source.get("sha256"))
            if status == "review_ready":
                ready += 1
                for field in CHANNEL_IDS:
                    demand(has_id(channel.get(field)),
                           f"review-ready {identifier} lacks {field}")
    demand(seen == set(EXPECTED_UNITS), "acquisition manifest is missing critical channel IDs")
    if acquired:
        for field in GLOBAL_IDS:
            demand(has_id(document.get(field)), f"acquired campaign lacks {field}")
    demand(document.get("physical_admissibility") in
           ("not_evaluated", "reviewed_pass", "reviewed_fail"),
           "physical admissibility must be an explicit reviewer status")
    demand(document.get("human_measurement_review") in
           ("not_performed", "signed_review_complete", "review_rejected"),
           "human review cannot be inferred from a checksum")
    if require_ready:
        demand(ready == len(EXPECTED_UNITS),
               f"acquisition gate needs all 17 channels review-ready; only {ready} are")
        demand(document["physical_admissibility"] == "reviewed_pass" and
               document["human_measurement_review"] == "signed_review_complete",
               "physical admissibility and qualified human review still required")
    return {
        "manifest": str(path),
        "critical_channels": len(EXPECTED_UNITS),
        "acquired": acquired,
        "review_ready": ready,
        "schema_and_referenced_file_hashes_pass": True,
        "scientific_truth_or_validity_proven_by_this_tool": False,
        "review_gate_satisfied": bool(require_ready and ready == len(EXPECTED_UNITS)),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest", type=Path)
    parser.add_argument("--require-ready", action="store_true")
    args = parser.parse_args()
    print(json.dumps(validate(args.manifest, require_ready=args.require_ready), indent=2))


if __name__ == "__main__":
    main()
