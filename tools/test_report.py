#!/usr/bin/env python3
"""End-to-end, standard-library checks of the compiled renderer and actual report."""

from __future__ import annotations

import argparse
import csv
import hashlib
from html.parser import HTMLParser
import json
import math
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import wave
import xml.etree.ElementTree as ET


def run(command: list[str], expected: int = 0) -> subprocess.CompletedProcess:
    result = subprocess.run(command, capture_output=True, text=True, timeout=30, check=False)
    if result.returncode != expected:
        raise AssertionError(f"Expected exit {expected}, got {result.returncode}: {command}\n"
                             f"stdout: {result.stdout}\nstderr: {result.stderr}")
    return result


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


class LocalLinkCollector(HTMLParser):
    def __init__(self) -> None:
        super().__init__()
        self.links: list[str] = []

    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        for name, value in attrs:
            if name in ("src", "href") and value is not None:
                self.links.append(value)


def test_all(renderer: Path, report_script: Path, root: Path) -> None:
    base = root / "first"
    second = root / "second"
    command = [str(renderer), "--duration", "0.05", "--sample-rate", "48000"]
    run(command + ["--output-dir", str(base)])
    run(command + ["--output-dir", str(second)])
    names = ("render.wav", "samples.csv", "modes.csv", "metadata.json")
    for name in names:
        assert (base / name).read_bytes() == (second / name).read_bytes(), name
    manifest_before = {name: digest(base / name) for name in names}
    run(command + ["--output-dir", str(base)], expected=1)
    assert manifest_before == {name: digest(base / name) for name in names}
    metadata = json.loads((base / "metadata.json").read_text(encoding="utf-8"))
    assert metadata["evidence_label"] == "synthetic_not_measured_setar"
    assert metadata["sample_count"] == 2400
    with (base / "samples.csv").open(newline="", encoding="utf-8") as stream:
        samples = list(csv.DictReader(stream))
    assert len(samples) == 2400
    with wave.open(str(base / "render.wav"), "rb") as audio:
        assert audio.getnchannels() == 1
        assert audio.getsampwidth() == 2
        assert audio.getframerate() == 48000
        assert audio.getnframes() == 2400
        pcm = [value[0] for value in struct.iter_unpack("<h", audio.readframes(2400))]
    for index, (row, integer_sample) in enumerate(zip(samples, pcm, strict=True)):
        assert int(row["sample_index"]) == index
        assert math.isclose(float(row["time_s"]), index / 48000, abs_tol=1e-16)
        scaled = float(row["pcm_linear"]) * 32767
        expected_pcm = int(math.copysign(math.floor(abs(scaled) + 0.5), scaled))
        assert integer_sample == expected_pcm, "PCM quantization differs from exported physical sample"
    # Independent analytic observation directly from physical inputs, rather than
    # taking frequencies or initial modal amplitudes from production mode output.
    for index in (0, 1, 7, 31, 239, 511, 1023, 2399):
        time = index / 48000
        expected = 0.0
        for mode in range(1, metadata["mode_count"] + 1):
            k = mode * math.pi / metadata["length_m"]
            omega = math.sqrt((metadata["tension_n"] * k ** 2 +
                               metadata["bending_stiffness_n_m2"] * k ** 4) /
                              metadata["linear_density_kg_per_m"])
            sigma = metadata["decay_constant_per_s"] + metadata["decay_frequency_squared_s"] * omega ** 2
            wd = math.sqrt(omega ** 2 - sigma ** 2)
            position = metadata["pluck_position_fraction"]
            q0 = (2 * metadata["pluck_height_m"] * math.sin(mode * math.pi * position) /
                  (mode ** 2 * math.pi ** 2 * position * (1 - position)))
            q = q0 * math.exp(-sigma * time) * (math.cos(wd * time) + sigma * math.sin(wd * time) / wd)
            expected += q * math.sin(mode * math.pi * metadata["pickup_position_fraction"])
        assert math.isclose(float(samples[index]["displacement_m"]), expected,
                            rel_tol=2e-9, abs_tol=3e-14), "independent analytic observation"
    report_a, report_b = root / "report-a", root / "report-b"
    report_command = [sys.executable, str(report_script), "--input", str(base),
                      "--source-revision", "integration-test-fixture"]
    run(report_command + ["--output", str(report_a)])
    run(report_command + ["--output", str(report_b)])
    report_names = ("index.html", "waveform.svg", "energy.svg", "dispersion.svg",
                    "manifest.json", "metadata.json", "render.wav")
    for name in report_names:
        assert (report_a / name).read_bytes() == (report_b / name).read_bytes(), name
    report_manifest = json.loads((report_a / "manifest.json").read_text(encoding="utf-8"))
    assert report_manifest["input_sha256"] == manifest_before
    assert report_manifest["checks"]["sample_rows_checked"] == 2400
    for name in ("waveform.svg", "energy.svg", "dispersion.svg"):
        svg = ET.parse(report_a / name).getroot()
        assert svg.tag == "{http://www.w3.org/2000/svg}svg"
        assert svg.find("{http://www.w3.org/2000/svg}title") is not None
        assert svg.find(".//{http://www.w3.org/2000/svg}polyline") is not None
    parser = LocalLinkCollector()
    page = (report_a / "index.html").read_text(encoding="utf-8")
    parser.feed(page)
    assert "Not a validated setar" in page and "Synthetic fixture" in page
    for link in parser.links:
        assert "://" not in link, "report must have no external resource dependency"
        assert (report_a / link).is_file(), f"broken local report link: {link}"
    run(report_command + ["--output", str(report_a)], expected=2)
    run(report_command + ["--output", str(base)], expected=2)
    for index, arguments in enumerate((
            ["--sample-rate", "0"], ["--sample-rate", "48000x"], ["--sample-rate", "nan"],
            ["--duration", "nan"], ["--duration", "-1"], ["--duration", "31"],
            ["--duration", "0.0000001"], ["--modes", "0"], ["--modes", "129"],
            ["--height-m", "1"], ["--pluck-position", "0"], ["--pickup-position", "2"],
            ["--gain-per-m", "1000000000"], ["--decay-per-s", "-1"],
            ["--density-kg-m", "0"], ["--tension-n", "inf"], ["--unknown", "1"])):
        invalid_output = root / f"invalid-{index}"
        run([str(renderer), "--duration", "0.05", "--output-dir", str(invalid_output)] + arguments,
            expected=2)
        assert not invalid_output.exists(), "invalid requests must not create output"
    run([str(renderer), "--duration"], expected=2)
    assert "SYNTHETIC" in run([str(renderer), "--help"]).stdout
    # A corrupt numerical row must be rejected before any report is created.
    corrupt = root / "corrupt"
    shutil.copytree(base, corrupt)
    lines = (corrupt / "samples.csv").read_text(encoding="utf-8").splitlines()
    fields = lines[1].split(",")
    fields[2] = "nan"
    lines[1] = ",".join(fields)
    (corrupt / "samples.csv").write_text("\n".join(lines) + "\n", encoding="utf-8")
    rejected_output = root / "rejected-report"
    run([sys.executable, str(report_script), "--input", str(corrupt), "--output", str(rejected_output)],
        expected=2)
    assert not rejected_output.exists()
    # Silent physical fixtures and a single output sample exercise empty-range handling.
    silence = root / "silence"
    run([str(renderer), "--duration", "0.000021", "--height-m", "0", "--output-dir", str(silence)])
    run([sys.executable, str(report_script), "--input", str(silence), "--output", str(root / "silent-report")])
    silent_manifest = json.loads((root / "silent-report" / "manifest.json").read_text(encoding="utf-8"))
    assert silent_manifest["checks"]["energy_ratio_final_to_initial"] is None
    print("PASS renderer, independent analytic samples, PCM16, deterministic files, real SVG/HTML, rejection paths")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--renderer", type=Path, required=True)
    parser.add_argument("--report-script", type=Path, required=True)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="setar-reference-test-") as temporary:
        test_all(args.renderer.resolve(), args.report_script.resolve(), Path(temporary))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
