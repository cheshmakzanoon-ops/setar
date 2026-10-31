#!/usr/bin/env python3
"""Analyze one ``setar_play`` render: graphs, pitch estimates and an offline HTML page.

Standard library only. Inputs are the files written by ``setar_play`` (render.wav,
diagnostics.csv, summary.json). Outputs are SVG graphs, a PNG spectrogram, an
analysis JSON and index.html. Everything describes a SYNTHETIC render of an
unvalidated physical-model prototype: no measured setar, no recording, no fidelity,
perceptual or realtime claim. Display scaling (dB relative to the loudest
spectrogram cell) is a plotting choice; the audio itself is never normalized.
"""

from __future__ import annotations

import argparse
import base64
import cmath
import csv
import hashlib
import html
import json
import math
from pathlib import Path
import shutil
import struct
import sys
import zlib

TOOL = "setar_play"
LABEL = "synthetic_unvalidated_prototype_render"
INPUT_FILES = ("render.wav", "diagnostics.csv", "summary.json")
OUTPUT_FILES = ("index.html", "waveform.svg", "spectrogram.png", "spectrogram.svg",
                "energy.svg", "pitch.svg", "analysis.json", "render.wav")
DISCLAIMER = ("SYNTHETIC render of an unvalidated physical-model prototype with synthetic "
              "parameters. Not a measured or validated setar, not a recording, and not a "
              "fidelity, perceptual or realtime claim.")
PALETTE = ["#22d3ee", "#fbbf24", "#a78bfa", "#34d399", "#f472b6", "#fb923c", "#60a5fa",
           "#f87171"]
# Viridis control points (matplotlib's CC0 colormap), interpolated linearly.
VIRIDIS = [(68, 1, 84), (72, 36, 117), (65, 68, 135), (53, 95, 141), (42, 120, 142),
           (33, 145, 140), (34, 168, 132), (68, 191, 112), (122, 209, 81), (189, 223, 38),
           (253, 231, 37)]
SEARCH_CENTS = 80.0          # pitch search half-width around the nominal fundamental
MAX_PITCH_STROKES = 48       # bound the pure-Python FFT work per report
MAX_SPECTROGRAM_COLUMNS = 360
STROKE_OFFSET_S = 0.03       # skip the contact transient before estimating pitch


def reject_json_constant(value: str) -> None:
    raise ValueError(f"nonstandard JSON constant {value}")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def finite(value: object, name: str) -> float:
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise ValueError(f"{name} must be a number")
    result = float(value)
    if not math.isfinite(result):
        raise ValueError(f"{name} is non-finite")
    return result


# ---- inputs ---------------------------------------------------------------------------

def read_wav(path: Path) -> tuple[int, list[float], list[float], str]:
    """Parse a stereo float32 (format 3) or 24-bit PCM (format 1) RIFF/WAVE file."""
    data = path.read_bytes()
    if len(data) < 12 or data[0:4] != b"RIFF" or data[8:12] != b"WAVE":
        raise ValueError("render.wav is not RIFF/WAVE")
    if struct.unpack_from("<I", data, 4)[0] + 8 != len(data):
        raise ValueError("render.wav RIFF size does not match the file size")
    at, fmt, payload = 12, None, None
    while at + 8 <= len(data):
        chunk_id, size = data[at:at + 4], struct.unpack_from("<I", data, at + 4)[0]
        body = data[at + 8:at + 8 + size]
        if len(body) != size:
            raise ValueError("truncated WAV chunk")
        if chunk_id == b"fmt ":
            fmt = struct.unpack_from("<HHIIHH", body, 0)
        elif chunk_id == b"data":
            payload = body
        at += 8 + size + (size & 1)
    if fmt is None or payload is None:
        raise ValueError("render.wav lacks fmt or data chunk")
    tag, channels, rate, byte_rate, align, bits = fmt
    if channels != 2:
        raise ValueError("render.wav must be stereo")
    if tag == 3 and bits == 32:
        encoding, width = "float32", 4
    elif tag == 1 and bits == 24:
        encoding, width = "pcm24", 3
    else:
        raise ValueError(f"unsupported WAV encoding tag={tag} bits={bits}")
    if align != 2 * width or byte_rate != rate * align or len(payload) % align:
        raise ValueError("inconsistent WAV block alignment")
    frames = len(payload) // align
    if encoding == "float32":
        values = struct.unpack(f"<{2 * frames}f", payload)
    else:
        values = []
        for i in range(0, len(payload), 3):
            raw = payload[i] | (payload[i + 1] << 8) | (payload[i + 2] << 16)
            values.append((raw - (1 << 24) if raw & 0x800000 else raw) / 8388607.0)
    left, right = list(values[0::2]), list(values[1::2])
    if not all(math.isfinite(v) for v in left) or not all(math.isfinite(v) for v in right):
        raise ValueError("render.wav contains non-finite samples")
    return rate, left, right, encoding


def read_summary(path: Path) -> dict:
    with path.open(encoding="utf-8") as stream:
        summary = json.load(stream, parse_constant=reject_json_constant)
    if summary.get("schema_version") != 1 or summary.get("tool") != TOOL:
        raise ValueError("unsupported summary schema or tool")
    if summary.get("evidence_label") != LABEL:
        raise ValueError("summary is not labelled as a synthetic prototype render")
    render = summary.get("render", {})
    if render.get("normalization") != "none_fixed_gain" or render.get("clipping") != "none":
        raise ValueError("analysis requires a fixed-gain, unclipped render")
    for key in ("sample_rate_hz", "frames", "gain_per_pa"):
        finite(render.get(key), f"render.{key}")
    return summary


def read_diagnostics(path: Path) -> list[dict]:
    rows = []
    with path.open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        required = {"block", "start_sample", "frames", "time_end_s", "body_energy_j", "fault"}
        required |= {f"string{s}_energy_j" for s in range(4)}
        if not reader.fieldnames or not required.issubset(reader.fieldnames):
            raise ValueError("diagnostics.csv lacks required columns")
        text = {"fault"} | {f"nail_phase_{s}" for s in range(4)}
        for index, raw in enumerate(reader):
            row = {}
            for key, value in raw.items():
                if key in text:
                    row[key] = value
                else:
                    number = float(value)
                    if not math.isfinite(number):
                        raise ValueError(f"diagnostics row {index} {key} is non-finite")
                    row[key] = number
            if int(row["block"]) != index:
                raise ValueError("diagnostics blocks are not contiguous")
            rows.append(row)
    if not rows:
        raise ValueError("diagnostics.csv has no rows")
    return rows


# ---- signal processing ----------------------------------------------------------------

_TWIDDLE: dict[int, list[complex]] = {}


def fft(x: list[complex]) -> list[complex]:
    """Recursive radix-2 FFT (length must be a power of two)."""
    n = len(x)
    if n == 1:
        return list(x)
    if n & (n - 1):
        raise ValueError("FFT length must be a power of two")
    even, odd = fft(x[0::2]), fft(x[1::2])
    tw = _TWIDDLE.get(n)
    if tw is None:
        tw = [cmath.exp(-2j * math.pi * k / n) for k in range(n // 2)]
        _TWIDDLE[n] = tw
    t = [o * w for o, w in zip(odd, tw)]
    return [e + u for e, u in zip(even, t)] + [e - u for e, u in zip(even, t)]


def hann(n: int) -> list[float]:
    return [0.5 - 0.5 * math.cos(2 * math.pi * i / n) for i in range(n)]


def power_spectra_pair(a: list[float], b: list[float]) -> tuple[list[float], list[float]]:
    """Two real-frame power spectra (bins 0..n/2) from one complex FFT."""
    n = len(a)
    z = fft([complex(p, q) for p, q in zip(a, b)])
    pa, pb = [], []
    for k in range(n // 2 + 1):
        zk, zc = z[k], z[-k % n].conjugate()
        pa.append(abs(0.5 * (zk + zc)) ** 2)
        pb.append(abs(-0.5j * (zk - zc)) ** 2)
    return pa, pb


def next_pow2(value: float) -> int:
    return 1 << max(1, math.ceil(math.log2(value)))


def spectrogram(mono: list[float], rate: int) -> tuple[list[list[float]], int, int]:
    n = next_pow2(0.04 * rate)
    hop = max(n // 4, math.ceil(max(1, len(mono) - n) / MAX_SPECTROGRAM_COLUMNS))
    window = hann(n)
    frames = []
    for start in range(0, max(1, len(mono) - n + 1), hop):
        segment = mono[start:start + n]
        segment += [0.0] * (n - len(segment))
        frames.append([s * w for s, w in zip(segment, window)])
    columns = []
    for i in range(0, len(frames), 2):
        other = frames[i + 1] if i + 1 < len(frames) else [0.0] * n
        pa, pb = power_spectra_pair(frames[i], other)
        columns.append(pa)
        if i + 1 < len(frames):
            columns.append(pb)
    return columns, n, hop


def peak_near(mono: list[float], rate: int, start: int, nominal: float) -> dict | None:
    """Largest spectral peak within ±SEARCH_CENTS of nominal, parabolic interpolation."""
    n = next_pow2(0.17 * rate)
    if start < 0 or start + n > len(mono):
        return None
    window = hann(n)
    segment = [s * w for s, w in zip(mono[start:start + n], window)]
    size = 2 * n
    power, _ = power_spectra_pair(segment + [0.0] * n, [0.0] * size)
    bin_hz = rate / size
    low = nominal * 2 ** (-SEARCH_CENTS / 1200)
    high = nominal * 2 ** (SEARCH_CENTS / 1200)
    k_low, k_high = max(1, math.floor(low / bin_hz)), min(len(power) - 2, math.ceil(high / bin_hz))
    if k_high <= k_low:
        return None
    k = max(range(k_low, k_high + 1), key=lambda i: power[i])
    if k in (k_low, k_high):
        return {"estimate_hz": None, "reason": "no interior maximum in the search window"}
    a, b, c = (math.log(max(power[i], 1e-300)) for i in (k - 1, k, k + 1))
    denominator = a - 2 * b + c
    delta = 0.5 * (a - c) / denominator if denominator < 0 else 0.0
    estimate = (k + delta) * bin_hz
    band = [power[i] for i in range(max(1, math.floor(40 / bin_hz)),
                                    min(len(power), math.ceil(4000 / bin_hz)))]
    strongest = max(range(len(band)), key=lambda i: band[i]) if band else 0
    peak_db = 10 * math.log10(max(power[k], 1e-300) / max(max(band, default=1e-300), 1e-300))
    return {"estimate_hz": estimate,
            "cents_vs_nominal": 1200 * math.log2(estimate / nominal),
            "level_db_vs_strongest_40_4000hz": peak_db,
            "strongest_peak_40_4000hz_hz": (strongest + math.floor(40 / bin_hz)) * bin_hz,
            "window_start_s": start / rate, "window_s": n / rate, "bin_hz": bin_hz}


# ---- graphics -------------------------------------------------------------------------

def svg_open(width: int, height: int, title: str, subtitle: str) -> list[str]:
    return [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" '
        f'viewBox="0 0 {width} {height}" role="img" aria-label="{html.escape(title)}">',
        '<rect width="100%" height="100%" fill="#0f172a"/>',
        f'<text x="90" y="34" font-size="22" fill="#f8fafc" font-family="sans-serif">'
        f'{html.escape(title)}</text>',
        f'<text x="90" y="56" font-size="12" fill="#94a3b8" font-family="sans-serif">'
        f'{html.escape(subtitle)}</text>',
    ]


def nice_ticks(low: float, high: float, count: int = 5) -> list[float]:
    """Ticks on a 1-2-5 grid inside [low, high]."""
    span = high - low
    if span <= 0:
        return [low]
    raw = span / count
    magnitude = 10.0 ** math.floor(math.log10(raw))
    step = next(m * magnitude for m in (1.0, 2.0, 5.0, 10.0) if m * magnitude >= raw)
    first = math.ceil(low / step - 1e-9) * step
    return [first + i * step for i in range(int((high - first) / step + 1e-9) + 1)]


def axes(elements: list[str], box: tuple[float, float, float, float], x_range: tuple[float, float],
         y_range: tuple[float, float], x_label: str, y_label: str, y_log: bool = False) -> None:
    """Frame, grid and labels. With y_log, y_range is in log10 units (integer decades)."""
    left, top, right, bottom = box
    x0, x1 = x_range
    y0, y1 = y_range
    elements.append(f'<rect x="{left}" y="{top}" width="{right - left}" height="{bottom - top}" '
                    'fill="none" stroke="#334155"/>')
    for xv in nice_ticks(x0, x1):
        xx = left + (right - left) * (xv - x0) / (x1 - x0)
        elements.append(f'<path d="M {xx:.1f} {top} V {bottom}" stroke="#1e293b"/>')
        elements.append(f'<text x="{xx:.1f}" y="{bottom + 18}" font-size="12" fill="#cbd5e1" '
                        f'text-anchor="middle" font-family="monospace">{xv:g}</text>')
    if y_log:
        step = max(1, math.ceil((y1 - y0) / 5))
        y_ticks = [float(v) for v in range(math.ceil(y0), math.floor(y1) + 1, step)]
    else:
        y_ticks = nice_ticks(y0, y1)
    for yv in y_ticks:
        yy = bottom - (bottom - top) * (yv - y0) / (y1 - y0)
        label = f"1e{yv:.0f}" if y_log else f"{yv:g}"
        elements.append(f'<path d="M {left} {yy:.1f} H {right}" stroke="#1e293b"/>')
        elements.append(f'<text x="{left - 8}" y="{yy + 4:.1f}" font-size="12" fill="#cbd5e1" '
                        f'text-anchor="end" font-family="monospace">{label}</text>')
    elements.append(f'<text x="{(left + right) / 2:.1f}" y="{bottom + 40}" font-size="13" '
                    f'fill="#e2e8f0" text-anchor="middle" font-family="sans-serif">'
                    f'{html.escape(x_label)}</text>')
    elements.append(f'<text transform="translate(22 {(top + bottom) / 2:.1f}) rotate(-90)" '
                    f'font-size="13" fill="#e2e8f0" text-anchor="middle" '
                    f'font-family="sans-serif">{html.escape(y_label)}</text>')


def legend(elements: list[str], x: float, y: float, items: list[tuple[str, str]]) -> None:
    for i, (colour, label) in enumerate(items):
        yy = y + 18 * i
        elements.append(f'<path d="M {x} {yy} h 22" stroke="{colour}" stroke-width="3"/>')
        elements.append(f'<text x="{x + 28}" y="{yy + 4}" font-size="12" fill="#e2e8f0" '
                        f'font-family="sans-serif">{html.escape(label)}</text>')


def waveform_svg(left: list[float], right: list[float], rate: int, gain: float, path: Path) -> None:
    width, height = 1140, 520
    elements = svg_open(width, height, "Observed output waveform (synthetic prototype)",
                        f"Fixed gain {gain:g} full scale per Pa, no normalization; min/max per column")
    duration = len(left) / rate
    peak = max(max(map(abs, left), default=0.0), max(map(abs, right), default=0.0)) or 1.0
    span = 10 ** math.ceil(math.log10(peak)) if peak > 0 else 1.0
    if span / 2 >= peak:
        span /= 2
    panels = [(left, "left", 80, 250), (right, "right", 290, 460)]
    columns = 960
    for index, (signal, name, top, bottom) in enumerate(panels):
        box = (90, top, 90 + columns, bottom)
        axes(elements, box, (0.0, duration), (-span, span),
             "time (s)" if index else "", f"{name} (full scale)")
        stride = max(1, math.ceil(len(signal) / columns))
        points_top, points_bottom = [], []
        for c in range(0, len(signal), stride):
            chunk = signal[c:c + stride]
            x = 90 + columns * c / max(1, len(signal))
            for value, store in ((max(chunk), points_top), (min(chunk), points_bottom)):
                store.append(f"{x:.1f},{bottom - (value + span) / (2 * span) * (bottom - top):.1f}")
        polygon = " ".join(points_top + points_bottom[::-1])
        elements.append(f'<polygon points="{polygon}" fill="{PALETTE[index]}" fill-opacity="0.7" '
                        f'stroke="{PALETTE[index]}" stroke-width="0.6"/>')
    elements.append("</svg>")
    path.write_text("\n".join(elements) + "\n", encoding="utf-8")


def colour(value: float) -> tuple[int, int, int]:
    position = min(max(value, 0.0), 1.0) * (len(VIRIDIS) - 1)
    i = min(int(position), len(VIRIDIS) - 2)
    t = position - i
    return tuple(round(a + (b - a) * t) for a, b in zip(VIRIDIS[i], VIRIDIS[i + 1]))  # type: ignore[return-value]


def write_png(path: Path, pixels: list[list[tuple[int, int, int]]]) -> None:
    height, width = len(pixels), len(pixels[0])
    raw = b"".join(b"\x00" + bytes(c for px in row for c in px) for row in pixels)

    def chunk(kind: bytes, payload: bytes) -> bytes:
        return (struct.pack(">I", len(payload)) + kind + payload +
                struct.pack(">I", zlib.crc32(kind + payload) & 0xFFFFFFFF))

    path.write_bytes(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height,
                                                                       8, 2, 0, 0, 0)) +
                     chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))


def spectrogram_outputs(mono: list[float], rate: int, out: Path, max_hz: float) -> dict:
    columns, n, hop = spectrogram(mono, rate)
    rows = 360
    bin_hz = rate / n
    top_bin = min(n // 2, math.ceil(max_hz / bin_hz))
    loudest = max((max(col[:top_bin + 1]) for col in columns), default=0.0)
    floor_db = -100.0
    pixels = []
    for r in range(rows):
        lo = round(top_bin * (rows - 1 - r) / rows)
        hi = max(lo + 1, round(top_bin * (rows - r) / rows))
        line = []
        for col in columns:
            value = max(col[lo:hi])
            db = 10 * math.log10(value / loudest) if value > 0 and loudest > 0 else floor_db
            line.append(colour((max(db, floor_db) - floor_db) / -floor_db))
        pixels.append(line)
    png = out / "spectrogram.png"
    write_png(png, pixels)
    encoded = base64.b64encode(png.read_bytes()).decode("ascii")
    duration = len(mono) / rate
    width, height = 1140, 560
    elements = svg_open(width, height, "Spectrogram of the mono mix (synthetic prototype)",
                        f"Hann {n} samples, hop {hop}; colour = dB re loudest cell "
                        f"(display scaling only, floor {floor_db:g} dB)")
    box = (90, 80, 1050, 480)
    elements.append(f'<image x="{box[0]}" y="{box[1]}" width="{box[2] - box[0]}" '
                    f'height="{box[3] - box[1]}" preserveAspectRatio="none" '
                    f'href="data:image/png;base64,{encoded}"/>')
    axes(elements, box, (0.0, duration), (0.0, top_bin * bin_hz), "time (s)", "frequency (Hz)")
    elements.append("</svg>")
    (out / "spectrogram.svg").write_text("\n".join(elements) + "\n", encoding="utf-8")
    return {"fft_size": n, "hop": hop, "columns": len(columns), "max_display_hz": top_bin * bin_hz,
            "colour_scale": "dB relative to loudest cell (display only)", "floor_db": floor_db}


def energy_svg(rows: list[dict], summary: dict, path: Path) -> dict:
    ledger = summary.get("ledger", {}).get("enabled") is True and "ledger_residual_j" in rows[0]
    width, height = 1140, 640 if ledger else 520
    elements = svg_open(width, height, "Mechanical energy by subsystem (synthetic prototype)",
                        "Modal H[n+1/2] per string (both polarizations) and body at block ends; "
                        "values below 1e-10 of the maximum are drawn at the floor")
    times = [r["time_end_s"] for r in rows]
    series = [(f"string {s}", [r[f"string{s}_energy_j"] for r in rows]) for s in range(4)]
    series.append(("body", [r["body_energy_j"] for r in rows]))
    top_value = max((max(v) for _, v in series), default=0.0)
    result: dict = {"max_energy_j": top_value}
    if top_value > 0:
        floor = top_value * 1e-10
        y1 = math.ceil(math.log10(top_value))
        y0 = y1 - 10
        box = (90, 80, 950, 440)
        axes(elements, box, (0.0, times[-1]), (y0, y1), "" if ledger else "time (s)",
             "energy (J), log10", y_log=True)
        for index, (_, values) in enumerate(series):
            pts = " ".join(
                f"{box[0] + (box[2] - box[0]) * t / times[-1]:.1f},"
                f"{box[3] - (math.log10(max(v, floor)) - y0) / (y1 - y0) * (box[3] - box[1]):.1f}"
                for t, v in zip(times, values))
            elements.append(f'<polyline points="{pts}" fill="none" stroke="{PALETTE[index]}" '
                            'stroke-width="1.8"/>')
        legend(elements, 970, 96, [(PALETTE[i], name) for i, (name, _) in enumerate(series)])
    else:
        elements.append('<text x="90" y="200" font-size="16" fill="#f59e0b" '
                        'font-family="sans-serif">All recorded energies are zero.</text>')
    if ledger:
        stored_max = max(r["ledger_stored_j"] for r in rows) or 1.0
        relative = [abs(r["ledger_residual_j"]) / stored_max for r in rows]
        worst = max(relative)
        y1 = math.ceil(math.log10(max(worst, 1e-18))) + 1
        y0 = y1 - 8
        box = (90, 480, 950, 590)
        axes(elements, box, (0.0, times[-1]), (y0, y1), "time (s)", "|residual| / max stored",
             y_log=True)
        pts = " ".join(
            f"{box[0] + (box[2] - box[0]) * t / times[-1]:.1f},"
            f"{box[3] - (math.log10(max(v, 10.0 ** y0)) - y0) / (y1 - y0) * (box[3] - box[1]):.1f}"
            for t, v in zip(times, relative))
        elements.append(f'<polyline points="{pts}" fill="none" stroke="{PALETTE[5]}" '
                        'stroke-width="1.8"/>')
        result.update({"ledger_max_relative_residual": worst, "ledger_max_stored_j": stored_max})
    elements.append("</svg>")
    path.write_text("\n".join(elements) + "\n", encoding="utf-8")
    return result


def pitch_svg(table: list[dict], path: Path) -> None:
    width, height = 1140, 460
    elements = svg_open(width, height, "Estimated fundamental vs nominal (synthetic prototype)",
                        f"Spectral peak within ±{SEARCH_CENTS:g} cents of the ideal stiff-string "
                        "nominal; descriptive, no pass/fail")
    points = [(row["time_s"], row["cents_vs_nominal"], row["string"]) for row in table
              if row.get("cents_vs_nominal") is not None]
    box = (90, 80, 950, 400)
    duration = max([row["time_s"] for row in table] + [1.0])
    axes(elements, box, (0.0, duration), (-SEARCH_CENTS, SEARCH_CENTS), "stroke time (s)",
         "estimate - nominal (cents)")
    zero = box[3] - 0.5 * (box[3] - box[1])
    elements.append(f'<path d="M {box[0]} {zero:.1f} H {box[2]}" stroke="#64748b" '
                    'stroke-dasharray="4 4"/>')
    for t, cents_value, string in points:
        x = box[0] + (box[2] - box[0]) * t / duration
        y = box[3] - (cents_value + SEARCH_CENTS) / (2 * SEARCH_CENTS) * (box[3] - box[1])
        elements.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="4" fill="{PALETTE[string % 8]}"/>')
    legend(elements, 970, 96, [(PALETTE[s], f"string {s}") for s in range(4)])
    if not points:
        elements.append('<text x="110" y="120" font-size="15" fill="#f59e0b" '
                        'font-family="sans-serif">No stroke had an estimable fundamental.</text>')
    elements.append("</svg>")
    path.write_text("\n".join(elements) + "\n", encoding="utf-8")


# ---- report ---------------------------------------------------------------------------

def produce(source: Path, out: Path, revision: str) -> dict:
    for name in INPUT_FILES:
        if not (source / name).is_file():
            raise ValueError(f"missing input {name}")
    if out.exists() and (not out.is_dir() or any(out.iterdir())):
        raise ValueError(f"refusing to write into non-empty output {out}")
    summary = read_summary(source / "summary.json")
    rows = read_diagnostics(source / "diagnostics.csv")
    rate, left, right, encoding = read_wav(source / "render.wav")
    render = summary["render"]
    if rate != render["sample_rate_hz"] or len(left) != render["frames"]:
        raise ValueError("render.wav rate/length disagree with summary.json")
    if encoding != render.get("format"):
        raise ValueError("render.wav encoding disagrees with summary.json")
    if int(rows[-1]["start_sample"] + rows[-1]["frames"]) != len(left):
        raise ValueError("diagnostics do not cover the rendered frames")
    peak_left, peak_right = max(map(abs, left)), max(map(abs, right))
    for measured, reported, name in ((peak_left, summary["output"]["peak_left"], "left"),
                                     (peak_right, summary["output"]["peak_right"], "right")):
        tolerance = 1.0 / 8388607 if encoding == "pcm24" else 0.0
        if abs(measured - finite(reported, f"peak_{name}")) > tolerance + 1e-12 * measured:
            raise ValueError(f"{name} peak disagrees with summary.json")
    out.mkdir(parents=True, exist_ok=True)
    mono = [0.5 * (a + b) for a, b in zip(left, right)]

    def rms(signal: list[float]) -> float:
        return math.sqrt(sum(v * v for v in signal) / len(signal)) if signal else 0.0

    waveform_svg(left, right, rate, render["gain_per_pa"], out / "waveform.svg")
    spectro = spectrogram_outputs(mono, rate, out, min(rate / 2, 6000.0))
    energy = energy_svg(rows, summary, out / "energy.svg")
    pitch_rows = []
    for stroke in summary.get("strokes", [])[:MAX_PITCH_STROKES]:
        nominal = stroke.get("nominal_hz")
        row = {"time_s": stroke["time_s"], "string": stroke["string"], "stop": stroke["stop"],
               "fret": stroke["fret"], "nominal_hz": nominal}
        if nominal is None:
            row["reason"] = "no nominal pitch (light touch / harmonic or damping)"
        else:
            found = peak_near(mono, rate, round((stroke["time_s"] + STROKE_OFFSET_S) * rate),
                              finite(nominal, "nominal_hz"))
            if found is None:
                row["reason"] = "analysis window exceeds the render"
            else:
                row.update(found)
        row.setdefault("cents_vs_nominal", None)
        pitch_rows.append(row)
    pitch_svg(pitch_rows, out / "pitch.svg")
    shutil.copyfile(source / "render.wav", out / "render.wav")

    analysis = {
        "schema_version": 1,
        "evidence_label": LABEL,
        "disclaimer": DISCLAIMER,
        "source_revision": revision,
        "inputs": {name: {"sha256": sha256(source / name), "bytes": (source / name).stat().st_size}
                   for name in INPUT_FILES},
        "signal": {"sample_rate_hz": rate, "frames": len(left), "encoding": encoding,
                   "duration_s": len(left) / rate, "peak_left": peak_left, "peak_right": peak_right,
                   "rms_left": rms(left), "rms_right": rms(right),
                   "mean_left": sum(left) / len(left), "mean_right": sum(right) / len(right)},
        "spectrogram": spectro,
        "energy": energy,
        "faults": summary.get("faults"),
        "ledger": summary.get("ledger"),
        "pitch_method": (f"Hann window of {next_pow2(0.17 * rate)} samples starting "
                         f"{STROKE_OFFSET_S} s after each nail stroke, zero-padded x2, largest "
                         f"mono-mix spectral peak within ±{SEARCH_CENTS:g} cents of the nominal, "
                         "parabolic interpolation on log power. Sympathetic and other ringing "
                         "strings are in the same mix; the estimate is descriptive only."),
        "pitch": pitch_rows,
        "pitch_strokes_analyzed": len(pitch_rows),
        "pitch_strokes_total": len(summary.get("strokes", [])),
    }
    (out / "analysis.json").write_text(json.dumps(analysis, indent=2, allow_nan=False) + "\n",
                                       encoding="utf-8")
    write_html(out / "index.html", summary, analysis)
    return analysis


def write_html(path: Path, summary: dict, analysis: dict) -> None:
    esc = html.escape
    score, preset, config = summary["score"], summary["preset"], summary["config"]
    faults, timing = summary["faults"], summary["timing"]

    def fmt(value: object, spec: str = ".6g") -> str:
        if value is None:
            return "—"
        if isinstance(value, (int, float)) and not isinstance(value, bool):
            return format(float(value), spec)
        return str(value)

    provenance = [
        ("score", f'{score["title"]} — status: {score["status"]}'),
        ("score path", score["path"]),
        ("config", f'{config["label"]} (provenance: {config["provenance"]})'),
        ("body", f'{config["body_label"]} (provenance: {config["body_provenance"]}, '
                 f'{config["body_modes"]} modes)'),
        ("tuning", f'{preset["tuning_id"] or "preset default"} — {preset["tuning_status"] or "see preset"}'),
        ("fret map", f'{preset["fret_map_id"] or "preset default"} — '
                     f'{preset["fret_map_status"] or "see preset"}'),
        ("source revision", analysis["source_revision"]),
        ("render", f'{summary["render"]["sample_rate_hz"]} Hz, {summary["render"]["frames"]} frames, '
                   f'{summary["render"]["format"]}, gain {summary["render"]["gain_per_pa"]:g}/Pa, '
                   "no normalization, no clipping"),
        ("faults", f'first={faults["first_fault"]}, rejected events={faults["rejected_events"]}, '
                   f'Newton failures={faults["newton_failures"]}, '
                   f'capacity overflows={faults["capacity_overflows"]}, '
                   f'containment resets={faults["containment_resets"]}'),
        ("timing", f'{fmt(timing["speed_vs_realtime"], ".3g")}x realtime — {timing["label"]}'),
    ]
    ledger = summary.get("ledger", {})
    if ledger.get("enabled"):
        relative = (ledger["max_abs_residual_j"] / ledger["max_stored_j"]
                    if ledger.get("max_stored_j") else None)
        provenance.append(("energy ledger", f'max |residual| {fmt(ledger["max_abs_residual_j"])} J, '
                                            f'relative to max stored {fmt(relative)}'))
    rows = "\n".join(f"<tr><th>{esc(k)}</th><td>{esc(str(v))}</td></tr>" for k, v in provenance)
    pitch = "\n".join(
        f'<tr><td>{fmt(p["time_s"], ".3f")}</td><td>{p["string"]}</td><td>{esc(p["stop"])}</td>'
        f'<td>{p["fret"]}</td><td>{fmt(p["nominal_hz"], ".2f")}</td>'
        f'<td>{fmt(p.get("estimate_hz"), ".2f")}</td><td>{fmt(p.get("cents_vs_nominal"), "+.1f")}</td>'
        f'<td>{esc(p.get("reason", ""))}</td></tr>' for p in analysis["pitch"])
    page = f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Prototype render analysis</title>
<style>body{{background:#0f172a;color:#e2e8f0;font:16px/1.6 system-ui,sans-serif;max-width:1180px;
margin:0 auto;padding:16px}} img{{max-width:100%;height:auto;border:1px solid #334155}}
.warning{{padding:16px;border:1px solid #f59e0b;background:#451a03}}
table{{border-collapse:collapse}} th,td{{border:1px solid #334155;padding:4px 8px;text-align:left;
vertical-align:top}} th{{color:#94a3b8;font-weight:normal}} code{{color:#fbbf24}}</style></head>
<body><h1>Setar prototype render analysis</h1>
<p class="warning"><strong>{esc(DISCLAIMER)}</strong></p>
<h2>Provenance</h2><table>{rows}</table>
<h2>Audio</h2><p><audio controls src="render.wav"></audio> (copied unchanged from the render)</p>
<h2>Waveform</h2><img src="waveform.svg" alt="Left and right observed output waveform envelopes">
<h2>Spectrogram</h2><img src="spectrogram.svg" alt="Spectrogram of the mono mix">
<h2>Energy</h2><img src="energy.svg" alt="Modal energy per string and body over time">
<h2>Pitch estimates</h2><img src="pitch.svg" alt="Estimated minus nominal fundamental in cents">
<p>{esc(analysis["pitch_method"])} Nominal frequencies are ideal stiff-string values for the speaking
length implied by the score; they are not targets the model was tuned to and not measurements.
{analysis["pitch_strokes_analyzed"]} of {analysis["pitch_strokes_total"]} strokes analysed.</p>
<table><tr><th>time (s)</th><th>string</th><th>stop</th><th>fret</th><th>nominal (Hz)</th>
<th>estimate (Hz)</th><th>cents</th><th>note</th></tr>{pitch}</table>
<h2>What this page cannot show</h2>
<p>It cannot show that the sound resembles a real setar: there is no measured specimen, no
recording comparison and no listening study. The observation is a synthetic far-field monopole/dipole
model of a synthetic body, not a calibrated microphone. The wall-clock speed is one run on one
machine, not a realtime qualification. See <code>analysis.json</code> for exact numbers and input
hashes.</p></body></html>
"""
    path.write_text(page, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True, help="setar_play output directory")
    parser.add_argument("--output", type=Path, required=True, help="new or empty report directory")
    parser.add_argument("--source-revision", default="unrecorded")
    args = parser.parse_args()
    try:
        analysis = produce(args.input, args.output, args.source_revision)
    except (OSError, ValueError, KeyError, TypeError, struct.error) as exc:
        print(f"analyze_render: {exc}", file=sys.stderr)
        return 1
    print(f"Analysed {analysis['signal']['duration_s']:.3f} s synthetic render; "
          f"{analysis['pitch_strokes_analyzed']} stroke pitch estimates; report in {args.output}")
    print("This is a synthetic prototype render, not a validated setar.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
