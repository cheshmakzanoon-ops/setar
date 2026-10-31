#!/usr/bin/env python3
"""Integration test: render every demo score with setar_play and analyze it.

Checks file formats, finiteness, fault counters, the discrete energy ledger and the
analysis report, plus failure paths (malformed score, non-empty output directory,
pcm24 overflow). Passing means the synthetic pipeline is self-consistent; it says
nothing about resemblance to a real setar.
"""

from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor
import importlib.util
import json
import math
import os
from pathlib import Path
import subprocess
import sys
import tempfile

# Ledger residual bound relative to the largest stored energy of the run. Roundoff of a
# double-precision energy-consistent scheme over ~1e6 steps is far below this; a
# missing or mis-booked power term (release, switching, actuator work) is far above it.
MAX_RELATIVE_LEDGER_RESIDUAL = 1.0e-6

SHORT_SCORE = """title Integration short score (temporary)
status synthetic test fixture, not a performance
pluck @0.0 string=0
pluck @0.6 string=1 fret=2
end @1.2
"""


def load_analyzer(path: Path):
    spec = importlib.util.spec_from_file_location("analyze_render", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot import analyzer {path}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def run(command: list[str]) -> subprocess.CompletedProcess:
    return subprocess.run(command, capture_output=True, text=True, timeout=280, check=False)


def check(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def render(player: Path, score: Path, out: Path, *extra: str) -> subprocess.CompletedProcess:
    return run([str(player), "--score", str(score), "--output-dir", str(out),
                "--sample-rate", "48000", *extra])


def verify_render(analyzer, out: Path, expect_ledger: bool) -> dict:
    for name in ("render.wav", "diagnostics.csv", "summary.json"):
        check((out / name).is_file(), f"{out}: missing {name}")
    summary = analyzer.read_summary(out / "summary.json")
    rate, left, right, encoding = analyzer.read_wav(out / "render.wav")
    render_info = summary["render"]
    check(rate == 48000 and render_info["sample_rate_hz"] == 48000, "sample rate mismatch")
    check(len(left) == render_info["frames"] == len(right), "frame count mismatch")
    check(encoding == render_info["format"], "encoding mismatch")
    check(all(math.isfinite(v) for v in left + right), "non-finite samples")
    peak = max(max(map(abs, left)), max(map(abs, right)))
    check(peak > 0.0, f"{out}: output is silent")
    faults = summary["faults"]
    check(summary["status"] == "ok" and faults["first_fault"] == "none", f"engine faults {faults}")
    for key in ("rejected_events", "newton_failures", "capacity_overflows", "containment_resets"):
        check(faults[key] == 0, f"{key} = {faults[key]}")
    rows = analyzer.read_diagnostics(out / "diagnostics.csv")
    check(int(rows[-1]["start_sample"] + rows[-1]["frames"]) == len(left), "diagnostics coverage")
    if expect_ledger:
        ledger = summary["ledger"]
        check(ledger["enabled"] is True, "ledger not enabled")
        check(ledger["max_stored_j"] > 0.0, "ledger stored no energy")
        relative = ledger["max_abs_residual_j"] / ledger["max_stored_j"]
        print(f"  ledger max |residual| / max stored = {relative:.3e}")
        check(relative <= MAX_RELATIVE_LEDGER_RESIDUAL,
              f"ledger residual {relative:.3e} exceeds {MAX_RELATIVE_LEDGER_RESIDUAL:g}")
        check(abs(rows[-1]["ledger_residual_j"]) <= ledger["max_abs_residual_j"] * (1 + 1e-12),
              "diagnostics and summary ledger disagree")
    print(f"  {len(left)} frames, peak {peak:.4g}, "
          f"{summary['timing']['speed_vs_realtime']:.3g}x realtime (this machine)")
    return summary


def verify_report(analyzer_path: Path, analyzer, source: Path, report: Path) -> None:
    result = run([sys.executable, str(analyzer_path), "--input", str(source), "--output",
                  str(report), "--source-revision", "integration-test"])
    check(result.returncode == 0, f"analyzer failed: {result.stderr}")
    for name in analyzer.OUTPUT_FILES:
        check((report / name).is_file() and (report / name).stat().st_size > 0,
              f"report missing {name}")
    analysis = json.loads((report / "analysis.json").read_text(encoding="utf-8"),
                          parse_constant=analyzer.reject_json_constant)
    check(analysis["evidence_label"] == analyzer.LABEL, "analysis label")
    check("SYNTHETIC" in (report / "index.html").read_text(encoding="utf-8"), "missing disclaimer")
    check((report / "spectrogram.png").read_bytes()[:8] == b"\x89PNG\r\n\x1a\n", "PNG signature")
    again = run([sys.executable, str(analyzer_path), "--input", str(source), "--output",
                 str(report)])
    check(again.returncode != 0, "analyzer overwrote a non-empty report directory")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--player", type=Path, required=True)
    parser.add_argument("--analyzer", type=Path, required=True)
    parser.add_argument("--scores", type=Path, required=True)
    args = parser.parse_args()
    analyzer = load_analyzer(args.analyzer)
    with tempfile.TemporaryDirectory(prefix="setar_play_test_") as temporary:
        tmp = Path(temporary)
        short = tmp / "short.score"
        short.write_text(SHORT_SCORE, encoding="utf-8")
        scores = sorted(args.scores.glob("*.score")) if args.scores.is_dir() else []
        if not scores:
            print(f"no *.score in {args.scores}; using only the temporary short score")
            scores = [short]

        # Full renders of every score run concurrently (each is single-threaded); the
        # short tail keeps Debug and sanitizer builds within the CTest timeout.
        jobs = {score: tmp / f"render_{index}" for index, score in enumerate(scores)}
        with ThreadPoolExecutor(max_workers=max(1, os.cpu_count() or 1)) as pool:
            results = dict(zip(jobs, pool.map(
                lambda item: render(args.player, item[0], item[1], "--ledger", "--tail", "0.5"),
                jobs.items())))
        for score, out in jobs.items():
            print(f"render {score.name}")
            result = results[score]
            check(result.returncode == 0, f"{score.name}: exit {result.returncode}: {result.stderr}")
            verify_render(analyzer, out, expect_ledger=True)
        with ThreadPoolExecutor(max_workers=max(1, os.cpu_count() or 1)) as pool:
            list(pool.map(lambda item: verify_report(args.analyzer, analyzer, item[1],
                                                     tmp / f"report_{item[1].name}"),
                          jobs.items()))

        # Short score: float32 reference, then pcm24 at half scale must round-trip.
        out = tmp / "short_float"
        result = render(args.player, short, out, "--tail", "0.3")
        check(result.returncode == 0, f"short score failed: {result.stderr}")
        summary = verify_render(analyzer, out, expect_ledger=False)
        short_peak = max(summary["output"]["peak_left"], summary["output"]["peak_right"])
        out = tmp / "pcm24"
        result = render(args.player, short, out, "--tail", "0.3", "--format", "pcm24",
                        "--gain-per-pa", repr(0.5 / short_peak))
        check(result.returncode == 0, f"pcm24 render failed: {result.stderr}")
        verify_render(analyzer, out, expect_ledger=False)
        verify_report(args.analyzer, analyzer, out, tmp / "report_pcm24")

        # pcm24 overflow must be refused, not clipped, and must write no audio.
        out = tmp / "overflow"
        result = render(args.player, short, out, "--tail", "0.3", "--format", "pcm24",
                        "--gain-per-pa", repr(1.0e3 / short_peak))
        check(result.returncode == 2 and "full scale" in result.stderr,
              f"pcm24 overflow not refused: {result.returncode} {result.stderr}")
        check(not out.exists(), "overflowing render wrote output")

        # Malformed score: invalid input, nothing written.
        bad = tmp / "bad.score"
        bad.write_text("title bad\nstatus synthetic\nthis is not a command\n", encoding="utf-8")
        out = tmp / "bad_out"
        result = render(args.player, bad, out)
        check(result.returncode == 2, f"malformed score accepted: {result.returncode}")
        check(not out.exists(), "malformed score wrote output")

        # Non-empty output directory is refused and left untouched.
        occupied = tmp / "occupied"
        occupied.mkdir()
        (occupied / "keep.txt").write_text("existing evidence\n", encoding="utf-8")
        result = render(args.player, short, occupied, "--tail", "0.3")
        check(result.returncode == 2 and "non-empty" in result.stderr,
              f"non-empty output accepted: {result.returncode} {result.stderr}")
        check(sorted(p.name for p in occupied.iterdir()) == ["keep.txt"], "output dir modified")

        # Unknown option and missing score are invalid input.
        check(run([str(args.player), "--bogus", "1"]).returncode == 2, "unknown option accepted")
        check(render(args.player, tmp / "missing.score", tmp / "missing_out").returncode == 2,
              "missing score accepted")

        # Grounded bridge (no body) renders without faults; its observed output is zero
        # by the engine contract, so silence is not asserted as a defect here.
        result = render(args.player, short, tmp / "no_body", "--no-body", "--tail", "0.3")
        check(result.returncode == 0, f"--no-body render failed: {result.stderr}")
    print("integration.setar_play: all checks passed (synthetic pipeline self-consistency only)")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except AssertionError as error:
        print(f"FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
