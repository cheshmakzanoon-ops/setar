# ADR-0004 — Require explicit dependency and license choices

Status: accepted project policy. Current optional JUCE configuration follows this policy.

## Context

A successful download or build does not establish permission to distribute a dependency, a dataset, or a derived asset. Automatic configuration-time fetching can also make a supposedly reproducible experiment depend on changing network state.

## Decision

Do not choose a commercial entitlement, project license, or redistribution permission automatically. Identify the exact dependency revision and ask the responsible maintainer to approve its intended use when required. Retain that choice with the build or asset record.

The current JUCE option accepts an explicit existing checkout and does not download it. Headless numerical work remains independent of that choice. Apply the same discipline to future solver libraries, model assets, recordings, fonts, and plugin packaging dependencies.

## Alternatives and consequences

Automatic fetching is convenient, but convenience does not resolve revision, license, offline, or supply-chain questions. Vendoring everything can improve availability while creating maintenance and redistribution obligations. Either approach needs an explicit reviewed policy before adoption.

This decision does not prohibit package managers or open-source tools. It prohibits treating availability as authorization and silently selecting terms for the user. It is a project workflow rule, not a legal opinion about a particular license.

## Acceptance and reconsideration

Dependency records identify source, revision, purpose, supported platforms, and applicable notices or review status. Builds must fail clearly when an optional dependency is absent rather than substituting an unrelated implementation.

Reconsider fetching or vendoring only with pinned identities, integrity checks, an offline strategy, and a reviewed distribution plan. Never broaden repository credentials or publishing authority merely to make a dependency step pass.

Related: [JUCE dependency boundary](../engineering/juce-boundary.md) and [asset preparation](../architecture/03-evidence-assets-and-scheduling-boundaries.md).
