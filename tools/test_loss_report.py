#!/usr/bin/env python3
"""Check the C++ loss study, its provenance gates, generated plots and failure behavior."""

from __future__ import annotations

import argparse
import csv
import io
import json
import math
from pathlib import Path
import subprocess
import sys
import tempfile

from generate_loss_report import orders, produce, read_decay, read_waveform


def run_study(study: Path, table: str) -> str:
    # --max-nodes 63 keeps the Debug/sanitizer run short; CI evidence uses all four levels.
    result = subprocess.run([str(study), table, "--max-nodes", "63"], capture_output=True,
                            text=True, check=True, timeout=240)
    return result.stdout


def rewrite(text: str, edit) -> str:
    """Apply ``edit(rows)`` to parsed CSV rows and serialize with the same header."""
    reader = csv.DictReader(io.StringIO(text))
    header = reader.fieldnames
    rows = list(reader)
    edit(rows)
    buffer = io.StringIO()
    writer = csv.DictWriter(buffer, fieldnames=header, lineterminator="\n")
    writer.writeheader()
    writer.writerows(rows)
    return buffer.getvalue()


def expect_rejected(reader, path: Path, text: str, why: str, reason: str) -> None:
    """The input must fail with ``reason`` in the message, not with an unrelated error."""
    path.write_text(text, encoding="utf-8")
    try:
        reader(path)
    except ValueError as error:
        assert reason in str(error), f"{why}: rejected for the wrong reason: {error}"
        return
    raise AssertionError(f"must be rejected: {why}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--study", type=Path, required=True)
    parser.add_argument("--generator", type=Path, required=True)
    args = parser.parse_args()

    decay_text = run_study(args.study, "decay")
    waveform_text = run_study(args.study, "waveform")
    decay_rows = list(csv.DictReader(io.StringIO(decay_text)))
    waveform_rows = list(csv.DictReader(io.StringIO(waveform_text)))
    assert len(decay_rows) == 4 * 3 * 6, "4 laws x 3 levels x 6 modes must be emitted"
    assert len({(r["law"], r["grid_nodes"], r["mode_number"]) for r in decay_rows}) == 72
    assert len(waveform_rows) == 10 * 3, "ten comparisons at each of three levels"
    for row in decay_rows:
        assert float(row["measured_vs_oracle_decay_rel"]) < 1.0e-8, (
            "simulated decay must equal the amplification-root oracle")
        assert float(row["fit_residual"]) < 1.0e-10, "every projected mode is one damped pair"
        assert float(row["oracle_decay_per_s"]) <= float(row["grid_decay_per_s"]) * (1 + 1e-9), (
            "midpoint must not over-damp a resolved mode")
    # The loss-law oracle must differ from the declared continuum law: the study would
    # otherwise be echoing its own input.
    assert max(float(r["oracle_vs_continuum_decay_rel"]) for r in decay_rows) > 1.0e-2

    for law in ("constant", "wavenumber_squared", "frequency_squared", "combined"):
        for mode in (1, 3, 8):
            series = [(float(r["grid_nodes"]), float(r["oracle_vs_continuum_decay_rel"]))
                      for r in decay_rows if r["law"] == law and int(r["mode_number"]) == mode]
            finest = orders(series)[-1]
            assert 1.9 < finest < 2.1, f"{law} mode {mode}: decay error order {finest:.3f}"

    def wave_series(law, reference, stiffness, band):
        return [(float(r["grid_nodes"]), float(r["relative_linf_error"])) for r in waveform_rows
                if r["law"] == law and r["reference_law"] == reference
                and math.isclose(float(r["bending_stiffness_n_m2"]), stiffness)
                and int(r["band_modes"]) == band]

    for law in ("lossless", "constant", "wavenumber_squared", "frequency_squared", "combined"):
        assert orders(wave_series(law, "same", 0.000015, 3))[-1] > 1.8, (
            f"{law}: matched waveform error must converge at second order")
    for reference in ("zero_loss", "constant_only"):
        assert orders(wave_series("combined", reference, 0.000015, 3))[-1] < 0.5, (
            f"a wrong {reference} reference must leave an error floor, not converge")
    matched = wave_series("combined", "same", 0.000015, 3)[-1][1]
    for reference in ("zero_loss", "constant_only"):
        assert wave_series("combined", reference, 0.000015, 3)[-1][1] > 3.0 * matched, (
            "the comparison must reject a wrong loss law")

    with tempfile.TemporaryDirectory(prefix="setar-loss-evidence-") as directory:
        root = Path(directory)
        decay_path, waveform_path = root / "decay.csv", root / "waveform.csv"
        decay_path.write_text(decay_text, encoding="utf-8")
        waveform_path.write_text(waveform_text, encoding="utf-8")
        provenance = produce(decay_path, waveform_path, root / "report", "synthetic-ci-revision")
        assert provenance["measured_instrument"] is False
        assert provenance["reference_specimen"] is None
        assert provenance["evidence_class"] == "synthetic_numerical_comparison_only"
        assert provenance["refinement_levels_grid_nodes"] == [15, 31, 63]
        assert provenance["decay_rows"] == 72 and provenance["waveform_rows"] == 30
        assert provenance["worst_measured_vs_oracle_decay_rel"] < 1.0e-8
        for name in ("loss-decay.csv", "loss-waveform.csv", "loss-evidence.json",
                     "decay-by-mode.svg", "decay-convergence.svg", "waveform-convergence.svg",
                     "index.html"):
            artifact = root / "report" / name
            assert artifact.is_file() and artifact.stat().st_size > 200, name
        evidence = json.loads((root / "report" / "loss-evidence.json").read_text())
        assert evidence["source_revision"] == "synthetic-ci-revision"
        assert len(evidence["decay_csv_sha256"]) == 64
        assert "not a measured Iranian setar" in (root / "report" / "index.html").read_text(
            encoding="utf-8")
        for svg in ("decay-by-mode.svg", "decay-convergence.svg", "waveform-convergence.svg"):
            text = (root / "report" / svg).read_text(encoding="utf-8")
            assert text.startswith("<svg") and "NaN" not in text and "inf" not in text.lower()

        # --- rejection paths: bad evidence must not become a clean report --------------
        bad = root / "bad.csv"
        # Controls: the helper's own serialization must be accepted, so a rejection below
        # can only come from the deliberate defect.
        bad.write_text(rewrite(decay_text, lambda rows: None), encoding="utf-8")
        assert len(read_decay(bad)) == 72, "identity rewrite of the decay table must be accepted"
        bad.write_text(rewrite(waveform_text, lambda rows: None), encoding="utf-8")
        assert len(read_waveform(bad)) == 30, "identity rewrite of the waveform table must be accepted"

        expect_rejected(read_decay, bad,
                        decay_text.replace(decay_rows[0]["oracle_decay_per_s"], "NaN", 1),
                        "nonfinite decay value", "nonfinite value in oracle_decay_per_s")
        expect_rejected(read_decay, bad, decay_text.split("\n", 1)[1], "missing header",
                        "columns must be exactly")
        expect_rejected(read_decay, bad,
                        rewrite(decay_text, lambda rows: rows[0].update(
                            measured_decay_per_s=str(float(rows[0]["measured_decay_per_s"]) * 1.01))),
                        "measured value contradicting its derived relative-error column",
                        "measured_vs_oracle column contradicts its inputs")

        def tamper_consistently(rows):
            measured = float(rows[0]["measured_decay_per_s"]) * 1.01
            rows[0]["measured_decay_per_s"] = repr(measured)
            rows[0]["measured_vs_oracle_decay_rel"] = repr(
                abs(measured / float(rows[0]["oracle_decay_per_s"]) - 1.0))

        expect_rejected(read_decay, bad, rewrite(decay_text, tamper_consistently),
                        "simulation disagreeing with the oracle must fail the publication gate",
                        "simulation disagrees with the amplification oracle")
        expect_rejected(read_decay, bad,
                        rewrite(decay_text, lambda rows: rows[0].update(fit_residual="0.5")),
                        "a mode that is not a single damped pair must fail the fit gate",
                        "not a single damped pair")
        def overdamp_consistently(rows):
            # Oracle AND simulation both doctored to 1% above the grid decay, with every
            # derived column consistent, so only the ordering invariant can object.
            doctored = float(rows[0]["grid_decay_per_s"]) * 1.01
            rows[0]["oracle_decay_per_s"] = repr(doctored)
            rows[0]["measured_decay_per_s"] = repr(doctored)
            rows[0]["oracle_vs_continuum_decay_rel"] = repr(
                abs(doctored / float(rows[0]["continuum_decay_per_s"]) - 1.0))
            rows[0]["measured_vs_oracle_decay_rel"] = "0.0"

        expect_rejected(read_decay, bad, rewrite(decay_text, overdamp_consistently),
                        "an over-damped oracle must violate the ordering", "oracle <= grid <= continuum")
        expect_rejected(read_decay, bad,
                        rewrite(decay_text, lambda rows: rows[0].update(rate_hz="25000")),
                        "levels must refine h and dt jointly", "refine h and dt jointly")
        expect_rejected(read_decay, bad,
                        rewrite(decay_text, lambda rows: rows[0].update(law="invented")),
                        "unknown loss law", "unknown law")
        expect_rejected(read_decay, bad, decay_text + decay_text.split("\n", 2)[1] + "\n",
                        "duplicate coordinates", "duplicate coordinates")
        expect_rejected(read_waveform, bad,
                        rewrite(waveform_text, lambda rows: rows[0].update(reference_law="oracle")),
                        "unknown reference law", "unknown law or reference law")
        expect_rejected(read_waveform, bad, waveform_text + waveform_text.split("\n", 2)[1] + "\n",
                        "duplicate waveform coordinates", "duplicate coordinates")
        expect_rejected(read_waveform, bad,
                        rewrite(waveform_text, lambda rows: rows[0].update(relative_linf_error="-1")),
                        "nonpositive waveform error", "relative_linf_error must be positive")
        expect_rejected(read_waveform, bad,
                        rewrite(waveform_text, lambda rows: rows[0].update(band_modes="9999")),
                        "band wider than the grid can represent", "band cannot exceed")
        try:
            orders([(15.0, 0.1)])
        except ValueError:
            pass
        else:
            raise AssertionError("one error value cannot define an order")

        # The stand-alone command path is tested too, not only the import path.
        decay_path.write_text(decay_text, encoding="utf-8")
        subprocess.run([sys.executable, str(args.generator), "--decay", str(decay_path),
                        "--waveform", str(waveform_path), "--output", str(root / "cli")],
                       capture_output=True, text=True, check=True, timeout=60)
        assert (root / "cli" / "index.html").is_file()
    print("PASS loss study rows, second-order decay and waveform evidence, wrong-law rejection, "
          "reports, plots and failure paths")


if __name__ == "__main__":
    main()
