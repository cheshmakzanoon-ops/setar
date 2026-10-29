# Reproducible visual evidence

This directory contains **actual numerical graphs** from the synthetic reference
laboratory and a visibly identified **proposed** user-interface wireframe. It
does not present a measured or validated setar instrument. Each SVG graph is
calculated from the C++ renderer's exported CSV and checked by
`tools/generate_report.py`. The PNG companions are raster exports of those
SVGs; they are not screenshots of a running audio plugin.

The [waveform](waveform.svg) shows string displacement in millimeters;
the [energy plot](energy.svg) shows mechanical energy of retained modes;
the [dispersion plot](dispersion.svg) shows predicted excess cents relative to
an ideal tension-only harmonic series. These are **not** measured spectra or
microphone-pressure predictions. All numeric and visual paths remain entirely
headless and reproducible.

Each graph also has an inspected PNG export:
[waveform](waveform.png), [energy](energy.png), and
[dispersion](dispersion.png). The [musician design rendering](musician-wireframe.png)
is a rasterization of the [proposed UX SVG](../ux/assets/musician-wireframe.svg).
It is **not** an implemented JUCE/plugin editor, a captured app screenshot, or
evidence of live controls.

## Passive-loss verification graphs

[`loss-study/`](loss-study/) holds the **actual** graphs, CSV tables and provenance of the synthetic
loss-law study described in the
[damped-midpoint loss chapter](../numerics/14-damped-midpoint-loss-verification.md):
[decay by mode](loss-study/decay-by-mode.svg), [decay-error convergence](loss-study/decay-convergence.svg)
and [waveform error against the analytic modal reference](loss-study/waveform-convergence.svg), each with a PNG
raster export made with headless Chromium from the committed SVG (not a screenshot of any application).
They were produced by a Linux GCC 13 Release build of the commit recorded in
[`loss-evidence.json`](loss-study/loss-evidence.json) (`source_revision`), together with the SHA-256 of
both CSVs. The loss magnitudes are arbitrary synthetic values; nothing here is measured setar data.
Different compilers can change the last digits of the CSVs; compare orders and ratios, not digits.

```sh
setar_loss_study decay > decay.csv
setar_loss_study waveform > waveform.csv
python3 tools/generate_loss_report.py --decay decay.csv --waveform waveform.csv --output artifacts/loss --source-revision <actual-revision>
```

## Regenerate a source report

The default C++ and Python build/test instructions are in
[engineering](../engineering/README.md). After building the executable:

```sh
setar_render --output-dir artifacts/reference --duration 2 --sample-rate 48000
python3 tools/generate_report.py --input artifacts/reference --output artifacts/report --source-revision <actual-revision>
```

`<actual-revision>` means the verified revision of the built executable, not a
placeholder that should be copied literally into a published manifest. The
renderer and report intentionally reject overwriting existing output files;
use a new report directory for each run rather than deleting old evidence.

## Capture an actual report screenshot in a GUI-capable environment

Install Playwright and its Chromium browser separately in a controlled tooling
environment. The numerical core and all three platform headless CI jobs do
*not* need them. To create a report screenshot from actual generated HTML:

```sh
python3 tools/capture_report.py --input artifacts/report/index.html --output docs/assets/reference-report.png
```

For the clearly identified design wireframe:

```sh
python3 tools/capture_report.py --input docs/ux/assets/musician-wireframe.svg --output docs/assets/musician-wireframe-browser.png
```

The capture script accepts an explicitly installed Chromium executable with
`--browser-executable`. It disables page JavaScript and external requests and
refuses to overwrite prior captures/manifests. A Chromium screenshot is **not
included in this initial evidence set** because the available browser runtime
could not launch under its system restrictions. Do not relabel an SVG raster
export as a browser screenshot. Reproduction can yield pixel differences with
different Chromium/font versions; compare semantics, labels, dimensions, and
source hashes before treating a pixel diff as a physics bug.

The screenshot scripts are optional visual QA. C++ mathematical/energy tests,
the independent Python report checker, and remote three-platform CI are the
executable evidence; screenshots supplement them but cannot replace them.

## Exact vector-to-raster transformations

The committed PNG exports were produced from the committed SVGs using the
headless Inkscape command-line renderer, with no post-capture image editing:

```sh
inkscape docs/assets/waveform.svg --export-filename=docs/assets/waveform.png
inkscape docs/assets/energy.svg --export-filename=docs/assets/energy.png
inkscape docs/assets/dispersion.svg --export-filename=docs/assets/dispersion.png
inkscape docs/ux/assets/musician-wireframe.svg --export-filename=docs/assets/musician-wireframe.png
```

Use new output paths for independent reproduction. The source SVGs are the
authoritative numeric plots; pixel differences between renderers do not change
their physical data.
