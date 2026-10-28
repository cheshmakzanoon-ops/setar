#!/usr/bin/env python3
"""Check C++ executable sweep, provenance, generated plots and failure behavior."""

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

from generate_solver_report import produce, read_sweep


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--solver", type=Path, required=True)
    parser.add_argument("--generator", type=Path, required=True)
    args = parser.parse_args()
    result = subprocess.run([str(args.solver)], capture_output=True,
                            text=True, check=True, timeout=25)
    rows = list(csv.DictReader(io.StringIO(result.stdout)))
    assert len(rows) == 60, "exactly 5 nodes x 4 rates x 3 modes must be emitted"
    assert len({(row["grid_nodes"], row["rate_hz"], row["mode_number"])
                for row in rows}) == 60, "sweep coordinates must be unique"
    assert any(row["leapfrog_stable"] == "0" and row["leapfrog_hz"] == ""
               for row in rows), "unstable CFL runs must be explicitly absent"
    assert any(row["leapfrog_stable"] == "1" and float(row["leapfrog_hz"]) > 0
               for row in rows), "stable CFL runs must have an explicit frequency"
    for mode in (1, 3, 5):
        for rate in (8000.0, 48000.0, 96000.0, 192000.0):
            subset = sorted((int(row["grid_nodes"]), float(row["spatial_abs_error_hz"]))
                            for row in rows if int(row["mode_number"]) == mode
                            and float(row["rate_hz"]) == rate)
            assert len(subset) == 5
            assert all(subset[i+1][1] < subset[i][1] for i in range(4)), (
                "reported spatial errors must decrease on refinement"
            )
    for nodes in (8, 16, 32, 64, 128):
        subset = sorted((float(row["rate_hz"]), float(row["temporal_abs_error_hz"]))
                        for row in rows if int(row["grid_nodes"]) == nodes
                        and int(row["mode_number"]) == 5)
        assert len(subset) == 4
        assert all(subset[i+1][1] < subset[i][1] for i in range(3)), (
            "reported midpoint dispersion must decrease at higher sample rates"
        )

    with tempfile.TemporaryDirectory(prefix="setar-numerical-evidence-") as directory:
        root = Path(directory)
        sweep = root / "sweep.csv"
        sweep.write_text(result.stdout, encoding="utf-8")
        provenance = produce(sweep, root / "report", "synthetic-ci-revision")
        assert provenance["row_count"] == 60
        assert provenance["measured_instrument"] is False
        assert provenance["reference_specimen"] is None
        assert provenance["explicit_cfl_rejected_rows"] > 0
        for name in ("solver-comparison.csv", "solver-evidence.json", "spatial-convergence.svg",
                     "temporal-dispersion.svg", "explicit-dispersion.svg", "index.html"):
            artifact = root / "report" / name
            assert artifact.is_file() and artifact.stat().st_size > 200
        evidence = json.loads((root / "report" / "solver-evidence.json").read_text())
        assert evidence["source_revision"] == "synthetic-ci-revision"
        html_content = (root / "report" / "index.html").read_text(encoding="utf-8")
        assert "not a measured Iranian setar" in html_content
        assert "not" in html_content and "host screenshot" in html_content
        assert math.isfinite(float(rows[0]["continuum_hz"]))

        corrupted = result.stdout.replace(rows[0]["spatial_abs_error_hz"], "NaN", 1)
        sweep.write_text(corrupted, encoding="utf-8")
        try:
            read_sweep(sweep)
        except ValueError:
            pass
        else:
            raise AssertionError("nonfinite evidence must be rejected")

        # The stand-alone command path is tested too, not only the import path.
        sweep.write_text(result.stdout, encoding="utf-8")
        subprocess.run([sys.executable, str(args.generator), "--input", str(sweep),
                        "--output", str(root / "cli")],
                       capture_output=True, text=True, check=True, timeout=25)
        assert (root / "cli" / "index.html").is_file()
    print("PASS 60-row C++ sweep, numerical monotonicity, reports, plots and rejection paths")


if __name__ == "__main__":
    main()
