# ADR-0003 — Treat parameter provenance as part of the model

Status: accepted policy. Synthetic renderer metadata implements a limited subset; specimen assets and a general registry remain proposed.

## Context

A physically dimensioned number can still be an assumption. Published tables can contain inconsistencies, and a fitted coefficient may compensate for uncertain geometry or support. Without evidence labels, plausible defaults can be mistaken for calibrated instrument data.

## Decision

Every distributable model identifies whether inputs are synthetic, published, measured, inferred, or effective. Preserve units, conventions, specimen/state identity where applicable, uncertainty, and validity limits. Keep raw evidence separate from corrected or fitted hypotheses.

Reject inadmissible tensors, missing unit conventions, and malformed assets rather than silently correcting them. A deliberate transformation creates a new traceable hypothesis.

Recordings may provide measurements and validation under permission; prerecorded note playback is not the physical synthesis engine.

## Alternatives and consequences

A plain preset bank is simpler but loses the distinction between evidence and taste. Restricting the project to fully measured quantities would prevent useful synthetic verification. The chosen policy permits both while keeping their claims separate.

Metadata adds storage and tooling requirements. Runtime assets need not redistribute private raw records; they retain authorized identifiers and enough provenance to locate the supporting evidence.

## Acceptance and reconsideration

Reports must retain evidence labels when exported independently. Parameter fitting requires held-out predictions and an identifiability assessment, not only a low training residual. Hashes establish identity, not calibration quality or permission.

Reconsider the representation when a new measurement or modeling method requires additional provenance, but do not remove evidence distinctions to simplify a user interface.

Related: [structural identifiability](../body/09-parameter-identifiability-and-experiment-design.md) and [solver evidence packages](../acoustics/06-solver-exchange-and-evidence-packages.md).
