# Numerical artifacts, real graphs, and reproducible reports

## Artifact identity before interpretation

A WAV, CSV, plot, or screenshot is not self-explanatory evidence. Its usefulness
depends on knowing what generated it, what the quantities mean, which inputs were
used, and which conclusion it can support. The reference renderer and report
generator therefore preserve model identity and the `synthetic_not_measured_setar`
label in machine-readable metadata and visible report text.

The baseline report is a numerical inspection artifact. It is not a screenshot
of a completed instrument application, a simulated measurement session, a
marketing illustration, or a benchmark result. A browser capture of the report
is a genuine screenshot of that rendered report, but it must still be labeled
as such. The origin of the pixels and the origin of the underlying data are
different provenance questions.

Use a dedicated output directory for each bounded experiment. State its input
question before generating it. Examples include whether lossless modal energy
remains within tolerance, whether the stiffness-induced frequency shift matches
the characteristic equation, or whether two block partitions produce identical
observations. A directory of attractive plots with no falsifiable question is
not a substitute for an experiment record.

## Renderer output contract

The renderer writes four files after validating parameters, rendering all
samples, and checking finite values and fixed-gain PCM headroom:

| File | Meaning | What it is not |
| --- | --- | --- |
| `render.wav` | Mono PCM16 representation of one displacement observation at fixed gain | Calibrated microphone pressure or radiated sound power |
| `samples.csv` | Every exported observation time, displacement, PCM level, and modal energy | A hidden decimated or normalized signal |
| `modes.csv` | Retained modal frequencies, damping, mass, and initial coordinates | Experimental modal identification of a specimen |
| `metadata.json` | Model/input/output manifest with units embedded in field names | An authenticated source-to-binary attestation |

Known existing output files are never overwritten. The renderer can use an
existing directory that does not contain those names, but callers should prefer
an empty experiment directory. This avoids confusing unrelated files with the
renderer contract. The report generator later uses only its expected inputs,
not whatever other material happens to be adjacent.

The output sequence is not a transaction across an entire file system. A disk
full error, interrupted process, or permission error can leave partial files.
Metadata is written after the data files, but its presence alone does not prove
that every byte is correct. The report validates content and hashes the inputs.
Do not reuse a partial directory merely because the filename exists; identify
the failed experiment and choose a fresh destination after fixing the cause.

## Sample table schema

`samples.csv` has this exact header:

```text
sample_index,time_s,displacement_m,pcm_linear,energy_j
```

The index starts at zero and increases by one. Time is index divided by sample
rate. Displacement is a signed point observation in meters. `pcm_linear` is
displacement multiplied by the explicit gain in inverse meters. `energy_j` is the
sum of the retained modes' mechanical energies before advancing the state for
that sample. There is one row per WAV frame.

The table uses a dot decimal separator, sufficient precision to round-trip the
exported doubles, and newline-delimited rows. The C++ parser and streams do not
inherit an arbitrary user-interface decimal convention. The Python reader
requires the expected columns and finite numeric values, checks contiguous
indices and timestamps, verifies fixed gain, and checks nonnegative passive
energy with an explicit rounding allowance.

`requested_duration_s` can be a non-integer number of sample intervals. The
renderer uses the floor of duration times rate as its sample count and rejects
a request that produces no samples. The final row is at (count − 1)/rate, not at
count/rate. Preserve this convention when importing the data into another
analysis tool; shifting time by one sample can create an apparent phase error
that is entirely an indexing mismatch.

The CSV contains the physical double-precision observation before PCM
quantization. Comparing a parsed WAV with this column therefore requires the
documented gain and quantizer. Treating the PCM integer as a physical displacement
or comparing it directly to energy mixes unrelated units.

## Modal table schema

`modes.csv` has this exact header:

```text
mode,frequency_hz,damped_frequency_hz,decay_per_s,modal_mass_kg,initial_displacement_m
```

The mode index begins at one. `frequency_hz` is the natural frequency of the
undamped mode. `damped_frequency_hz` is the oscillation frequency for the accepted
underdamped coefficient. `decay_per_s` is the σ in q̈ + 2σq̇ + ω²q = 0, not a dB
per second slope and not an energy decay coefficient without the factor two.
Modal mass corresponds to the unit-peak sine normalization. The initial
displacement is the signed finite-mode triangle coefficient.

The report independently recomputes these relationships from the input
parameters. It checks frequency, damped frequency, decay, modal mass, excitation,
and initial energy. It also samples the direct analytic time response at
deterministic locations in the exported sequence. These checks protect against
several schema and implementation errors, but they do not turn the chosen
physical assumptions into measured truth.

## WAV encoding and level policy

The WAV is RIFF/WAVE with one PCM channel, a 16-bit sample width, and the selected
integer sample rate. Header fields and sample words are written explicitly in
little-endian order. The quantizer maps the fixed-gain sample x to the nearest
integer to 32767x, using the C++ rounding rule whose halfway cases go away from
zero. This symmetric scale leaves the most negative 16-bit code unused.

No dither, automatic normalization, limiter, compressor, clipping, or perceptual
equalization is applied. This makes the mapping inspectable. It does not imply
that undithered PCM16 is the best eventual delivery format for an instrument or
measurement system. A future floating-point or higher-resolution export should
have an explicit format contract and independent decoder test.

The renderer rejects any non-finite sample, non-finite energy, or peak scaled
level above one before creating the output directory. A fixed-gain request that
would clip is a user-visible invalid input. Physical state is never reduced to
fit the file. If a state is unstable, changing a presentation gain cannot make
the physics acceptable.

Listening is useful for noticing gross discontinuities, unexpected silence,
oscillation, or file corruption. It is not a calibrated verification of energy,
dispersion, or setar identity. Keep playback volume conservative, especially when
experimenting with new parameters or new solvers. The numerical headroom check
does not measure the acoustic level produced by a listener's audio hardware.

## Report generation and deterministic processing

The Python tool uses only the standard library. It reads and verifies the actual
renderer output, creates three SVG plots, writes an offline HTML page, copies
the WAV and metadata into the report directory, and writes a sorted JSON
manifest. There is no network retrieval, generative image step, external chart
service, remote font, or browser dependency in that generation process.

The waveform graph includes every exported sample through 25 ms. The energy
graph uses deterministic uniform subsampling to keep the SVG manageable, while
the parser still checks every source row. The dispersion graph uses every
retained natural frequency. Its reference is the tension-only fundamental
multiplied by mode number, and its vertical value is the frequency excess in
cents. It is not the inharmonicity ratio relative to the already-stiff first
partial; those are distinct reference choices.

Subsampling is disclosed in `chart_processing`. It does not replace the raw
samples CSV. A plot intended to establish an extremum or catch a narrow spike
must use a rendering policy that preserves that feature, or show the raw values
separately. The current energy plot supports broad inspection while the
all-sample numeric passivity check governs the maximum positive-step result.

The generator records no wall-clock timestamp and no platform-specific absolute
input path in the report. Given identical input bytes, the same script, and the
same supplied source revision, output bytes are deterministic within the tested
runtime. Tests generate two reports and compare them. This avoids accidental
noise in reproducibility checks, but it also means experiment chronology belongs
in the surrounding experiment record rather than being inferred from report
content.

## Hashes and provenance boundaries

`manifest.json` includes SHA-256 hashes of all four source files and the generated
report assets except itself. It includes the model/evidence labels, caller-
supplied source revision, check results, numeric tolerances, processing policy,
and limitations. It does not embed copyrighted papers, external recordings, or
unlicensed measurements.

A hash establishes byte identity relative to a known digest. It does not show
that the bytes are physically correct, that a measurement was performed honestly,
that a particular compiler produced them, or that an input license permits
redistribution. Keep those questions separate. If a future report uses measured
data, it needs specimen and acquisition identifiers, calibration, consent and
rights records, uncertainty, and an audit trail in addition to hashes.

The `source_revision` argument is intentionally not guessed by inspecting a
nearby Git directory. A report can be generated from copied data, an installed
renderer, or a dirty research tree. The caller is responsible for its provenance
statement. CI can supply the event commit because its checkout/build sequence
establishes a clearer relationship; a local experiment should retain the exact
binary or a rebuildable source/compiler record when that distinction matters.

## Genuine screenshot procedure

A screenshot should be captured from the actual generated HTML or actual
compiled GUI, not drawn to resemble one. For a report capture, first complete the
numeric checks, open its `index.html` with local assets available, choose and
record a viewport, wait for the document and images to finish loading, and
capture the rendered page. Inspect the resulting image at normal reading size.
Check that labels, units, warnings, legends, and numerical values are legible and
not clipped or covered by browser dialogs.

Retain the HTML and SVG sources along with the screenshot. The image is a useful
review surface; the underlying data and manifest remain the scientific record.
If a crop omits the synthetic warning, add an accompanying caption that preserves
the evidence label and model identity. A cropped waveform without provenance is
particularly easy to misrepresent when reused outside the repository.

For an optional JUCE screenshot, identify the actual operating system, JUCE
revision, build configuration, and running target. Do not claim that a report
capture is a plugin-host screenshot, or that a standalone window proves VST3
compatibility. A visual mockup may still be useful during design, but its caption
must say `mockup` and it must not be used as execution evidence.

## Review tasks and rejection rules

Before accepting a report, verify the model and evidence labels; compare the
input manifest with the intended experiment; check units and observation meaning;
inspect the numerical checks; inspect the real plots; verify that the source
revision statement is defensible; and retain a reproducible invocation. If the
report fails before output, preserve the failure reason instead of substituting
a plausible-looking graph.

Reject a report that uses silent normalization, mislabels synthetic inputs as
measured, claims an acoustic quantity while exporting displacement, changes the
truncation without acknowledging energy consequences, or points to a source
revision that does not contain the experiment. Also reject a screenshot whose
visible values cannot be reproduced from the retained data. A beautiful figure
is not a reason to weaken provenance.

The next useful artifact extension is one that answers a new question: a modal
reconstruction-error curve, a forced-response comparison, a work-balance trace,
or an actual calibrated measurement overlay with uncertainty. Add the raw data
schema and acceptance rule before styling the plot. That order keeps the report
an inspection tool rather than a source of unsupported confidence.
