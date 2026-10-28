# Implement one host-facing contract

## Scoped implementation request

Implement `[one contract: parameter mapping, state migration, event translation,
prepared-state exchange, lifecycle handling, or telemetry]` in `[allowed files]`
for `[supported formats/platforms]`. Read AGENTS.md, the actual CMake targets,
the relevant plugin chapter, and the callback/concurrency contracts. Keep the
C++20 physical core independent of JUCE. Do not turn this task into a complete
instrument or claim optional targets already built.

Inventory current behavior and tests first. Identify the pinned JUCE/SDK versions
and actual adapter entry points. Reproduce the headless baseline using the
repository's presets. An unavailable dependency is a blocker for that adapter,
not permission to download an arbitrary branch or replace the core with a
framework example oscillator.

## Contract before code

Write inputs, outputs, ownership, calling contexts, capacities, timing, failure
response, and compatibility obligations. For events, define identity and equal-
time order. For state, define active versus requested generation and installation
completion. For parameters, define stable ID, normalized mapping, units, and
transition semantics. For telemetry, define loss policy and stale-data meaning.

List the callback call graph changes. Explain where preparation, allocation,
destruction, parsing, locking, and I/O occur. None may be introduced into audio
processing. Check indirect destruction and framework helpers. An atomic pointer
swap does not by itself solve reclamation, and a try-lock is not a complete
ownership design.

## Independent tests

Create a simple reference state machine or expected-value model independent of
the optimized implementation. For state migration, retain real historical bytes
and expected meanings. For parameter mapping, test endpoints, round trips,
invalid values, text conversion, and old automation. For event translation,
test repeated pitches, channel reuse, simultaneous events, block boundaries,
overflow, and note-off preservation.

Add adversarial lifecycle tests: process without editor, editor cycles, repeated
prepare/release, variable and zero blocks, sample-rate changes, suspended
processing, canceled workers, and destruction with pending work. Include only
tests relevant to the selected contract, but explicitly identify adjacent cases
that remain outside this task.

Instrument callback allocation and deallocation. Test the detector with a known
unsafe operation in the harness so an inactive detector cannot produce a false
pass. Use memory/race/real-time sanitizers where supported in separate suitable
builds. Preserve tool limitations and do not use sanitized timing as production
performance evidence.

## Implementation and verification

Make the smallest change satisfying the contract. Reuse the canonical core API
instead of adding wrapper-specific physics. Keep errors bounded in processing
and actionable outside it. If state preparation is asynchronous, prove that
offline rendering cannot silently use the previous preset after a reported
successful restore.

Run local tests and update Linux GCC/Clang, Windows MSVC, and macOS Apple Clang
coverage. Inspect observed jobs and report unrun cells honestly. If an actual
plugin binary is available, run the approved relevant validator and preserve
the exact log and binary hash. A generic validator pass does not replace the
specific semantics tests.

For UI changes, assert numeric layout and authoritative parameter state, then
capture the actual deterministic editor/report with a manifest. Do not use a
wireframe as implementation evidence. Opening diagnostics must not alter the
canonical audio render or introduce callback allocation.

## Handoff

Provide changed files, contract, ownership/capacity rationale, independent tests,
logs, performance impact where relevant, screenshots where genuine, and a
capability matrix separating implemented, locally run, cross-platform observed,
host-qualified, and deferred. Name any compatibility break and migration policy.
Do not sign, publish, install commercial hosts, or change repository protections
as an implicit continuation. End with the smallest remaining integration check
needed to support the declared contract.
