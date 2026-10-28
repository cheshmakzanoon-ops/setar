# ADR-0001 — Retain an offline reference alongside the real-time engine

Status: accepted design direction. The current synthetic reference is implemented; a full coupled offline/reference and reduced/runtime pair is proposed.

## Context

Research models need inspectable residuals, convergence studies, and alternative discretizations. Interactive audio needs bounded work and carefully prepared state. Making one implementation serve both without an explicit approximation contract can hide physics behind optimization or make the callback depend on unbounded scientific computation.

## Decision

Keep a reproducible reference path for each accepted physical mechanism and compare the real-time approximation against it. Share definitions and audited infrastructure where useful, but preserve enough independent calculation to detect errors rather than comparing a function with itself.

The existing setar::reference API remains a narrow synthetic baseline. Its exact free-response update is not promoted into a universal coupled solver. New force, contact, body, and radiation behavior receives its own reference fixtures and validity limits.

## Alternatives and consequences

Using only the production engine reduces code maintenance but weakens independent verification. Running the full offline model in every callback avoids reduction but can violate bounded processing. Both can be useful experimental configurations, but neither replaces the dual evidence path.

Maintaining two paths costs review effort and requires clear model identifiers. Their internal state dimensions need not match. Compare physical observables, energy exchange, and event semantics at common physical times.

## Acceptance and reconsideration

Require full-versus-reduced error reports, convergence evidence for the reference, and measured processing cost for the runtime. An offline result is not automatically truth; it also needs verification.

Reconsider a separate reference only for a mechanism whose production implementation is itself independently verifiable at adequate precision and scope. Record that argument explicitly rather than deleting the reference because maintenance is inconvenient.

Related: [runtime preparation boundary](../architecture/03-evidence-assets-and-scheduling-boundaries.md) and [passive reduction](../body/08-passive-reduction-and-residual-flexibility.md).
