#!/usr/bin/env python3
"""Build an offline, deterministic report from actual synthetic reference output.

Standard library only. No invented curves, browser requirement, network access,
or claim of measured instrument validation. All output graphics are SVG files.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import html
import json
import math
from pathlib import Path
import re
import shutil
import sys
import wave


MODEL = "simply_supported_linear_stiff_string_modal_reference"
LABEL = "synthetic_not_measured_setar"
INPUT_FILES = ("metadata.json", "modes.csv", "samples.csv", "render.wav")
OUTPUT_FILES = ("index.html", "waveform.svg", "energy.svg", "dispersion.svg",
                "manifest.json", "metadata.json", "render.wav")


def finite(value: object, name: str) -> float:
    if isinstance(value, bool):
        raise ValueError(f"{name} must be a finite number, not a boolean")
    result = float(value)
    if not math.isfinite(result):
        raise ValueError(f"{name} is non-finite")
    return result


def near(actual: float, expected: float, name: str, relative: float = 2e-11,
         absolute: float = 1e-18) -> None:
    if not math.isclose(actual, expected, rel_tol=relative, abs_tol=absolute):
        raise ValueError(f"{name} does not agree with the independent reference")


def reject_json_constant(value: str) -> None:
    raise ValueError(f"nonstandard JSON constant {value}")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def read_fixture(source: Path) -> dict:
    with (source / "metadata.json").open(encoding="utf-8") as stream:
        metadata = json.load(stream, parse_constant=reject_json_constant)
    if metadata.get("schema_version") != 1 or metadata.get("model") != MODEL:
        raise ValueError("unsupported metadata schema or model identity")
    if metadata.get("evidence_label") != LABEL or metadata.get("normalization") != "none_fixed_gain":
        raise ValueError("report requires the explicitly labeled, fixed-gain synthetic fixture")
    numeric = ("sample_rate_hz", "sample_count", "requested_duration_s", "last_sample_time_s",
               "mode_count", "length_m", "tension_n", "linear_density_kg_per_m",
               "bending_stiffness_n_m2", "decay_constant_per_s", "decay_frequency_squared_s",
               "pluck_height_m", "pluck_position_fraction", "pickup_position_fraction",
               "gain_per_m", "peak_displacement_m", "peak_pcm_linear", "initial_energy_j",
               "final_sample_energy_j")
    for key in numeric:
        if key not in metadata:
            raise ValueError(f"metadata missing {key}")
        finite(metadata[key], key)
    rate = metadata["sample_rate_hz"]
    count = metadata["sample_count"]
    mode_count = metadata["mode_count"]
    if not isinstance(rate, int) or not 8000 <= rate <= 384000:
        raise ValueError("unsupported integer sample rate")
    if not isinstance(count, int) or not 1 <= count <= 11520000:
        raise ValueError("unsupported sample count")
    if not isinstance(mode_count, int) or not 1 <= mode_count <= 128:
        raise ValueError("unsupported modal count")
    length = metadata["length_m"]
    tension = metadata["tension_n"]
    density = metadata["linear_density_kg_per_m"]
    stiffness = metadata["bending_stiffness_n_m2"]
    pluck_position = metadata["pluck_position_fraction"]
    if min(length, tension, density, metadata["gain_per_m"]) <= 0 or stiffness < 0:
        raise ValueError("invalid physical parameters")
    if not 0 < pluck_position < 1 or not 0 <= metadata["pickup_position_fraction"] <= 1:
        raise ValueError("invalid observation or pluck location")
    if metadata["decay_constant_per_s"] < 0 or metadata["decay_frequency_squared_s"] < 0:
        raise ValueError("negative damping")
    near(metadata["last_sample_time_s"], (count - 1) / rate, "last sample time")
    if math.floor(metadata["requested_duration_s"] * rate) != count:
        raise ValueError("duration and sample count disagree")

    modes = []
    max_frequency_error = 0.0
    with (source / "modes.csv").open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        expected_fields = ["mode", "frequency_hz", "damped_frequency_hz", "decay_per_s",
                           "modal_mass_kg", "initial_displacement_m"]
        if reader.fieldnames != expected_fields:
            raise ValueError("unexpected mode CSV columns")
        for index, row in enumerate(reader, 1):
            mode = {key: finite(value, f"mode {index} {key}") for key, value in row.items()}
            if mode["mode"] != index or index > mode_count:
                raise ValueError("mode indices are not contiguous")
            k = index * math.pi / length
            omega = math.sqrt(tension * k * k / density + stiffness * k ** 4 / density)
            expected_frequency = omega / (2 * math.pi)
            sigma = metadata["decay_constant_per_s"] + metadata["decay_frequency_squared_s"] * omega ** 2
            if not 0 <= sigma < omega or expected_frequency >= 0.45 * rate:
                raise ValueError("mode violates supported decay or observation domain")
            near(mode["frequency_hz"], expected_frequency, "modal frequency")
            near(mode["decay_per_s"], sigma, "modal damping")
            near(mode["damped_frequency_hz"], math.sqrt((omega - sigma) * (omega + sigma)) /
                 (2 * math.pi), "damped modal frequency")
            near(mode["modal_mass_kg"], density * length / 2, "modal mass")
            amplitude = (2 * metadata["pluck_height_m"] * math.sin(index * math.pi * pluck_position) /
                         (index ** 2 * math.pi ** 2 * pluck_position * (1 - pluck_position)))
            near(mode["initial_displacement_m"], amplitude, "modal excitation", absolute=2e-18)
            max_frequency_error = max(max_frequency_error,
                                      abs(mode["frequency_hz"] / expected_frequency - 1))
            modes.append(mode)
    if len(modes) != mode_count:
        raise ValueError("mode CSV length does not match metadata")

    stride = max(1, math.ceil(count / 1600))
    analytic_stride = max(1, count // 64)
    waveform, energy_curve = [], []
    peak = 0.0
    initial_energy = None
    previous_energy = None
    final_energy = 0.0
    max_positive_energy_step = 0.0
    max_analytic_error = 0.0
    seen = 0
    with (source / "samples.csv").open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        if reader.fieldnames != ["sample_index", "time_s", "displacement_m", "pcm_linear", "energy_j"]:
            raise ValueError("unexpected sample CSV columns")
        for index, row in enumerate(reader):
            if index >= count or int(row["sample_index"]) != index:
                raise ValueError("sample indices are not contiguous or exceed metadata count")
            time = finite(row["time_s"], "time")
            displacement = finite(row["displacement_m"], "displacement")
            pcm = finite(row["pcm_linear"], "PCM level")
            energy = finite(row["energy_j"], "energy")
            near(time, index / rate, "sample timestamp", absolute=1e-15)
            near(pcm, displacement * metadata["gain_per_m"], "fixed observation gain")
            if energy < 0 or abs(pcm) > 1:
                raise ValueError("negative physical energy or clipped PCM data")
            if initial_energy is None:
                initial_energy = energy
            if previous_energy is not None:
                increase = max(0.0, energy - previous_energy)
                max_positive_energy_step = max(max_positive_energy_step, increase)
                if increase > 1e-11 * max(initial_energy, 1e-300):
                    raise ValueError("passive energy increase exceeds floating-point allowance")
            previous_energy = energy
            final_energy = energy
            peak = max(peak, abs(displacement))
            if time <= 0.025:
                waveform.append((time * 1000, displacement * 1000))
            if index % stride == 0 or index == count - 1:
                energy_curve.append((time, energy * 1e6))
            if index % analytic_stride == 0 or index == count - 1:
                reference_value = 0.0
                for mode in modes:
                    omega = 2 * math.pi * mode["frequency_hz"]
                    sigma = mode["decay_per_s"]
                    wd = math.sqrt((omega - sigma) * (omega + sigma))
                    q = mode["initial_displacement_m"] * math.exp(-sigma * time) * (
                        math.cos(wd * time) + sigma / wd * math.sin(wd * time))
                    position = metadata["pickup_position_fraction"]
                    weight = 0.0 if position in (0, 1) else math.sin(mode["mode"] * math.pi * position)
                    reference_value += q * weight
                error = abs(displacement - reference_value)
                max_analytic_error = max(max_analytic_error, error)
                near(displacement, reference_value, "sampled analytic displacement",
                     relative=1e-7, absolute=5e-12)
            seen += 1
    if seen != count or initial_energy is None:
        raise ValueError("sample CSV length does not match metadata")
    near(peak, metadata["peak_displacement_m"], "peak displacement")
    near(peak * metadata["gain_per_m"], metadata["peak_pcm_linear"], "peak PCM level")
    near(initial_energy, metadata["initial_energy_j"], "initial energy")
    near(final_energy, metadata["final_sample_energy_j"], "final energy")
    energy_from_modes = sum(0.5 * mode["modal_mass_kg"] * (
        2 * math.pi * mode["frequency_hz"] * mode["initial_displacement_m"]) ** 2 for mode in modes)
    near(initial_energy, energy_from_modes, "initial energy from modal coordinates")
    with wave.open(str(source / "render.wav"), "rb") as audio:
        if (audio.getnchannels(), audio.getsampwidth(), audio.getframerate(), audio.getnframes(),
            audio.getcomptype()) != (1, 2, rate, count, "NONE"):
            raise ValueError("WAV encoding disagrees with metadata")
    ideal_fundamental = math.sqrt(tension / density) / (2 * length)
    dispersion = [(mode["mode"], 1200 * math.log2(mode["frequency_hz"] /
                   (mode["mode"] * ideal_fundamental))) for mode in modes]
    return {"metadata": metadata, "modes": modes, "waveform": waveform,
            "energy_curve": energy_curve, "dispersion": dispersion,
            "checks": {"maximum_frequency_relative_error": max_frequency_error,
                       "maximum_analytic_sample_error_m": max_analytic_error,
                       "maximum_positive_energy_step_j": max_positive_energy_step,
                       "sample_rows_checked": seen,
                       "energy_ratio_final_to_initial": final_energy / initial_energy
                       if initial_energy else None}}


def svg_plot(title: str, description: str, points: list[tuple[float, float]],
             x_label: str, y_label: str, color: str, include_zero: bool = True) -> str:
    width, height = 840, 360
    left, top, right, bottom = 82, 42, 28, 66
    plot_width, plot_height = width - left - right, height - top - bottom
    xs = [point[0] for point in points]
    ys = [point[1] for point in points]
    x_min, x_max = min(xs), max(xs)
    y_min, y_max = min(ys), max(ys)
    if include_zero:
        y_min, y_max = min(0.0, y_min), max(0.0, y_max)
    if x_min == x_max:
        x_max = x_min + 1.0
    if y_min == y_max:
        y_min, y_max = y_min - 0.5, y_max + 0.5
    else:
        padding = (y_max - y_min) * 0.07
        y_min, y_max = y_min - padding, y_max + padding

    def coordinate(x: float, y: float) -> tuple[float, float]:
        return (left + (x - x_min) / (x_max - x_min) * plot_width,
                top + (y_max - y) / (y_max - y_min) * plot_height)

    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width} {height}" '
             'role="img" aria-labelledby="title description">',
             f'<title id="title">{html.escape(title)}</title>',
             f'<desc id="description">{html.escape(description)}</desc>',
             f'<rect width="{width}" height="{height}" fill="#fffefa"/>',
             '<g font-family="system-ui,sans-serif" font-size="12" fill="#4a535b">']
    for step in range(5):
        x_value = x_min + (x_max - x_min) * step / 4
        y_value = y_min + (y_max - y_min) * step / 4
        x, _ = coordinate(x_value, y_min)
        _, y = coordinate(x_min, y_value)
        parts.extend([
            f'<path d="M {x:.3f} {top} V {top + plot_height}" stroke="#e2e6e4"/>',
            f'<path d="M {left} {y:.3f} H {left + plot_width}" stroke="#e2e6e4"/>',
            f'<text x="{x:.3f}" y="{top + plot_height + 25}" text-anchor="middle">{x_value:.4g}</text>',
            f'<text x="{left - 10}" y="{y + 4:.3f}" text-anchor="end">{y_value:.4g}</text>'])
    coords = " ".join(f"{x:.3f},{y:.3f}" for x, y in (coordinate(*p) for p in points))
    parts.extend([
        f'<polyline points="{coords}" fill="none" stroke="{color}" stroke-width="2" '
        'stroke-linejoin="round" stroke-linecap="round"/>',
        f'<text x="{left}" y="24" font-weight="650" fill="#14212b">{html.escape(title)}</text>',
        f'<text x="{left + plot_width / 2}" y="{height - 15}" text-anchor="middle">{html.escape(x_label)}</text>',
        f'<text transform="translate(19,{top + plot_height / 2}) rotate(-90)" '
        f'text-anchor="middle">{html.escape(y_label)}</text>',
        '</g></svg>'])
    return "\n".join(parts) + "\n"


def make_html(data: dict, source_revision: str) -> str:
    m, checks = data["metadata"], data["checks"]
    rows = [("String length", f'{m["length_m"]:.6g} m'),
            ("Static tension", f'{m["tension_n"]:.6g} N'),
            ("Linear density", f'{m["linear_density_kg_per_m"]:.6g} kg/m'),
            ("Bending stiffness EI", f'{m["bending_stiffness_n_m2"]:.6g} N m²'),
            ("Constant decay", f'{m["decay_constant_per_s"]:.6g} s⁻¹'),
            ("Frequency-squared decay", f'{m["decay_frequency_squared_s"]:.6g} s'),
            ("Initial triangle height", f'{m["pluck_height_m"] * 1000:.6g} mm'),
            ("Pluck / observation position", f'{m["pluck_position_fraction"]:.4g} L / '
             f'{m["pickup_position_fraction"]:.4g} L'),
            ("Fixed observation gain", f'{m["gain_per_m"]:.6g} m⁻¹')]
    parameters = "".join(f"<tr><th scope='row'>{html.escape(key)}</th><td>{html.escape(value)}</td></tr>"
                         for key, value in rows)
    modal_rows = "".join(f'<tr><td>{int(mode["mode"])}</td><td>{mode["frequency_hz"]:.5f}</td>'
                         f'<td>{mode["decay_per_s"]:.6g}</td>'
                         f'<td>{mode["initial_displacement_m"] * 1000:.6g}</td></tr>' for mode in data["modes"])
    ratio = checks["energy_ratio_final_to_initial"]
    remaining = f"{100 * ratio:.3f}%" if ratio is not None else "silent fixture"
    revision = html.escape(source_revision)
    return f'''<!doctype html>
<html lang="en"><head><meta charset="utf-8"/><meta name="viewport" content="width=device-width,initial-scale=1"/>
<meta name="color-scheme" content="light"/><title>Setar — synthetic reference laboratory</title>
<style>
:root{{--ink:#14212b;--muted:#53616a;--paper:#f5f4ed;--card:#fffefa;--line:#dce1dc;--accent:#8f3d38;--teal:#16766d}}
*{{box-sizing:border-box}}body{{margin:0;background:var(--paper);color:var(--ink);font:16px/1.55 system-ui,sans-serif}}
main{{max-width:1280px;margin:auto;padding:48px 32px}}header{{border-bottom:1px solid var(--line);padding-bottom:32px}}
.eyebrow{{font-size:12px;text-transform:uppercase;letter-spacing:.18em;font-weight:700;color:var(--accent)}}
h1{{font-size:clamp(30px,4vw,54px);line-height:1.08;letter-spacing:-.04em;margin:18px 0;max-width:950px}}
h2{{font-size:22px;letter-spacing:-.02em;margin:0 0 10px}}h3{{font-size:17px;margin:0 0 10px}}
p{{margin:10px 0}}.lead{{max-width:820px;color:var(--muted);font-size:18px}}.label{{display:inline-block;border:1px solid #bdc9bc;
border-radius:24px;padding:4px 10px;font-size:12px;margin-right:8px;background:#eef1e9}}
.warning{{border-left:4px solid var(--accent);padding:12px 18px;background:#f4eae2;font-size:14px;margin-top:22px}}
.metrics{{display:grid;grid-template-columns:repeat(4,1fr);gap:14px;margin:28px 0}}
.metric{{border:1px solid var(--line);border-radius:12px;background:var(--card);padding:18px}}
.metric strong{{display:block;font-size:28px;letter-spacing:-.04em}}.metric span{{font-size:12px;color:var(--muted)}}
.grid{{display:grid;grid-template-columns:1fr 1fr;gap:22px}}section{{background:var(--card);border:1px solid var(--line);
border-radius:14px;padding:23px;margin-bottom:22px;min-width:0}}.wide{{grid-column:1/-1}}figure{{margin:12px 0 0}}
img{{display:block;width:100%;height:auto}}figcaption,.small{{font-size:13px;color:var(--muted)}}
table{{width:100%;border-collapse:collapse;font-size:13px}}td,th{{padding:9px 7px;border-bottom:1px solid var(--line);text-align:left}}
th{{font-weight:550}}td{{font-variant-numeric:tabular-nums}}code{{font-size:12px;overflow-wrap:anywhere}}
a{{color:var(--teal)}}audio{{width:100%;margin:12px 0}}footer{{font-size:12px;color:var(--muted);padding-top:12px}}
.check{{display:flex;justify-content:space-between;gap:18px;padding:10px 0;border-bottom:1px solid var(--line);font-size:13px}}
@media(max-width:800px){{main{{padding:24px 16px}}.metrics{{grid-template-columns:1fr 1fr}}.grid{{grid-template-columns:1fr}}
.wide{{grid-column:auto}}section{{padding:18px}}h1{{font-size:36px}}}}
</style></head><body><main>
<header><div class="eyebrow">SETAR / REFERENCE LABORATORY / MODEL 0.1</div>
<h1>A small, inspectable physics baseline.</h1>
<p class="lead">Actual output from a C++20 finite-mode stiff-string reference: displacement, passive energy decay,
and dispersion. Every chart below is computed from this run’s exported data.</p>
<span class="label">Synthetic fixture</span><span class="label">Simply-supported string</span>
<span class="label">No recorded-note playback</span>
<div class="warning"><strong>Not a validated setar.</strong> These inputs are constructed examples, not instrument measurements.
There is no body, bridge feedback, nail contact, fret interaction, or acoustic radiation model here.</div></header>
<div class="metrics">
<div class="metric"><strong>{m["sample_rate_hz"]:,}</strong><span>sample rate · Hz</span></div>
<div class="metric"><strong>{m["mode_count"]}</strong><span>retained physical modes</span></div>
<div class="metric"><strong>{m["sample_count"]:,}</strong><span>samples checked</span></div>
<div class="metric"><strong>{m["peak_pcm_linear"]:.4f}</strong><span>peak fixed-gain PCM · no normalization</span></div>
</div><div class="grid">
<section class="wide"><h2>Displacement at the observation point</h2>
<p class="small">First {min(m["last_sample_time_s"], 0.025) * 1000:.3g} ms. This is transverse string displacement, not sound pressure.</p>
<figure><img src="waveform.svg" alt="Synthetic displacement waveform in millimeters versus milliseconds from actual renderer output"/>
<figcaption>Samples are taken at t = n / sample rate before each exact free-response state update.</figcaption></figure></section>
<section><h2>Passive modal energy</h2><figure><img src="energy.svg" alt="Total finite-mode mechanical energy in microjoules over time"/>
<figcaption>Energy remaining at the final exported sample: {remaining}. No external work after initialization.</figcaption></figure></section>
<section><h2>Stiffness-induced dispersion</h2><figure><img src="dispersion.svg" alt="Frequency excess in cents over the corresponding zero-stiffness harmonics"/>
<figcaption>Each point uses the exported modal frequency, compared with the ideal tension-only string at the same mode number.
This is not an FFT or a measured setar spectrum.</figcaption></figure></section>
<section><h2>Synthetic input manifest</h2><table><tbody>{parameters}</tbody></table>
<p class="small">Model: <code>{MODEL}</code><br/>Source revision supplied to the report: <code>{revision}</code></p></section>
<section><h2>Checks on this artifact</h2>
<div class="check"><span>Modal formula relative error</span><strong>{checks["maximum_frequency_relative_error"]:.3e}</strong></div>
<div class="check"><span>Maximum sampled analytic error</span><strong>{checks["maximum_analytic_sample_error_m"]:.3e} m</strong></div>
<div class="check"><span>Largest positive energy step</span><strong>{checks["maximum_positive_energy_step_j"]:.3e} J</strong></div>
<div class="check"><span>Finite, ordered CSV rows</span><strong>{checks["sample_rows_checked"]:,}</strong></div>
<p class="small">These checks inspect this run. They do not claim that platform CI, plugin-host compatibility, acoustic validation,
or perceptual testing has passed. Full tolerances and input hashes are in <a href="manifest.json">the report manifest</a>.</p>
<h3>Listen to the reference observation</h3><audio controls preload="none" src="render.wav">Your browser cannot play WAV audio.</audio>
<p class="small">Mono PCM16, scaled by an explicit fixed gain. <a href="render.wav">Download WAV</a> ·
<a href="metadata.json">Input/output metadata</a></p></section>
<section class="wide"><h2>Retained modal coordinates</h2><table><thead><tr><th>Mode</th><th>Natural frequency · Hz</th>
<th>Decay · s⁻¹</th><th>Initial modal displacement · mm</th></tr></thead><tbody>{modal_rows}</tbody></table></section>
</div><footer><strong>Interpretation boundary.</strong> The finite sine projection regularizes a sharp triangular displacement.
A triangle does not have finite continuum bending energy when EI is nonzero, so modal-count convergence of displacement is not
an energy-convergence claim. This report contains synthetic verification evidence, not instrument calibration, product validation,
performance benchmarks, or a completed physical setar.</footer></main></body></html>
'''


def generate(source: Path, destination: Path, source_revision: str) -> dict:
    if not re.fullmatch(r"[A-Za-z0-9_.+\-]{1,128}", source_revision):
        raise ValueError("source revision must be a short identifier without markup or whitespace")
    data = read_fixture(source)
    if source.resolve() == destination.resolve():
        raise ValueError("report output must differ from the input fixture directory")
    for name in OUTPUT_FILES:
        if (destination / name).exists():
            raise ValueError(f"refusing to overwrite existing report output {name}")
    destination.mkdir(parents=True, exist_ok=True)
    charts = {
        "waveform.svg": svg_plot("Synthetic displacement observation", "Actual samples from the reference renderer.",
                                 data["waveform"], "Time (ms)", "Displacement (mm)", "#16766d"),
        "energy.svg": svg_plot("Finite-mode energy", "Actual modal energy sampled during the render.",
                               data["energy_curve"], "Time (s)", "Energy (microjoules)", "#8f3d38"),
        "dispersion.svg": svg_plot("Deviation from tension-only harmonics", "Computed from exported natural frequencies.",
                                   data["dispersion"], "Mode number", "Frequency excess (cents)", "#66549c")}
    for name, content in charts.items():
        (destination / name).write_text(content, encoding="utf-8", newline="\n")
    (destination / "index.html").write_text(make_html(data, source_revision), encoding="utf-8", newline="\n")
    for name in ("render.wav", "metadata.json"):
        shutil.copyfile(source / name, destination / name)
    manifest = {
        "schema_version": 1,
        "evidence_label": LABEL,
        "model": MODEL,
        "source_revision": source_revision,
        "input_sha256": {name: sha256(source / name) for name in INPUT_FILES},
        "output_sha256": {name: sha256(destination / name) for name in OUTPUT_FILES if name != "manifest.json"},
        "checks": data["checks"],
        "tolerances": {"frequency_relative": 2e-11, "positive_energy_step_relative_to_initial": 1e-11,
                       "analytic_sample_absolute_m": 5e-12, "analytic_sample_relative": 1e-7},
        "chart_processing": {"waveform": "all exported samples through 25 ms",
                             "energy": "uniform deterministic subsampling at no more than about 1601 points",
                             "dispersion": "all retained modal frequencies versus independently computed tension-only harmonics"},
        "limitations": ["synthetic inputs, not measured setar data", "no acoustic sound pressure prediction",
                        "sharp triangular shape is finite-mode regularized, not continuum bending-energy converged",
                        "source revision is caller-supplied provenance, not an automatically authenticated build identity"]}
    (destination / "manifest.json").write_text(json.dumps(manifest, indent=2, sort_keys=True, allow_nan=False) + "\n",
                                               encoding="utf-8", newline="\n")
    return manifest


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True, help="Directory produced by setar_render")
    parser.add_argument("--output", type=Path, required=True, help="New report directory")
    parser.add_argument("--source-revision", default="unrecorded-working-tree")
    args = parser.parse_args()
    try:
        manifest = generate(args.input, args.output, args.source_revision)
    except (ValueError, OSError, KeyError, TypeError, OverflowError, csv.Error, wave.Error) as error:
        print(f"Report rejected: {error}", file=sys.stderr)
        return 2
    print(f"Synthetic reference report: {args.output / 'index.html'}")
    print(f"Verified {manifest['checks']['sample_rows_checked']} exported sample rows; no measured-data claim.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
