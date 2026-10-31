# Prototype demonstration renders

**These are synthetic renders of an unvalidated physical-model prototype.** They
are not recordings, not measurements of any setar, and not evidence of acoustic
or perceptual fidelity. Every parameter behind them is a declared synthetic or
historical-theory placeholder (see the
[preset chapter](../../strings/08-prototype-string-and-fret-presets.md) and the
[synthetic body chapter](../../body/10-synthetic-prototype-body.md)). The scores
are engineering demonstrations, not transcriptions of a performance, and their
gesture grammar has not been reviewed by a setar player.

Each directory holds one render of a score in [`data/scores`](../../../data/scores):

| Render | Score content | Page |
| --- | --- | --- |
| `01-open-strings` | Each open string, then course strums | [index](01-open-strings/index.html), [audio](01-open-strings/render.wav) |
| `02-fret-scale` | Scale on string 0 through the reconstructed Pythagorean fret map | [index](02-fret-scale/index.html), [audio](02-fret-scale/render.wav) |
| `03-phrase` | Short phrase with tremolo, slide, damping and a touch harmonic | [index](03-phrase/index.html), [audio](03-phrase/render.wav) |
| `04-dynamics` | One note at six dynamic levels | [index](04-dynamics/index.html), [audio](04-dynamics/render.wav) |

Files per render: `render.wav` (stereo, 48 kHz, 24-bit PCM, written unchanged by
`setar_play`; never normalized or clipped), `summary.json` (score, preset,
engine sizes, fault counters, energy-ledger extrema, this-machine timing),
`analysis.json` (input SHA-256 hashes, signal statistics, pitch estimates),
`waveform.svg`, `spectrogram.png` with `spectrogram.svg`, `energy.svg`,
`pitch.svg` and an offline `index.html`. The per-block `diagnostics.csv` is not
committed because of its size; regenerate it with the commands below.

All four renders reported zero engine faults (no rejected events, Newton
failures, capacity overflows or containment resets). The largest energy-ledger
residual was below 4e-13 J against peak stored energies of 3–9 mJ. The pitch
estimates in `analysis.json` describe the mixed render, which contains
sympathetic and ringing strings; they are descriptive, not a tuning validation.

## Reproduce

The `source_revision` field of each `analysis.json` names the commit that
produced the render. From that commit, with a GCC 13 Release build on Linux:

```sh
cmake --preset headless-release
cmake --build --preset headless-release
for s in data/scores/*.score; do
  n=$(basename "$s" .score)
  build/headless-release/setar_play --score "$s" --output-dir "artifacts/demo/$n" --ledger --format pcm24
  python3 tools/analyze_render.py --input "artifacts/demo/$n" --output "artifacts/demo-report/$n" --source-revision "$(git rev-parse --short HEAD)"
done
```

Other compilers or platforms can change the last digits of the samples; compare
fault counters, ledger residuals and spectra, not bytes.
