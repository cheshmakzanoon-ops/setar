# Developer prompt index

Choose a prompt by the next unresolved evidence gate, not by the most appealing
feature name. Read [AGENTS.md](../AGENTS.md),
[the orientation](../docs/START_HERE.md), and the relevant phase first. Prompts
are scoped work requests; they do not grant new authority for external actions,
instrument interventions, participant contact, publication, or licensing choices.

| Prompt collection | Use when | Expected handoff |
| --- | --- | --- |
| [Research and measurement](research/README.md) | Sources, specimens, calibration, data, tuning evidence, rights, independent tests | Traceable evidence or a precise blocker |
| [Numerics](numerics/README.md) | Equations, discretization, stability, energy, convergence, solver diagnosis | Independent numerical verification |
| [Performance and contact](performance/README.md) | Persistent strings, fingernail/fret interaction, gestures, playability | Physical event/state contract and tested behavior |
| [Acoustics](acoustics/README.md) | Geometry, body, cavity, bridge coupling, radiation | Admissible coupled response and uncertainty |
| [Validation](validation/README.md) | Evidence pipelines, acoustic discrepancies, claim review | Scoped independent validation decision |
| [Plugin and real time](plugin/README.md) | Host lifecycle, events, state, bounded processing, host failures | Reproducible host and execution evidence |
| [Integration](integration.md) | Connecting accepted work across boundaries | Verified interfaces and an honest capability report |

## Starting a work request

Copy the chosen prompt and add a task identifier, exact owned files, relevant
revision, permitted data, and the acceptance question. Use the
[task template](../templates/task.md) for the resulting work record. If an input
is missing, require a blocker report or a clearly synthetic fixture; never ask
the developer to invent plausible measurements.

State whether the request is inspection, diagnosis, implementation, experiment,
or review. A diagnosis should not silently expand into a fix. A physical-model
implementation must not introduce recorded-note playback as a shortcut. A
working optional plugin boundary must not be described as a finished instrument.

## Completing a work request

Require changed files, actual commands and results, independent evidence,
remaining assumptions, unrun checks, and the next gate. The default runnable
baseline is documented in [CONTRIBUTING.md](../CONTRIBUTING.md); a prompt does
not replace it. Preserve failed attempts when they explain the decision and keep
cross-platform configuration distinct from observed platform success.

