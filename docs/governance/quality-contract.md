# Quality contract: claims, evidence, and acceptance

## Purpose

This document defines the rules used to judge work in this repository. It applies
to scientific prose, numerical code, presets, tests, interfaces, diagrams, and
release claims. It prevents two common mistakes: treating a plausible instrument
sound as proof of correct physics, and treating mathematically careful code as
proof that a particular real instrument has been recreated.

The project requires both verification and validation. Verification asks whether
we solved the stated mathematical problem correctly. Validation asks whether that
problem represents the observed instrument sufficiently well for a stated use.
Neither can be inferred from the other. Successful CI demonstrates only the
assertions actually implemented by its jobs.

## Claim levels

### Q0: design proposal

The interface, equation, method, or experiment is specified but not implemented.
Its usefulness can be reviewed, but its numerical or acoustic behavior is not
established. Most of the handbook initially belongs here. A polished diagram
does not move a proposal to another level.

### Q1: executable synthetic reference

An implementation exists and runs on defined synthetic inputs. Expected behavior
comes from an independent calculation or analytic result. Inputs are labeled
synthetic, and output units and conventions are explicit. This is the level of
the initial reference scaffold. It is a valuable testing tool, not a setar model
validated against a real specimen.

### Q2: numerically verified component

The component meets its stated accuracy, stability, and energy conditions across
its declared parameter domain. Verification includes failure inputs and numerical
resolution changes, not only one successful plot. A finite-difference scheme's
stability bound must match its exact stencil, damping discretization, and
boundaries. A modal model's output normalization must be derivable from its modal
coordinates and observation vector.

### Q3: independently validated component

The component's predictions agree with measurements not used to fit its
parameters, subject to stated error budgets and uncertainty. If only resonance
frequencies were measured, do not claim amplitude or radiation validation. If a
bridge was clamped in a fixture, do not claim free-playing boundary behavior.
Validation statements must include the specimen, setup, parameter range, and
observation space.

### Q4: integrated instrument qualification

The coupled instrument meets component, interaction, performance-control,
real-time, platform, and host criteria for a defined release domain. Independent
listeners and practitioners evaluate the relevant capabilities. Complete files,
source versions, calibration data permissions, and build dependencies are
traceable. Uncovered gestures remain explicit exclusions.

These labels do not imply a timeline. They are distinct evidence conditions.

## Acceptance-budget policy

Numerical thresholds in this repository fall into three categories: derived,
measurement-limited, and proposed. Derived thresholds follow an equation or a
known invariant. Measurement-limited thresholds follow calibration uncertainty,
repeatability, or an uncertainty budget. Proposed thresholds are engineering
choices that require justification and may be revised through a decision record.

Examples of inappropriate practice include inventing a universal 0.1% audible
accuracy target, demanding bit-identical floating-point results across unrelated
libraries, or accepting any error smaller than the previous implementation's
error. A tolerance must correspond to the quantity being tested, its scale,
its expected numerical behavior, and the claim being made.

Choose acceptance metrics before seeing the candidate's results. Report absolute
and relative errors where each is well-conditioned. Near a true zero, a relative
error can be meaningless. A phase-error metric must specify unwrapping and the
frequency range. A spectral comparison must specify window, duration, alignment,
level normalization, and noise floor. A runtime measurement must identify hardware,
compiler, optimization flags, block size, warm-up, and worst-case work configuration.

## Required evidence packet

Each significant physics or DSP change supplies:

1. A falsifiable question and linked requirement/task identifier.
2. Source commit and relevant build/compiler/dependency versions.
3. Input manifest with units, provenance, random seed where used, and data hashes.
4. Exact commands and environment assumptions required to reproduce the result.
5. An independent expected result or justified comparator.
6. Machine-readable output and human-readable plots with units and limitations.
7. Numerical/experimental uncertainty and reasons for chosen tolerances.
8. Pass/fail summary that does not hide failed subcases in averages.
9. A short interpretation and the conditions under which it does not apply.
10. Reviewer disposition and any changed risk or assumption.

Not every cosmetic edit needs all ten artifacts. The author must explain why
an omitted category is irrelevant rather than fill it with meaningless prose.
The PR template helps reviewers make that decision.

## Independent tests

A test that repeats the implementation's formula with renamed variables can
miss the same error twice. Prefer analytic limiting cases, a separately derived
method, symmetry/reciprocity, dimensional checks, exact oscillator solutions,
energy identities, manufactured forcing, or comparison to an independently
converged solver. For measurements, preserve raw force and response channels so
the derived FRF can be recomputed independently.

Golden files are useful for regression, but they do not establish correctness.
A bad algorithm can reproduce a bad golden file forever. Every scientific golden
fixture must have an independent provenance explanation and a defined update
procedure. Never approve a new golden output just because a failing test would
otherwise block the change.

## Numerical failure policy

NaN, infinity, a non-converged nonlinear solve, negative dissipation where the
model requires positive dissipation, or an inadmissible material matrix must
produce an explicit failure in the reference laboratory. Do not normalize the
output, add random noise, or clip the state to make it appear plausible.

An interactive instrument also needs a bounded audio-safety response. That is
a separate containment mechanism: flag the failure, safely silence or reset the
affected state using a documented transition, and retain diagnostic evidence
outside the callback. Containment is not a successful physical solution. A fault
counter must never be interpreted as an ordinary musical control.

## Review roles

Numerical review checks equations, schemes, discretization, invariants, and
convergence. Acoustic review checks specimen assumptions, measurement controls,
coupling, observation, and perceptual interpretation. Software review checks
ownership, real-time safety, portability, interface contracts, and maintainability.
Practitioner review checks idiomatic gestures, tuning, response, and musical use.
One person may cover multiple roles, but the review record should say which
perspectives were actually supplied.

AI assistance may draft code, derive candidate equations, propose tests, or find
sources. It is not a substitute for independent validation. Reviewers must be
able to inspect original sources and reproduce the evidence without trusting the
assistant's authority. Do not cite a chat transcript as the source of a measured
physical constant.

## Visual evidence

Plots are scientific outputs. A graph must identify axes, units, normalization,
input state, and whether its data are synthetic or measured. A screenshot must
identify what executable or report was rendered and how to reproduce that state.
Mockups belong in design discussions and must be labeled. Do not use generated
instrument imagery as evidence that a GUI exists or that the physical model works.

Visual regression tests should control viewport, fonts, locale, theme, device
scale, deterministic state, and animations. Keep structural/layout assertions
separate from pixel-difference tolerances. Platform font rasterization differences
are not acoustic regressions. Audio/model errors must not be hidden by refreshing
an image baseline.

## Scope of green CI

All introduced executable engineering work must have an appropriate test path on
Linux, Windows, and macOS. Some tools or sanitizers apply only to a subset; explain
the complement, do not label skipped work as passed. Licensed DAWs, physical MIDI
devices, audio-interface timing, signing identities, and human listening studies
need separate authorized evidence. GitHub-hosted compilation cannot certify
hardware latency or performer satisfaction.

Required-check policies are repository administration choices. Workflows can be
committed without changing branch protection or secrets. If the owner later
enables a required check, ensure the check name is stable and cannot be
accidentally bypassed by path filtering. Do not auto-approve PRs based only on
the documentation count.
