# Optional physiological controls and accessible performance mappings

## Optional means the instrument works without them

Motion capture, force sensing, wearable controllers, or other physiological
signals can provide expressive input. They should be optional adapters, not
prerequisites for playing the model or for validating its core mechanics. A
keyboard, mouse, accessible switch, or scripted event source should be able to
address the same core event contract under an appropriate mapping.

This chapter is an engineering and consent protocol, not medical guidance. The
project must not infer health status, diagnose a condition, or promise therapeutic
benefit from performance signals. A signal that correlates with a gesture under
one calibration does not necessarily measure force, fatigue, intention, or
anatomy directly.

Inputs are an explicitly selected sensor, participant permission, calibration
records, and a desired expressive control. Outputs are a bounded adapter,
uncertainty and latency characterization, accessible alternatives, and a clear
statement of what the signal does and does not establish.

## Distinguish observed signals from physical quantities

A sensor can report a voltage, position estimate, acceleration, orientation, or
other device-specific value. Converting that value into newtons, metres, or a
contact trajectory requires a model and calibration. Keep raw observations,
calibrated estimates, and musical commands as separate data layers.

For example, a pressure-sensitive controller surface can support a useful
normalized finger-load intent without measuring the force a setar finger would
apply. That is a legitimate mapping. Calling the value actual finger force
would require a different evidential claim. The core should receive either a
clearly normalized intent or a calibrated physical quantity, never an ambiguous
float that changes meaning by device.

Orientation sensing also requires a reference frame. A wearable's axes are not
the instrument's axes until calibrated. Record the transform and its uncertainty.
If the device shifts on the hand, the transform can become stale. Provide a
recalibration or safe fallback rather than silently interpreting the new frame as
a changed playing style.

## Calibration as a reproducible task

Choose a small set of comfortable, voluntary calibration actions relevant to the
control. Do not request maximal force, painful motion, or medically meaningful
tests. The participant can stop or decline without losing access to ordinary
controls. The calibration should support the instrument interface, not collect
unnecessary personal information.

Measure repeatability, drift, saturation, and the mapping's sensitivity to device
placement. Retain uncertainty rather than fitting an exact curve through a few
points. If a mapping is person-specific, label it as such and do not generalize
it to other users. A default synthetic mapping can remain available with clear
status.

Keep calibration assets outside the audio callback. The rendering path receives
validated transforms and bounded coefficients. Replacing a calibration while
playing is a configuration transition: define interpolation or an explicit
reset of controller state so the change does not create a sudden physical
command.

## Latency, jitter, and missing data

Sensor transport can add latency and irregular arrival times. Timestamp the
observation at the best-supported point and document clock synchronization.
Arrival time at the audio callback is not necessarily the time of the physical
gesture. If device timestamps are unavailable, state the resulting uncertainty.

Use a bounded reconstruction policy. Interpolation, prediction, and smoothing
trade responsiveness against noise and phase delay. Test them on recorded or
synthetic trajectories with known timing before using them to drive stiff
contacts. Differentiating noisy position into velocity can create large
spurious forces.

Missing data needs a safe, musically understandable policy. Holding the last
position, relaxing toward a neutral trajectory, releasing an owned contact, and
resetting the instrument are different choices. Select one by adapter mode and
record it. A disconnected sensor should not leave an indefinitely advancing nail
trajectory or unbounded force command.

## Physiological inference is not gesture truth

Some signals are only indirect correlates of muscle activity or motion. Their
relationship to an intended gesture can change with placement, posture, effort,
and context. Treat inferred intent as uncertain. A classifier confidence is not
the same as a probability that a particular physical force occurred.

Use a fallback that lets the performer override or disable inference. If a
classifier selects a stroke, expose the selected action in a trace or interface.
Do not silently adapt the person's physical profile in ways that make the same
gesture unpredictable. Adaptive mappings need their own versioning and
reproducibility policy.

Avoid attaching cultural technique labels solely from an unvalidated sensor
classifier. A recognized movement pattern may not establish the musical context
or intent that gives a technique its meaning. Practitioner review and the
gesture glossary remain necessary.

## Privacy and data minimization

Collect only data needed for the selected experiment or control. Live expressive
use need not imply persistent recording. If recordings are useful for analysis,
obtain separate permission and state retention, access, and redistribution
conditions. Do not put raw personal sensor streams into a public repository by
default.

Remove unnecessary identifiers from shared fixtures. Synthetic trajectories can
often exercise numerical behavior without retaining personal data. If a real
trajectory is necessary, preserve its permission record and avoid implying that
de-identification guarantees anonymity under every future use.

The model should not infer or store sensitive attributes that are unrelated to
the task. A controller profile can be called a user's preferred mapping without
describing their health, disability, or anatomy. Accessibility settings should
be offered as ordinary choices, not as diagnoses.

## Accessible equivalence

Offer alternate ways to control the same expressive dimensions: separate stroke
buttons, a continuous gesture pad, automation lanes, or a keyboard mapping.
Equivalence means comparable access to the intended musical control, not that
every device reproduces the same biomechanics. Preserve the distinction in
documentation while making the instrument usable.

Allow independent adjustment of control range, sensitivity, inversion, and
gesture triggering where supported. A narrow comfortable input range can map to
the full admissible physical range without requiring large motion. The mapping
must still respect prepared numerical limits and avoid surprise discontinuities.

Do not make optional sensor failure disable basic note input. The adapter can
release its own ownership and leave other sources active. Tests should verify
that a failed wearable stream does not clear keyboard-owned contacts or corrupt
the four-string state.

## Verification and acceptance

Use synthetic sensor streams to test calibration, unit conversion, frame
transforms, jitter, missing packets, saturation, and clock discontinuities.
Compare the resulting core events with a direct script. Then test the real
device under the same documented conditions and report differences.

Acceptance requires bounded processing, explicit uncertainty, safe missing-data
behavior, participant-controlled data use, and a non-sensor fallback. Any claim
that the adapter measures a physical quantity requires corresponding calibration
evidence. Any claim that it improves playability requires user evaluation under
defined conditions.

The next action is not to add more sensors automatically. Identify whether the
current controller actually lacks an expressive dimension that a sensor can
reliably supply. If a simpler mapping gives the performer better control with
less uncertainty, that is a valid and often preferable engineering result.
