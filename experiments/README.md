# Reproducible experiments

An experiment connects a falsifiable question to observations and a decision.
This directory is for versioned experiment definitions and rights-cleared small
manifests, not an unreviewed collection of recordings. Use the
[experiment template](../templates/experiment.md) and the
[research gates](../docs/research/reproducibility-and-stop-gates.md).

## Three experiment classes

| Class | What it can establish | What it cannot establish alone |
| --- | --- | --- |
| Synthetic verification | Equation implementation, invariants, convergence, parser and export behavior | Fidelity to a measured setar |
| Physical identification | Parameters or responses for a documented specimen and condition | Generalization beyond that evidence |
| Independent validation | Predictions on data not used for the relevant design decisions | Exact replication of every possible instrument state |

Label mixed experiments by quantity. A measured outer scan with inferred
thickness is not an entirely measured model. A source-derived prior is published
evidence, not a project measurement. An effective fitted parameter is not
automatically a directly observed material constant.

## First runnable synthetic experiment

The current CMake project implements `setar_render` and an offline report
generator. Reproduce the baseline first:

```sh
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug
```

For a fresh output location, the implemented command sequence is:

```sh
build/headless-debug/setar_render --output-dir artifacts/experiment-example --duration 2 --sample-rate 48000
python3 tools/generate_report.py --input artifacts/experiment-example --output artifacts/experiment-example-report --source-revision working-tree-experiment
```

The renderer creates `render.wav`, `samples.csv`, `modes.csv`, and
`metadata.json`. The report includes actual generated SVG/HTML and provenance.
Inspect the generated `index.html` rather than treating successful file creation
as visual QA. The output is a synthetic displacement observation with explicit
gain, not measured acoustic pressure or a validated setar recording.

Choose another exact directory for another run. The tools refuse to overwrite
their known outputs. Do not resolve that refusal through broad recursive deletion.
The example source-revision label is intentionally a working-tree label; a clean
committed experiment should record its actual revision and build relationship.
The argument alone does not prove which source produced the executable.

On Windows or another generator, use the actual executable path described in
[engineering setup](../docs/engineering/README.md), including configuration
subdirectories and the executable suffix where applicable. Do not assume a Unix
path proves the Windows target was tested.

## Definition packet

Commit the question, input manifest, parameter definitions, method, uncertainty,
acceptance rule, and reproduction commands when appropriate. Keep generated
outputs in the designated artifact storage unless a small approved fixture is
needed by a test. Record hashes and stable identifiers so a result can be linked
to exact inputs without embedding large restricted files in Git.

An experiment definition should specify controls, repetition units, exclusions,
and the result that would reject the proposed explanation. Define these before
opening independent test data. If a failed test changes the model, retain the
failure and reclassify reused data as development evidence.

## Real measurements

Follow the [measurement volume](../docs/measurement/README.md) before acquisition.
Required records include specimen/configuration, handling authorization, channel
units, calibration, timing, environmental state, quality flags, and uncertainty.
Participant data additionally require the
[consent and rights workflow](../docs/ethnomusicology/participants-consent-and-rights.md).
Do not substitute internet recordings or guessed measurements for missing
authorized evidence.

Raw data stay immutable. Every correction, filter, window, registration, fit,
and normalization creates a documented derivative. Preserve unsuccessful trials
and reasons where permitted. A failed measurement can diagnose a protocol problem
without becoming valid calibration evidence.

## Review and acceptance

Have a second analyst reproduce one decisive result from the packet without
private instructions. Include an invalid fixture to confirm the pipeline can
reject bad data. Report actual commands, observed outcomes, uncertainty, and
unrun checks. The documentation audit checks links and structure; it does not
validate an experiment's physics.

An accepted packet states which claim passed, which failed, and what remains
unknown. Its next action addresses the dominant uncertainty or failure mechanism,
not simply a larger dataset or a more attractive plot.

