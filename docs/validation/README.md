# Verification and validation handbook

This volume distinguishes mathematical verification, empirical validation,
listening evidence, and product qualification. Procedures are proposed engineering
contracts unless an identified execution packet establishes a narrower result.
A successful synthetic reference build does not establish measured-setar fidelity.

## Reading path

1. [Evidence contract](00-evidence-contract.md): claims, states, gates, and packets.
2. [Test taxonomy and error budgets](01-test-taxonomy-error-budgets.md): independent oracles and threshold status.
3. [Reference corpus](02-reference-corpus-provenance.md): permissions, provenance, grouped splits, and leakage.
4. [Acoustic metrics](03-acoustic-metrics.md): pitch, partials, spectra, attacks, decay, and dynamic response.
5. [Gestures and radiation](04-gesture-and-radiation-validation.md): physical transitions, coupling, and receiver geometry.
6. [Identifiability and calibration](05-identifiability-and-calibration.md): uncertainty, fitted meaning, and gradient pitfalls.
7. [Listening experiments](06-listening-experiments.md): preregistration, ABX limits, reference matching, and playability.
8. [Cross-platform CI](07-cross-platform-ci.md): compiler coverage, numerical reproducibility, sanitizers, and artifacts.
9. [Release evidence and triage](08-release-evidence-and-failure-triage.md): bounded quality claims and regression review.
10. [Final fidelity acceptance](09-final-fidelity-acceptance.md): preregistered acoustic, perceptual and performer studies with explicit failure/claim rules.

The implementation, review, and debugging entry points are in
[validation prompts](../../prompts/validation/README.md). Runtime and host claims
also require the [real-time](../realtime/README.md) and
[plugin](../plugin/README.md) volumes; screenshots and accessible interaction
require [UX evidence](../ux/README.md).

## Primary-source use

The chapters link primary sources at the claims they support. ITU-R BS.1116-3
and BS.1534-3 establish listening-method context, not a certification of the
project's adapted experiments. Raue et al. provides identifiability background;
the contact-gradient papers identify limitations to test, not ready-made setar
algorithms. Official GitHub, compiler, JUCE, and Steinberg documentation supplies
tool/API contracts. Pin software revisions before relying on behavior that can
change. Sources were checked during handbook authoring on 2026-10-28; a future
dependency update needs a fresh compatibility review.

Do not copy entire papers or standards into the repository. Preserve bibliographic
identity, accessed version, relevant section, and any access limitation. Project
procedures are original engineering designs and must be tested rather than
treated as established merely because a related source is cited.

## Starting evidence packet

Choose one claim, one independent oracle, one fixture, and one result. Record
what exists, what ran, and what remains proposed. If measurement data are absent,
run synthetic verification and mark empirical validation blocked. If a platform
is unavailable, retain portable tests and mark its execution unrun. This makes
the next engineer's task precise without inventing confidence.
