#!/usr/bin/env python3
"""Turn an actual C++ synthetic solver sweep into labelled, reviewable evidence."""

from __future__ import annotations

import argparse
import csv
import hashlib
import html
import json
import math
from pathlib import Path

REQUIRED = {
    "grid_nodes", "rate_hz", "mode_number", "continuum_hz", "spatial_hz",
    "midpoint_hz", "spatial_abs_error_hz", "temporal_abs_error_hz",
    "total_abs_error_hz", "dense_matrix_entries", "leapfrog_cfl_min_rate_hz",
    "leapfrog_stable", "leapfrog_hz", "leapfrog_abs_error_hz",
}


def read_sweep(path: Path) -> list[dict[str, float]]:
    with path.open(newline="", encoding="utf-8") as source:
        reader = csv.DictReader(source)
        if set(reader.fieldnames or []) != REQUIRED:
            raise ValueError("solver CSV must have exactly the documented fourteen columns")
        rows = [{key: float(value) if value not in (None, "") else None
                 for key, value in row.items()} for row in reader]
    if not rows:
        raise ValueError("solver CSV cannot be empty")
    for row in rows:
        if any(not math.isfinite(v) for v in row.values() if v is not None):
            raise ValueError("solver comparison has a nonfinite value")
        for key in ("grid_nodes", "rate_hz", "mode_number", "continuum_hz", "spatial_hz",
                    "midpoint_hz", "dense_matrix_entries", "leapfrog_cfl_min_rate_hz"):
            if row[key] is None or row[key] <= 0:
                raise ValueError(f"invalid positive solver column {key}")
        if int(row["grid_nodes"]) ** 2 != row["dense_matrix_entries"]:
            raise ValueError("dense storage-complexity proxy is inconsistent")
        for error, lhs, rhs in (
            ("spatial_abs_error_hz", "continuum_hz", "spatial_hz"),
            ("temporal_abs_error_hz", "spatial_hz", "midpoint_hz"),
            ("total_abs_error_hz", "continuum_hz", "midpoint_hz"),
        ):
            if not math.isclose(row[error], abs(row[lhs] - row[rhs]), abs_tol=1e-10, rel_tol=1e-10):
                raise ValueError(f"error column {error} contradicts measured sweep values")
        is_stable = row["rate_hz"] > 1.000000000001 * row["leapfrog_cfl_min_rate_hz"]
        if row["leapfrog_stable"] != float(is_stable):
            raise ValueError("explicit CFL status disagrees with grid maximum frequency")
        if is_stable:
            if row["leapfrog_hz"] is None or row["leapfrog_abs_error_hz"] is None:
                raise ValueError("stable explicit row lacks a frequency comparison")
            if not math.isclose(row["leapfrog_abs_error_hz"],
                                abs(row["leapfrog_hz"] - row["spatial_hz"]),
                                abs_tol=1e-10, rel_tol=1e-10):
                raise ValueError("explicit frequency error is inconsistent")
            if row["leapfrog_hz"] < row["spatial_hz"]:
                raise ValueError("stable leapfrog frequencies cannot warp downward")
        elif row["leapfrog_hz"] is not None or row["leapfrog_abs_error_hz"] is not None:
            raise ValueError("unstable explicit grid cannot report valid frequency values")
    return rows


def render_plot(title: str, subtitle: str, x_label: str, y_label: str,
                points: list[tuple[float, float]], x_ticks: list[float],
                output: Path) -> None:
    if len(points) < 2 or any(y <= 0 for _, y in points):
        raise ValueError("log-error plot requires at least two strictly positive values")
    width, height = 900, 540
    left, right, top, bottom = 106, 48, 98, 94
    x_min, x_max = min(x for x, _ in points), max(x for x, _ in points)
    values = [math.log10(y) for _, y in points]
    y_min = math.floor(min(values) - 0.2)
    y_max = math.ceil(max(values) + 0.2)
    if y_max == y_min:
        y_max += 1

    def px(x: float) -> float:
        return left + (x - x_min) / (x_max - x_min) * (width - left - right)

    def py(y: float) -> float:
        return height - bottom - (math.log10(y) - y_min) / (y_max - y_min) * (height - top - bottom)

    elements = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
        f'viewBox="0 0 {width} {height}" role="img" aria-label="{html.escape(title)}">',
        '<rect width="100%" height="100%" fill="#0f172a"/>',
        f'<text x="{left}" y="40" font-size="24" fill="#f8fafc" font-family="sans-serif">'
        f'{html.escape(title)}</text>',
        f'<text x="{left}" y="66" font-size="13" fill="#94a3b8" font-family="sans-serif">'
        f'{html.escape(subtitle)}</text>',
    ]
    for power in range(y_min, y_max + 1):
        yy = py(10.0 ** power)
        elements.append(f'<path d="M {left} {yy:.2f} H {width-right}" stroke="#334155"/>')
        elements.append(f'<text x="{left-15}" y="{yy+5:.2f}" font-size="13" fill="#cbd5e1" '
                        f'text-anchor="end" font-family="monospace">1e{power}</text>')
    for tick in x_ticks:
        xx = px(tick)
        elements.append(f'<path d="M {xx:.2f} {top} V {height-bottom}" '
                        'stroke="#1e293b"/>')
        elements.append(f'<text x="{xx:.2f}" y="{height-bottom+25}" font-size="13" '
                        f'text-anchor="middle" fill="#cbd5e1" font-family="monospace">{tick:g}</text>')
    points_string = " ".join(f"{px(x):.2f},{py(y):.2f}" for x, y in points)
    elements.append(f'<polyline points="{points_string}" fill="none" stroke="#22d3ee" '
                    'stroke-width="3" stroke-linejoin="round"/>')
    for x, y in points:
        elements.append(f'<circle cx="{px(x):.2f}" cy="{py(y):.2f}" r="5" fill="#fbbf24"/>')
    elements.append(f'<text x="{(width+left-right)/2:.2f}" y="{height-24}" '
                    f'text-anchor="middle" fill="#e2e8f0" font-size="15" '
                    f'font-family="sans-serif">{html.escape(x_label)}</text>')
    elements.append(f'<text transform="translate(26 {(top+height-bottom)/2:.2f}) rotate(-90)" '
                    'text-anchor="middle" fill="#e2e8f0" font-size="15" '
                    f'font-family="sans-serif">{html.escape(y_label)}</text>')
    elements.append('</svg>')
    output.write_text("\n".join(elements) + "\n", encoding="utf-8")


def produce(source: Path, out: Path, revision: str) -> dict:
    rows = read_sweep(source)
    out.mkdir(parents=True, exist_ok=True)
    original = source.read_bytes()
    (out / "solver-comparison.csv").write_bytes(original)
    spatial = sorted(((row["grid_nodes"], row["spatial_abs_error_hz"])
                      for row in rows if row["mode_number"] == 3 and row["rate_hz"] == 48000))
    temporal = sorted(((row["rate_hz"], row["temporal_abs_error_hz"])
                       for row in rows if row["mode_number"] == 5 and row["grid_nodes"] == 64))
    explicit = sorted(((row["rate_hz"], row["leapfrog_abs_error_hz"])
                       for row in rows if row["mode_number"] == 5 and row["grid_nodes"] == 64
                       and row["leapfrog_stable"] == 1.0))
    render_plot("Spatial grid convergence", "Uniform synthetic stiff string; mode 3; simply supported",
                "Interior spatial nodes", "Absolute frequency error (Hz, log10)", spatial,
                [x for x, _ in spatial], out / "spatial-convergence.svg")
    render_plot("Midpoint temporal dispersion", "Uniform synthetic stiff string; 64 nodes; mode 5",
                "Sample rate (Hz)", "Absolute frequency error (Hz, log10)", temporal,
                [x for x, _ in temporal], out / "temporal-dispersion.svg")
    render_plot("Explicit leapfrog temporal dispersion", "64 nodes; mode 5; unstable CFL rows omitted",
                "Sample rate (Hz)", "Absolute frequency error (Hz, log10)", explicit,
                [x for x, _ in explicit], out / "explicit-dispersion.svg")
    provenance = {
        "evidence_class": "synthetic_numerical_comparison_only",
        "measured_instrument": False,
        "reference_specimen": None,
        "producer": "setar_compare_solvers C++20; formulas declared in include/setar/numerical_prototype.hpp",
        "source_revision": revision or None,
        "sweep_sha256": hashlib.sha256(original).hexdigest(),
        "row_count": len(rows),
        "grid_nodes": sorted({int(r["grid_nodes"]) for r in rows}),
        "sample_rates_hz": sorted({int(r["rate_hz"]) for r in rows}),
        "mode_numbers": sorted({int(r["mode_number"]) for r in rows}),
        "explicit_cfl_rejected_rows": sum(r["leapfrog_stable"] == 0.0 for r in rows),
        "known_limitations": [
            "Linear simply supported uniform transverse string; one polarization",
            "No measured geometry, wooden body, bridge, nail or fret contact",
            "Dense matrix entries show memory growth; no CPU timing measured",
            "Explicit CFL rejections are real unsupported configurations, not zero error",
            "Analytical frequency calculations are not plugin sound-quality results",
            "No nonlinear, multirate, FEM, host, or calibrated microphone qualification",
        ],
        "graphs": ["spatial-convergence.svg", "temporal-dispersion.svg",
                   "explicit-dispersion.svg"],
    }
    (out / "solver-evidence.json").write_text(
        json.dumps(provenance, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    table_rows = "\n".join(
        f'<tr><td>{int(x):d}</td><td>{y:.9g}</td></tr>' for x, y in spatial
    )
    page = f'''<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Setar solver evidence</title>
<style>body{{background:#0f172a;color:#e2e8f0;font:16px/1.6 system-ui,sans-serif;
max-width:1050px;margin:auto;padding:28px}}a{{color:#67e8f9}}img{{max-width:100%;height:auto}}
.warning{{padding:16px;border:1px solid #f59e0b;background:#451a03}}
table{{border-collapse:collapse}}td,th{{border-bottom:1px solid #475569;padding:6px 18px}}</style>
</head><body><h1>Experimental solver comparison: synthetic reference only</h1>
<p class="warning">This is not a measured Iranian setar, an accepted production solver,
or a host screenshot. No listening test, FEM result or runtime benchmark is represented.</p>
<p>CSV SHA-256: <code>{provenance['sweep_sha256']}</code>. Sweep rows: {len(rows)}.</p>
<figure><img src="spatial-convergence.svg" alt="Mode-three spatial error versus grid density">
<figcaption>Fixed-end discretization converges toward the analytic continuum eigenfrequency.</figcaption></figure>
<figure><img src="temporal-dispersion.svg" alt="Midpoint temporal error versus sample rate">
<figcaption>The temporally integrated frequency approaches the spatial semidiscrete frequency.</figcaption></figure>
<figure><img src="explicit-dispersion.svg" alt="Explicit temporal error for CFL-stable rates">
<figcaption>The explicit scheme has a different frequency bias. Rejected CFL rows are omitted,
not plotted as zero error. Rejected rows: {provenance['explicit_cfl_rejected_rows']}.</figcaption></figure>
<h2>Spatial-error data (mode 3, 48 kHz)</h2><table><thead><tr>
<th>Nodes</th><th>Absolute error (Hz)</th></tr></thead><tbody>{table_rows}</tbody></table>
<p><a href="solver-comparison.csv">Download all comparison rows</a> ·
<a href="solver-evidence.json">Provenance and limitations</a></p></body></html>'''
    (out / "index.html").write_text(page, encoding="utf-8")
    return provenance


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--source-revision", default="")
    args = parser.parse_args()
    print(json.dumps(produce(args.input, args.output, args.source_revision), indent=2))


if __name__ == "__main__":
    main()
