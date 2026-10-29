#!/usr/bin/env python3
"""Turn the C++ loss-law study into labelled, reviewable SYNTHETIC numerical evidence.

Inputs are the two CSV tables written by ``setar_loss_study`` (``decay`` and
``waveform``). The generator validates them strictly, recomputes derived columns and
convergence orders itself, refuses to publish a study whose simulation disagrees with
its amplification-root oracle, and writes SVG graphs, a provenance JSON and an HTML page.
It is evidence for the declared linear simply supported fixture only: no measured
setar, no fitted loss law, no acoustic or perceptual claim.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import html
import json
import math
from collections import defaultdict
from pathlib import Path

DECAY_COLUMNS = [
    "law", "decay_constant_per_s", "decay_wavenumber_squared_m2_per_s",
    "decay_frequency_squared_s", "grid_nodes", "rate_hz", "mode_number",
    "continuum_decay_per_s", "grid_decay_per_s", "oracle_decay_per_s",
    "measured_decay_per_s", "continuum_hz", "oracle_hz", "measured_hz",
    "oracle_vs_continuum_decay_rel", "measured_vs_oracle_decay_rel", "fit_residual",
    "regressor_condition",
]
WAVEFORM_COLUMNS = [
    "law", "reference_law", "bending_stiffness_n_m2", "band_modes", "grid_nodes",
    "rate_hz", "duration_s", "relative_linf_error",
]
DECAY_LAWS = {"constant", "wavenumber_squared", "frequency_squared", "combined"}
WAVEFORM_LAWS = DECAY_LAWS | {"lossless"}
REFERENCE_LAWS = {"same", "zero_loss", "constant_only"}

# Publication gates: a study that violates these is a failed experiment, not a report.
MAX_MEASURED_VS_ORACLE_DECAY_REL = 1.0e-6
MAX_FIT_RESIDUAL = 1.0e-8
# Bound on how far two doubles printed with 17 digits may differ when recomputed here.
RECOMPUTE_REL_TOL = 1.0e-9
RECOMPUTE_ABS_TOL = 1.0e-13

PALETTE = ["#22d3ee", "#fbbf24", "#a78bfa", "#34d399", "#f472b6", "#fb923c", "#60a5fa",
           "#f87171", "#a3e635", "#e2e8f0"]


def _read_table(path: Path, columns: list[str], text_columns: set[str]) -> list[dict]:
    with path.open(newline="", encoding="utf-8") as source:
        reader = csv.DictReader(source)
        if reader.fieldnames != columns:
            raise ValueError(f"{path.name}: columns must be exactly {columns}")
        rows = []
        for raw in reader:
            row: dict = {}
            for key, value in raw.items():
                if key in text_columns:
                    if value is None or not value:
                        raise ValueError(f"{path.name}: empty text field {key}")
                    row[key] = value
                    continue
                if value is None or value == "":
                    raise ValueError(f"{path.name}: missing numeric field {key}")
                number = float(value)
                if not math.isfinite(number):
                    raise ValueError(f"{path.name}: nonfinite value in {key}")
                row[key] = number
            rows.append(row)
    if not rows:
        raise ValueError(f"{path.name}: table cannot be empty")
    return rows


def _close(actual: float, expected: float) -> bool:
    return math.isclose(actual, expected, rel_tol=RECOMPUTE_REL_TOL, abs_tol=RECOMPUTE_ABS_TOL)


def read_decay(path: Path) -> list[dict]:
    rows = _read_table(path, DECAY_COLUMNS, {"law"})
    seen: set[tuple] = set()
    spacing_ratio: set[float] = set()
    for row in rows:
        if row["law"] not in DECAY_LAWS:
            raise ValueError(f"decay table: unknown law {row['law']}")
        key = (row["law"], row["grid_nodes"], row["rate_hz"], row["mode_number"])
        if key in seen:
            raise ValueError("decay table: duplicate coordinates")
        seen.add(key)
        for name in ("grid_nodes", "rate_hz", "mode_number", "continuum_decay_per_s",
                     "grid_decay_per_s", "oracle_decay_per_s", "measured_decay_per_s",
                     "continuum_hz", "oracle_hz", "measured_hz", "regressor_condition"):
            if row[name] <= 0:
                raise ValueError(f"decay table: {name} must be positive")
        for name in ("decay_constant_per_s", "decay_wavenumber_squared_m2_per_s",
                     "decay_frequency_squared_s", "fit_residual"):
            if row[name] < 0:
                raise ValueError(f"decay table: {name} must be nonnegative")
        if not _close(row["oracle_vs_continuum_decay_rel"],
                      abs(row["oracle_decay_per_s"] / row["continuum_decay_per_s"] - 1.0)):
            raise ValueError("decay table: oracle_vs_continuum column contradicts its inputs")
        if not _close(row["measured_vs_oracle_decay_rel"],
                      abs(row["measured_decay_per_s"] / row["oracle_decay_per_s"] - 1.0)):
            raise ValueError("decay table: measured_vs_oracle column contradicts its inputs")
        if row["measured_vs_oracle_decay_rel"] > MAX_MEASURED_VS_ORACLE_DECAY_REL:
            raise ValueError("decay table: simulation disagrees with the amplification oracle; "
                             "the study failed and no report is published")
        if row["fit_residual"] > MAX_FIT_RESIDUAL:
            raise ValueError("decay table: a mode is not a single damped pair (fit residual)")
        # Grid eigenvalues lie below the continuum ones and the midpoint rule contracts the
        # decay by 1/(1+(omega*dt/2)^2), so oracle <= grid <= continuum for every law.
        # A violation would mean the scheme over-damps or the oracle echoes something else.
        if row["oracle_decay_per_s"] > row["grid_decay_per_s"] * (1.0 + 1e-9) \
                or row["grid_decay_per_s"] > row["continuum_decay_per_s"] * (1.0 + 1e-9):
            raise ValueError("decay table: expected oracle <= grid <= continuum decay ordering")
        spacing_ratio.add(round(row["rate_hz"] / (row["grid_nodes"] + 1.0), 9))
    if len(spacing_ratio) != 1:
        raise ValueError("decay table: levels must refine h and dt jointly (rate/(N+1) constant)")
    return rows


def read_waveform(path: Path) -> list[dict]:
    rows = _read_table(path, WAVEFORM_COLUMNS, {"law", "reference_law"})
    seen: set[tuple] = set()
    for row in rows:
        if row["law"] not in WAVEFORM_LAWS or row["reference_law"] not in REFERENCE_LAWS:
            raise ValueError("waveform table: unknown law or reference law")
        key = (row["law"], row["reference_law"], row["bending_stiffness_n_m2"],
               row["band_modes"], row["grid_nodes"], row["rate_hz"])
        if key in seen:
            raise ValueError("waveform table: duplicate coordinates")
        seen.add(key)
        for name in ("bending_stiffness_n_m2", "band_modes", "grid_nodes", "rate_hz",
                     "duration_s", "relative_linf_error"):
            if row[name] <= 0:
                raise ValueError(f"waveform table: {name} must be positive")
        if row["band_modes"] > row["grid_nodes"]:
            raise ValueError("waveform table: band cannot exceed the representable modes")
    return rows


def orders(points: list[tuple[float, float]]) -> list[float]:
    """Observed order log2(e_i / e_{i+1}) for errors on successively halved steps."""
    ordered = sorted(points)
    if len(ordered) < 2 or any(error <= 0 for _, error in ordered):
        raise ValueError("convergence order needs two or more strictly positive errors")
    return [math.log2(ordered[i][1] / ordered[i + 1][1]) for i in range(len(ordered) - 1)]


def render_plot(title: str, subtitle: str, x_label: str, y_label: str, series: list[dict],
                output: Path, x_log: bool, x_ticks: list[float],
                guide: tuple[str, list[tuple[float, float]]] | None = None,
                y_log: bool = True) -> None:
    """Plot with log-x when requested and log-y (default) or linear y.

    Each series is {label, points, marker (default True), line (default True), dash}.
    """
    everything = [point for item in series for point in item["points"]]
    if guide:
        everything += guide[1]
    if len(everything) < 2 or (y_log and any(y <= 0 for _, y in everything)):
        raise ValueError("log plot requires strictly positive values")
    width, height = 1140, 640
    left, right, top, bottom = 108, 400, 98, 94
    xs = [x for x, _ in everything]
    x_min, x_max = min(xs), max(xs)
    if x_min == x_max:
        raise ValueError("plot needs a nonzero x range")

    def tx(x: float) -> float:
        return math.log10(x) if x_log else x

    def ty(y: float) -> float:
        return math.log10(y) if y_log else y

    y_values = [ty(y) for _, y in everything]
    if y_log:
        y_low, y_high = math.floor(min(y_values) - 0.1), math.ceil(max(y_values) + 0.1)
    else:
        # Outward rounding to a 1-2-5 grid keeps tick labels short and the data inside.
        span = (max(y_values) - min(y_values)) or 1.0
        raw = span / 5.0
        magnitude = 10.0 ** math.floor(math.log10(raw))
        linear_step = next(m * magnitude for m in (1.0, 2.0, 5.0, 10.0) if m * magnitude >= raw)
        y_low = math.floor(min(y_values) / linear_step) * linear_step
        y_high = math.ceil(max(y_values) / linear_step) * linear_step
    if y_high == y_low:
        y_high += 1

    def px(x: float) -> float:
        return left + (tx(x) - tx(x_min)) / (tx(x_max) - tx(x_min)) * (width - left - right)

    def py(y: float) -> float:
        return height - bottom - (ty(y) - y_low) / (y_high - y_low) * (height - top - bottom)

    elements = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
        f'viewBox="0 0 {width} {height}" role="img" aria-label="{html.escape(title)}">',
        '<rect width="100%" height="100%" fill="#0f172a"/>',
        f'<text x="{left}" y="40" font-size="24" fill="#f8fafc" font-family="sans-serif">'
        f'{html.escape(title)}</text>',
        f'<text x="{left}" y="66" font-size="13" fill="#94a3b8" font-family="sans-serif">'
        f'{html.escape(subtitle)}</text>',
    ]
    if y_log:
        y_ticks = [(10.0 ** power, f"1e{power}") for power in range(int(y_low), int(y_high) + 1)]
    else:
        count = int(round((y_high - y_low) / linear_step)) + 1
        y_ticks = [(y_low + linear_step * i, f"{y_low + linear_step * i:.3g}") for i in range(count)]
    for value, text in y_ticks:
        yy = height - bottom - ((ty(value) if y_log else value) - y_low) / (y_high - y_low) * (
            height - top - bottom)
        elements.append(f'<path d="M {left} {yy:.2f} H {width - right}" stroke="#334155"/>')
        elements.append(f'<text x="{left - 15}" y="{yy + 5:.2f}" font-size="13" fill="#cbd5e1" '
                        f'text-anchor="end" font-family="monospace">{text}</text>')
    for tick in x_ticks:
        xx = px(tick)
        elements.append(f'<path d="M {xx:.2f} {top} V {height - bottom}" stroke="#1e293b"/>')
        elements.append(f'<text x="{xx:.2f}" y="{height - bottom + 25}" font-size="13" '
                        f'text-anchor="middle" fill="#cbd5e1" font-family="monospace">{tick:g}</text>')
    if guide:
        label, points = guide
        string = " ".join(f"{px(x):.2f},{py(y):.2f}" for x, y in points)
        elements.append(f'<polyline points="{string}" fill="none" stroke="#64748b" '
                        'stroke-width="2" stroke-dasharray="3 6"/>')
    legend_y = top + 6
    for index, item in enumerate(series):
        colour = PALETTE[index % len(PALETTE)]
        dash = ' stroke-dasharray="9 5"' if item.get("dash") else ""
        points = sorted(item["points"])
        string = " ".join(f"{px(x):.2f},{py(y):.2f}" for x, y in points)
        if item.get("line", True):
            elements.append(f'<polyline points="{string}" fill="none" stroke="{colour}" '
                            f'stroke-width="2.5" stroke-linejoin="round"{dash}/>')
        if item.get("marker", True):
            for x, y in points:
                elements.append(f'<circle cx="{px(x):.2f}" cy="{py(y):.2f}" r="4.5" fill="{colour}" '
                                'stroke="#0f172a" stroke-width="1"/>')
        if item.get("line", True):
            elements.append(f'<path d="M {width - right + 16} {legend_y:.1f} h 26" stroke="{colour}" '
                            f'stroke-width="3"{dash}/>')
        else:
            elements.append(f'<circle cx="{width - right + 29}" cy="{legend_y:.1f}" r="4.5" '
                            f'fill="{colour}"/>')
        elements.append(f'<text x="{width - right + 52}" y="{legend_y + 4:.1f}" font-size="12" '
                        f'fill="#e2e8f0" font-family="sans-serif">{html.escape(item["label"])}</text>')
        legend_y += 20
    if guide:
        elements.append(f'<path d="M {width - right + 16} {legend_y:.1f} h 26" stroke="#64748b" '
                        'stroke-width="2" stroke-dasharray="3 6"/>')
        elements.append(f'<text x="{width - right + 52}" y="{legend_y + 4:.1f}" font-size="12" '
                        f'fill="#94a3b8" font-family="sans-serif">{html.escape(guide[0])}</text>')
    elements.append(f'<text x="{(width + left - right) / 2:.2f}" y="{height - 24}" '
                    f'text-anchor="middle" fill="#e2e8f0" font-size="15" '
                    f'font-family="sans-serif">{html.escape(x_label)}</text>')
    elements.append(f'<text transform="translate(26 {(top + height - bottom) / 2:.2f}) rotate(-90)" '
                    'text-anchor="middle" fill="#e2e8f0" font-size="15" '
                    f'font-family="sans-serif">{html.escape(y_label)}</text>')
    elements.append('</svg>')
    output.write_text("\n".join(elements) + "\n", encoding="utf-8")


def _series_by(rows: list[dict], key_fn, x_name: str, y_name: str) -> dict:
    grouped: dict = defaultdict(list)
    for row in rows:
        grouped[key_fn(row)].append((row[x_name], row[y_name]))
    return {key: sorted(points) for key, points in grouped.items()}


def produce(decay_csv: Path, waveform_csv: Path, out: Path, revision: str) -> dict:
    decay = read_decay(decay_csv)
    waveform = read_waveform(waveform_csv)
    out.mkdir(parents=True, exist_ok=True)
    decay_bytes, waveform_bytes = decay_csv.read_bytes(), waveform_csv.read_bytes()
    (out / "loss-decay.csv").write_bytes(decay_bytes)
    (out / "loss-waveform.csv").write_bytes(waveform_bytes)

    nodes = sorted({int(r["grid_nodes"]) for r in decay})
    coarse, fine = nodes[0], nodes[-1]
    if len(nodes) < 3:
        raise ValueError("at least three refinement levels are required for order evidence")

    # --- decay relative to the continuum law, by mode (combined law) --------------------
    def ratio(level: int, column: str) -> list[tuple[float, float]]:
        return sorted((r["mode_number"], r[column] / r["continuum_decay_per_s"]) for r in decay
                      if r["law"] == "combined" and int(r["grid_nodes"]) == level)

    modes = sorted({int(r["mode_number"]) for r in decay})
    render_plot(
        "Decay rate relative to the continuum law, by mode",
        "Combined loss law, synthetic stiff string. The simulated mode sits on the amplification-root "
        "oracle at every level",
        "Mode number", "decay rate / continuum decay rate",
        [{"label": f"oracle, {coarse} nodes", "points": ratio(coarse, "oracle_decay_per_s")},
         {"label": f"measured from simulation, {coarse} nodes",
          "points": ratio(coarse, "measured_decay_per_s"), "line": False},
         {"label": f"oracle, {fine} nodes", "points": ratio(fine, "oracle_decay_per_s")},
         {"label": f"measured from simulation, {fine} nodes",
          "points": ratio(fine, "measured_decay_per_s"), "line": False}],
        out / "decay-by-mode.svg", x_log=False, x_ticks=[float(m) for m in modes], y_log=False,
        guide=("continuum law (ratio 1)", [(float(modes[0]), 1.0), (float(modes[-1]), 1.0)]))

    # --- decay error convergence --------------------------------------------------
    decay_series = _series_by([r for r in decay if r["law"] == "combined"],
                              lambda r: int(r["mode_number"]), "grid_nodes",
                              "oracle_vs_continuum_decay_rel")
    guide_origin = decay_series[min(decay_series)][0]
    render_plot(
        "Decay-rate error against the continuum law",
        "Combined law; h and dt halved together; dashed guide has slope -2 (second order)",
        "Interior grid nodes (sample rate scales with N+1)",
        "|oracle / continuum - 1| (log10)",
        [{"label": f"mode {mode}", "points": points} for mode, points in sorted(decay_series.items())],
        out / "decay-convergence.svg", x_log=True, x_ticks=[float(n) for n in nodes],
        guide=("slope -2 guide",
               [(float(n), guide_origin[1] * (guide_origin[0] / n) ** 2) for n in nodes]))

    # --- waveform convergence -----------------------------------------------------
    def label(key: tuple) -> str:
        law, reference, stiffness, band = key
        tail = "" if reference == "same" else f" | ref {reference}"
        return f"{law} | EI {stiffness:g} | {int(band)} modes{tail}"

    wave_series = _series_by(
        waveform,
        lambda r: (r["law"], r["reference_law"], r["bending_stiffness_n_m2"], r["band_modes"]),
        "grid_nodes", "relative_linf_error")
    ordered_keys = sorted(wave_series, key=lambda k: (k[1] != "same", k[3], k[0], k[2]))
    first = wave_series[ordered_keys[0]][0]
    render_plot(
        "Waveform error against the analytic modal reference",
        "Pickup at 25% of the length; matched laws converge at second order, wrong references plateau",
        "Interior grid nodes (sample rate scales with N+1)",
        "max|u_FD - u_modal| / max|u_modal| (log10)",
        [{"label": label(k), "points": wave_series[k], "dash": k[1] != "same"} for k in ordered_keys],
        out / "waveform-convergence.svg", x_log=True, x_ticks=[float(n) for n in nodes],
        guide=("slope -2 guide", [(float(n), first[1] * (first[0] / n) ** 2) for n in nodes]))

    # --- numerical summary, recomputed here from the raw tables ---------------------
    decay_orders = {f"{law}/mode{int(mode)}": orders(points)
                    for (law, mode), points in _series_by(
                        decay, lambda r: (r["law"], r["mode_number"]), "grid_nodes",
                        "oracle_vs_continuum_decay_rel").items()}
    waveform_orders = {label(k): orders(v) for k, v in wave_series.items()}
    finest_decay = [o[-1] for o in decay_orders.values()]
    matched_resolved = {label(k): orders(v)[-1] for k, v in wave_series.items()
                        if k[1] == "same" and k[3] == 3.0}
    mismatched = {label(k): orders(v)[-1] for k, v in wave_series.items() if k[1] != "same"}
    provenance = {
        "evidence_class": "synthetic_numerical_comparison_only",
        "measured_instrument": False,
        "reference_specimen": None,
        "producer": "setar_loss_study C++20; formulas in include/setar/numerical_prototype.hpp "
                    "and tests/support/modal_crosscheck.hpp",
        "source_revision": revision or None,
        "decay_csv_sha256": hashlib.sha256(decay_bytes).hexdigest(),
        "waveform_csv_sha256": hashlib.sha256(waveform_bytes).hexdigest(),
        "decay_rows": len(decay),
        "waveform_rows": len(waveform),
        "refinement_levels_grid_nodes": nodes,
        "refinement_rule": "h and dt halved together; sample_rate/(N+1) is constant",
        "decay_observed_order_finest_pair": {"minimum": min(finest_decay), "maximum": max(finest_decay)},
        "worst_measured_vs_oracle_decay_rel": max(r["measured_vs_oracle_decay_rel"] for r in decay),
        "worst_single_pair_fit_residual": max(r["fit_residual"] for r in decay),
        "worst_regressor_condition": max(r["regressor_condition"] for r in decay),
        "waveform_matched_resolved_order_finest_pair": matched_resolved,
        "waveform_wrong_reference_order_finest_pair": mismatched,
        "decay_orders_by_series": decay_orders,
        "waveform_orders_by_series": waveform_orders,
        "publication_gates": {
            "max_measured_vs_oracle_decay_rel": MAX_MEASURED_VS_ORACLE_DECAY_REL,
            "max_single_pair_fit_residual": MAX_FIT_RESIDUAL,
        },
        "known_limitations": [
            "Linear simply supported uniform string, one polarization, uniform grid; the sine "
            "modes diagonalize the damping operator only because coefficients are uniform",
            "Loss magnitudes are arbitrary synthetic values, not fitted or measured string loss",
            "Convergence is shown for band-limited, resolved initial data; the 6-mode band is "
            "pre-asymptotic on the coarsest grids by construction",
            "Observed orders are for the joint (h, dt) refinement path used here",
            "Wavenumber-squared and frequency-squared laws are nearly collinear at setar-scale "
            "bending stiffness: decay data cannot separate them (see numerics.loss_decay_oracle)",
            "No bridge, body, nail or fret coupling; no measured partial decay of any instrument",
            "Dense O(N^2) prototype; no CPU timing or real-time claim",
        ],
        "graphs": ["decay-by-mode.svg", "decay-convergence.svg", "waveform-convergence.svg"],
    }
    (out / "loss-evidence.json").write_text(
        json.dumps(provenance, indent=2, sort_keys=True) + "\n", encoding="utf-8")

    def row_html(name: str, values: list[float]) -> str:
        cells = "".join(f"<td>{v:.2f}</td>" for v in values)
        return f"<tr><td>{html.escape(name)}</td>{cells}</tr>"

    pair_headers = "".join(f"<th>{nodes[i]}→{nodes[i + 1]}</th>" for i in range(len(nodes) - 1))
    wave_rows = "\n".join(row_html(name, value) for name, value in sorted(waveform_orders.items()))
    page = f'''<!doctype html><html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Setar loss-law evidence</title>
<style>body{{background:#0f172a;color:#e2e8f0;font:16px/1.6 system-ui,sans-serif;
max-width:1100px;margin:auto;padding:28px}}a{{color:#67e8f9}}img{{max-width:100%;height:auto}}
.warning{{padding:16px;border:1px solid #f59e0b;background:#451a03}}
table{{border-collapse:collapse}}td,th{{border-bottom:1px solid #475569;padding:6px 14px;text-align:left}}</style>
</head><body><h1>Passive loss in the linear string: synthetic verification evidence</h1>
<p class="warning">This is not a measured Iranian setar, a fitted loss law, an acoustic or perceptual
result, or a host screenshot. The loss magnitudes are arbitrary synthetic values.</p>
<p>Decay CSV SHA-256: <code>{provenance['decay_csv_sha256']}</code> ({len(decay)} rows).
Waveform CSV SHA-256: <code>{provenance['waveform_csv_sha256']}</code> ({len(waveform)} rows).</p>
<p>Worst simulated-versus-oracle decay disagreement:
<code>{provenance['worst_measured_vs_oracle_decay_rel']:.3e}</code> (relative). Observed decay-error
order at the finest refinement pair: {min(finest_decay):.2f} to {max(finest_decay):.2f}.</p>
<figure><img src="decay-by-mode.svg" alt="Decay rate by mode for the combined law">
<figcaption>The simulated modal decay equals the amplification-root oracle, and both depart from the
continuum law at high modes: the scheme's own algorithmic decay error, reported rather than hidden.</figcaption></figure>
<figure><img src="decay-convergence.svg" alt="Decay error versus refinement">
<figcaption>Second-order convergence of decay to the continuum law as h and dt are halved together.</figcaption></figure>
<figure><img src="waveform-convergence.svg" alt="Waveform error versus refinement">
<figcaption>With a matched analytic reference the waveform error falls at second order. A reference
with the wrong loss law leaves an error floor that refinement does not remove: a model error,
not a discretization error. Series with six modes show the resolution boundary. The matched
three-mode curves nearly coincide.</figcaption></figure>
<h2>Observed waveform convergence orders (successive refinement pairs)</h2>
<table><thead><tr><th>Series</th>{pair_headers}</tr></thead><tbody>{wave_rows}</tbody></table>
<p><a href="loss-decay.csv">Decay rows</a> · <a href="loss-waveform.csv">Waveform rows</a> ·
<a href="loss-evidence.json">Provenance and limitations</a></p></body></html>'''
    (out / "index.html").write_text(page, encoding="utf-8")
    return provenance


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--decay", type=Path, required=True)
    parser.add_argument("--waveform", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--source-revision", default="")
    args = parser.parse_args()
    print(json.dumps(produce(args.decay, args.waveform, args.output, args.source_revision), indent=2))


if __name__ == "__main__":
    main()
