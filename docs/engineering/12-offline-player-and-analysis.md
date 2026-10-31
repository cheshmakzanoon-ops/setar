# Offline player, renders and analysis

**Status: implemented tooling for a synthetic, unvalidated prototype.** The
offline player `setar_play` renders a text score through the four-string
prototype engine described in [the prototype chapter](11-playable-prototype.md).
The analyzer turns one render into graphs, pitch estimates and an offline HTML
page. A render is a deterministic output of synthetic parameters. It is not a
recording, not a measured setar, and not evidence of acoustic or perceptual
fidelity. Every file it writes carries that label.

## Prerequisites and sources

- A configured build with `SETAR_BUILD_INSTRUMENT=ON` (the default), which builds
  `setar_play` from [`apps/setar_play.cpp`](../../apps/setar_play.cpp).
- Python 3.10 or newer with the standard library only, for
  [`tools/analyze_render.py`](../../tools/analyze_render.py) and
  [`tools/test_setar_play.py`](../../tools/test_setar_play.py).
- A score in the format of
  [the score-format chapter](../performance/07-score-format.md). Demo scores live
  in `data/scores/`.

The player composes existing modules and adds no physics: the preset builder
(`synthetic_setar_config`), the performance layer (`parse_score`, `schedule`),
and the engine (`Instrument::prepare`, `Instrument::process`). Their contracts are
in the public headers under `include/setar/instrument/`.

## Render a score

```sh
cmake --preset headless-release
cmake --build --preset headless-release --target setar_play
./build/headless-release/setar_play --score data/scores/<name>.score \
    --output-dir artifacts/play/<name> --ledger
python3 tools/analyze_render.py --input artifacts/play/<name> \
    --output artifacts/play/<name>-report --source-revision "$(git rev-parse HEAD)"
```

| Option | Meaning | Default |
| --- | --- | --- |
| `--score PATH` | Text score; required. | — |
| `--output-dir DIR` | New or empty directory; required. A non-empty directory is refused. | — |
| `--sample-rate HZ` | Integer rate in [22050, 192000]. | 48000 |
| `--block FRAMES` | `process()` block size in [1, 8192]. Gestures land at exact sample offsets inside blocks. | 256 |
| `--tuning ID`, `--fret-map ID` | Synthetic preset identifiers from `setar_preset.hpp`. | preset defaults |
| `--gain-per-pa G` | Fixed observation gain: digital full scale per pascal. | 1 |
| `--format F` | `float32` (IEEE float WAV) or `pcm24`. | `float32` |
| `--ledger` | Enables the engine's discrete energy ledger (extra work per sample). | off |
| `--tail SECONDS` | Render time after the later of the score `end` time and the last gesture, in [0, 60]. | 2 |
| `--no-body` | Empty body: bridge contacts are grounded. The engine observes pressure only through body modes, so this render is silent by contract. | off |
| `--allow-faults` | Exit 0 even when the engine reported faults. | off |

Exit codes: `0` success; `2` invalid input (bad option, unreadable or malformed
score, inadmissible preset, non-empty output directory, `pcm24` overflow); `1`
other failures, including a non-finite output sample; `3` the engine reported a
fault (outputs are still written so the fault can be inspected, and
`summary.json` has `"status": "engine_faults"`).

There is **no normalization and no clipping**. With `pcm24`, a peak above full
scale aborts the run before any file is written; lower `--gain-per-pa` or use
`float32`. The event capacity passed to the engine is the larger of 64 and the
largest number of scheduled gestures in any block, so the player never makes the
engine drop a scheduled gesture.

## Output schema

### `render.wav`

Stereo, interleaved, little-endian. `float32` uses `WAVE_FORMAT_IEEE_FLOAT`
(tag 3) with a `fact` chunk; `pcm24` uses integer PCM with full scale
8388607 at magnitude 1. Channels are the engine's two observation channels
(left, right): synthetic far-field pressure of the synthetic body model, in
pascals, multiplied by the fixed gain. Python's `wave` module cannot read tag 3,
so the analyzer has its own RIFF parser.

### `diagnostics.csv`

One row per processed block, values at the end of the block:

| Columns | Meaning |
| --- | --- |
| `block`, `start_sample`, `frames`, `time_end_s` | Block identity and end time. |
| `string{0..3}_energy_j`, `body_energy_j` | Modal energy \(H^{n+1/2}\) per string (both polarizations) and body, from `Instrument::snapshot()`. |
| `active_channels`, `max_newton_iterations`, `newton_failures`, `capacity_overflows`, `rejected_events`, `containment_resets`, `fault` | That block's `ProcessSummary`. |
| `nail_phase_{s}`, `nail_force_{s}_n` | Nail state (`idle`, `approaching`, `in_contact`, `released`) and contact force. |
| `finger_force_s{s}_k{k}_n` | Finger pad force per string and slot. |
| `ledger_stored_j`, `ledger_interaction_potential_j`, `ledger_external_work_j`, `ledger_dissipation_j`, `ledger_switching_j`, `ledger_residual_j` | Only with `--ledger`: cumulative `EnergyLedger` fields; `stored` = string + body + interaction potential. |
| `wall_s` | Wall-clock time of that `process()` call on this machine. |

### `summary.json`

`schema_version` 1, `tool` `setar_play`, `evidence_label`
`synthetic_unvalidated_prototype_render`, a `disclaimer`, and `status`
(`ok` or `engine_faults`). Sections:

- `score`: path, title, status line copied from the score, end time, gesture count.
- `preset`: requested tuning and fret-map identifiers with their status and
  description strings, or `null` when the preset default was used; `include_body`.
- `config`: configuration and body label and provenance, body mode count, fret
  count, mode-frequency cap, and per string the length, bridge position,
  tension, density, bending stiffness and the ideal stiff-string open frequency.
- `engine`: retained string modes per polarization, body modes, total degrees of
  freedom and the solver capacities used.
- `render`: rate, block, frames, duration, tail, format, gain, `normalization`
  `none_fixed_gain`, `clipping` `none`, observation description, ledger flag.
- `output`: peak per channel.
- `faults`: first fault and summed counters over all blocks, maximum Newton
  iterations and active channels.
- `timing`: wall-clock seconds, audio seconds, `speed_vs_realtime`, the largest
  single-block wall time and the block duration. These are **one measurement on
  the machine that ran the command**, not a realtime qualification: the offline
  loop allocates around the engine, the operating system is not configured for
  audio, and no worst-case input was searched.
- `ledger`: with `--ledger`, residual extrema, maximum and final stored energy,
  and the final cumulative external work, dissipation and switching energy.
- `strokes`: every nail stroke with time, string, contact position, angle,
  speed, release force, the stop implied by the score (`open`, `fret`, or
  `touch` for a pad above the fret-top line), the fret number, speaking length
  and `nominal_hz`. The nominal is the ideal simply supported stiff-string
  fundamental of that speaking length from the configured tension, density and
  stiffness. It ignores the finger offset, bridge and body compliance, contact
  stiffness, and pressing-induced tension, so it is a reference line for reading
  the analysis, not a target the model was tuned to.

## Analysis report

`tools/analyze_render.py --input RENDER --output REPORT [--source-revision REV]`
validates the three inputs against each other (rate, frame count, encoding,
peaks, diagnostics coverage, labels, fixed gain), refuses a non-empty report
directory, and writes:

| File | Content |
| --- | --- |
| `waveform.svg` | Per-pixel min/max envelope of left and right output at the fixed gain. |
| `spectrogram.png`, `spectrogram.svg` | Hann-window short-time power spectrum of the mono mix to 6 kHz. Colour is dB relative to the loudest cell, a display choice only. The SVG embeds the PNG with axes. |
| `energy.svg` | String and body modal energy on a log axis; with a ledger, the relative residual \(|r|/\max E_\text{stored}\). |
| `pitch.svg` | Per-stroke estimate minus nominal, in cents. |
| `analysis.json` | Input SHA-256 hashes, signal statistics, spectrogram parameters, energy and ledger extrema, faults and the pitch table. |
| `index.html` | Offline page with the disclaimer, provenance, an audio player, the graphs and the pitch table. |
| `render.wav` | Unchanged copy so the page is self-contained. |

Pitch estimation: a Hann window of the next power of two above 0.17 s, starting
30 ms after the stroke, zero-padded by two, and the largest spectral peak of the
mono mix within ±80 cents of the nominal, refined by parabolic interpolation of
log power. The table also lists the peak level relative to the strongest
40–4000 Hz peak, because other strings ring in the same mix. A stroke with a
light-touch stop has no nominal and is reported without an estimate. At most 48
strokes per render are analysed to bound the pure-Python FFT work.

## Integration test

CTest `integration.setar_play` runs [`tools/test_setar_play.py`](../../tools/test_setar_play.py)
with the built player, the analyzer and `data/scores/`. If no score exists it
writes a temporary fallback score. For every score it renders at 48 kHz with the
ledger and checks: WAV structure, frame count against the summary, finite and
non-silent output, zero fault counters, ledger residual at most `1e-6` of the
largest stored energy, consistency of the diagnostics with the summary, and a
complete analysis report. It also checks a `pcm24` render at half scale, a
refused `pcm24` overflow with no audio written, a refused malformed score, a
refused non-empty output directory with the existing file untouched, a refused
unknown option and missing score, the analyzer's refusal to overwrite a report,
and a fault-free `--no-body` render.

The ledger bound is a defect detector: roundoff of an energy-consistent
double-precision scheme over the length of a demo score is orders of magnitude
smaller, while an unbooked release, switching or actuator power term is orders
of magnitude larger. It is not a statement about physical accuracy.

## What the analysis can and cannot show

It can show that a render is finite, deterministic for its inputs, free of
engine faults, energetically self-consistent within the discrete ledger, and
where its spectral energy and estimated fundamentals lie relative to simple
string theory for the configured synthetic parameters.

It cannot show that the output resembles a real setar. There is no measured
specimen, no recording comparison, no calibrated microphone, no room and no
listening study. The observation model is a synthetic monopole/dipole of a
synthetic plate body. The nominal-pitch comparison uses an idealized formula
and a mixed signal; disagreement may come from the model, the formula's
omissions, or the estimator picking a neighbouring component. The wall-clock
figure is not a realtime qualification; see the realtime chapters and the
`setar_benchmark` evidence for that question.

## Failure modes and next actions

- Engine faults produce exit code 3 and a written but labelled render; inspect
  `diagnostics.csv` for the first faulting block before changing tolerances.
- A ledger residual above the bound means a power term is missing or
  mis-signed; it must be fixed in the engine, not by loosening the bound.
- Next: compare renders across sample rates and block sizes in the report
  (block-size invariance is a realtime test), and, once a specimen exists,
  replace the nominal pitch reference with measured open-string and fretted
  frequencies under the measurement chapters' protocols.
