# Implement one observable's evidence pipeline

## Task to give the engineer

Implement the smallest reproducible pipeline for `[observable and claim ID]`
using `[candidate model revision]`, `[reference/corpus manifest]`, and
`[allowed files]`. The intended result is `[verification or empirical comparison]`.
Do not expand this into a complete acoustic score or claim a finished setar.
Read AGENTS.md, the current capability register, the error-budget chapter, and
the observable's technical chapter before editing. Preserve unrelated user work.

Begin by reporting what actually exists: production entry point, reference
implementation, measurement availability, permission status, current tests, and
platform evidence. If the requested corpus is absent or unlicensed, implement
and test the synthetic analysis path only, and mark empirical validation blocked.
Do not generate plausible recordings or measured values.

## Required design decisions

Name the observable, units, observation location, sample/time convention, input
definition, conditioning, and feature-extraction algorithm. Explain the oracle's
independence from production code. Identify model discrepancy, measurement noise,
input uncertainty, discretization, rounding, and extractor uncertainty. Provide
a threshold record whose status is proposed unless existing evidence justifies
it. Do not infer a perceptual tolerance from numerical precision.

Choose synthetic fixtures that can falsify the extractor. For frequency, include
known drift and close inharmonic partials. For decay, include one exponential,
two-stage behavior, beating, noise-floor truncation, and unresolved modes. For
an attack, include a known event time and a controlled delay. Explain what result
the extractor should return when the requested feature cannot be resolved.

Keep raw inputs immutable and create versioned derivatives. Record transformations,
hashes, algorithm revision, parameters, and output identity. Do not hide a
normalization or alignment inside a plotting helper. If conditioning removes
information relevant to the claim, produce a separate clearly limited analysis.

## Implementation sequence

1. Reproduce the current headless baseline with the repository's actual CMake
   preset and test interface. Record the observed outcome, not an assumed pass.
2. Add independent synthetic expected values and failure fixtures before changing
   the extractor or renderer.
3. Implement the bounded analysis unit with clear invalid/unresolved status.
   Offline allocation is allowed; do not move analysis into the audio callback.
4. Export a machine-readable table and manifest. Generate plots from that table,
   with units, common axes, uncertainty, and synthetic/measured labels.
5. Add tests for malformed input, insufficient samples, nonfinite values, silence,
   clipping, and configuration/schema mismatch.
6. Run the relevant local suite and update cross-platform coverage. Inspect actual
   Linux GCC/Clang, Windows MSVC, and macOS Apple Clang results when available;
   name unavailable executions as pending.
7. Review the generated report visually and numerically. If a screenshot is used,
   capture the real report or application and identify which one it is.

## Acceptance evidence

Deliver changed-file summary, independent-oracle explanation, fixture manifest,
numeric results, uncertainty/threshold table, plots, test logs, platform matrix,
and unresolved cases. Show at least one deliberately invalid or unresolved
example so the pipeline's failure behavior is visible. A report that always
produces a confident number is not acceptable merely because its layout looks
polished.

If empirical data are available, respect grouped splits and do not inspect sealed
holdouts to tune the algorithm. Register any development decisions before final
evaluation. Report the exact claim supported and what remains untested. End with
the next informative experiment or implementation boundary, not a broad claim
that fidelity is solved.

## Review questions

Could two implementations share the same bug? Could alignment, normalization,
or a noise floor manufacture the reported improvement? Does the uncertainty
permit the claimed precision? Are repeated frames being treated as independent
measurements? Can another engineer regenerate the numeric table and image from
the manifest without guessing hidden defaults? Answer these explicitly in the
handoff.
