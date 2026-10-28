# Measurement laboratory volume

These chapters specify proposed acquisitions for a physical-modeling project. They
do not contain measured instrument parameters. Every numerical example must be
labeled synthetic unless it is linked to a real specimen, calibration, and raw
record. The first deliverable is a defensible measurement chain, not a large
folder of recordings.

| Chapter | Main responsibility | Gate before using its output |
| --- | --- | --- |
| [Reference instrument](reference-instrument.md) | Specimen identity and reversible configuration | Authorization and condition record |
| [Geometry and thickness](geometry-and-thickness.md) | Shape, coordinate registration, internal dimensions | Coverage and uncertainty map |
| [Strings, frets, and setup](strings-frets-and-setup.md) | String state, speaking length, fret geometry | Independent units and tuning checks |
| [Materials, joints, environment](materials-joints-and-environment.md) | Constitutive evidence and changing state | Admissibility and intervention approval |
| [FRF and modal protocol](frf-and-modal-protocol.md) | Calibrated mechanical input-output data | Loading, linearity, coherence, repeatability |
| [Radiation and acoustic capture](radiation-and-acoustic-capture.md) | Pressure transfer and listening references | Acoustic calibration and room validity |
| [Pluck, contact, and motion](pluck-contact-and-motion.md) | Excitation and gesture observations | Synchronization and disturbance controls |
| [Data contracts and provenance](data-contracts-and-provenance.md) | Machine-readable interpretation | Schema, checksum, rights validation |
| [Uncertainty and calibration](uncertainty-and-calibration.md) | What the observations actually constrain | Complete uncertainty model |
| [Independent validation](independent-validation.md) | Predictive tests outside calibration | Frozen model and groupwise partitions |
| [Specimen completeness matrix](specimen-completeness-matrix.md) | All-domain digital-twin acquisition checklist and explicit unknowns | Actual specimen, rights and calibrated evidence |

Read the [research evidence ledger](../research/evidence-ledger.md) before treating
published procedures as established project practice. Read
[participant and rights guidance](../ethnomusicology/participants-consent-and-rights.md)
before involving musicians or distributing recordings. A public research article
does not authorize intervention on a privately owned instrument or release of a
performer's session.

## Minimum laboratory vocabulary

A **specimen** is the physical instrument. A **configuration** is its documented
assembly and setup state. A **session** groups acquisitions under a stated chain
of calibration and environmental observations. A **trial** is one controlled
excitation or performance event. A **channel** has a physical quantity, direction,
unit, calibration, and time base. A **derived asset** is an output of a named
transformation. None of these concepts can substitute for another.

Assign identifiers before acquisition and avoid putting personally identifying
information in filenames. A retune or moved bridge may create a new configuration;
a replacement sensor may create a new acquisition-chain version. The exact policy
is documented, but the effect must be recoverable. Do not rely on an engineer's
memory that “the last few takes used the other strings.”

## Start small and earn expansion

Demonstrate a complete pilot path: known reference input, calibrated channels,
immutable raw file, manifest, derived observable, uncertainty, and independent
reproduction. Then repeat it after remounting. Only expand to dense scanning or a
large gesture corpus when the pilot identifies the information the expansion will
add. More data cannot repair a wrong unit, missing force reference, or unrecorded
state change.

The laboratory must preserve rejected trials with reason flags when lawful and
practical. Do not use a failed acquisition as calibration evidence, but do retain
its diagnostic value. A safe instrument and an honest stop report are better
outcomes than a complete-looking dataset obtained outside authorization.
