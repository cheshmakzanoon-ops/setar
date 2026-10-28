# Implement one numerical component with independent evidence

Use this prompt for a bounded implementation task. Fill in the requested
component, owned paths, supported physical range, and selected equation variant
before starting. Do not turn a one-component task into an unreviewed instrument
rewrite.

## Task prompt

Implement [component] in the setar repository, modifying only [owned paths].
Read AGENTS.md, README.md, docs/START_HERE.md if present, the relevant master
phase in docs/ROADMAP.md, and the mathematical/numerical chapters named below.
If a required document is missing, report that fact and use the explicit task
contract rather than inventing its content.

The engine baseline is C++20. Keep the mechanical core independent of JUCE.
Use SI at physical-model boundaries. Do not add sample playback, unexplained
audio coloration, random noise to hide numerical problems, state-energy
normalization, or silent clipping. Do not claim setar fidelity from a generic
component demonstration.

Before editing, inspect the actual source and tests, identify existing user
changes, and state the exact equation variant and state-time convention you
will implement. List inputs, outputs, units, boundary conditions, force signs,
energy, parameter validity range, and failure behavior. Preserve unrelated work.

Choose exactly one of the following implementation slices unless the task
explicitly requests more:

### Slice A: explicit stiff-string preparation and stepping

Use docs/numerics/01-stiff-string-fdtd.md without changing the time placement of
the loss terms. Implement a checked preparation path, actual spacing \(h=L/N\),
strict stability margin, supported minimum grid, compatible simply supported
boundary rows, and preallocated previous/current/next storage.

Use the backward-gradient-loss scheme only if that is the selected variant.
Its h minimum includes the \(4\sigma_1k\) term, and its complete temporal order
is generally one when \(\sigma_1>0\). Do not advertise it as globally second
order. If implementing centered gradient loss instead, derive and test its
future-state solve and different energy criterion as a separate variant.

Provide an independent small-matrix reference, a modal amplification test, a
force-normalization test, an energy-balance test, and temporal refinement with
both zero and nonzero gradient loss. Include boundary and minimal-grid tests.
Validate coefficients before entering the callback.

### Slice B: scalar discrete-gradient contact

Use docs/mathematics/05-contact-potentials-and-solvers.md and
docs/numerics/09-nonlinear-algorithms.md. Implement stable potential divided
differences, the exact scalar residual, the analytic bracket where its assumptions
apply, safeguarded Newton/bisection, finite iteration bounds, and structured
failure results.

Do not infer multi-contact convergence from the scalar proof. Test separation,
compression, onset crossing, release crossing, equal-endpoint limits, several
exponents, large supported stiffness, and invalid inputs. Compare against an
independent high-accuracy bracketed reference and potential quadrature. Translate
solver residual into an energy-defect diagnostic.

### Slice C: collocated linear coupling

Use docs/mathematics/03-energy-and-passive-ports.md and
docs/numerics/02-boundaries-and-coupling.md. Implement one shared force unknown
between two prepared components with consistent port transforms and synchronized
midpoint velocities. Preserve the transpose relationship between observation
and force injection.

Start with two masses, then a string and a simple passive body. Check equal
and opposite force, velocity compatibility, internal work cancellation, and
coordinate rotation. If either component uses a different integrator, stop and
derive the joint work relation before connecting convenient sample values.

## Implementation process

First write or identify independent expected results. Then implement the smallest
correct reference path. Only after it passes should you add an optimized stencil,
SIMD path, low-rank solve, or prepared coefficient cache. Compare the optimized
path with the reference in physical quantities.

Keep allocation, locks, I/O, and unbounded work out of audio processing. Separate
immutable preparation data from mutable state. If a step can fail, preserve the
last accepted state and report a bounded diagnostic. A product mute or fade may
contain a fault, but the render must remain marked invalid.

Treat all numerical thresholds as proposed design budgets unless they follow
from a derivation. Do not loosen tests merely to pass. Investigate the scale,
conditioning, and reference accuracy first. Do not introduce a magic epsilon
into physical matrices without explaining the mechanical element it represents.

## Required handoff

Return the changed file list, the exact implemented equations, actual test
commands and results, measured convergence or error tables, and remaining
limitations. Distinguish compiled, executed, and untested platforms. State which
models or performance gestures are still absent.

Do not commit or push unless the coordinator or user explicitly authorizes that
operation. Do not present pseudocode as completed implementation. End by naming
the next evidence-gated task, not by promising commercial parity or a schedule.
