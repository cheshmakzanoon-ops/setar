# P09 — Couple strings, bridge, and body without hidden energy

**Status:** specification. **Prerequisites:** P04 and P08; P05/P06 provide
interaction tests. **Exit evidence:** a verified multiport coupling whose
mechanical work, reciprocity, and coupled behavior remain correct under the
supported discretizations.

## Why coupling is its own phase

Independently convincing subsystems can become unstable or physically wrong when
connected. A string may exert a force while the body is driven with yesterday's
velocity; a bridge rotation may be confused with translation; or two different
normalizations may scale the same mode twice. These defects often sound like
resonance or extra richness before they become obvious instability.

Read the power-port contracts in [mathematics](../mathematics/README.md),
[body](../body/README.md), and [numerics](../numerics/README.md). The first product
of this phase is a written coupling contract, not a new preset.

## P09-T01: define mechanical ports

For every interface, state coordinate frame, orientation, force sign, velocity
sign, and power pairing. Translational ports use force and velocity; rotational
ports use torque and angular velocity. A normal/tangential basis must be
transformed consistently on both effort and flow variables. Test that a frame
rotation leaves computed physical power invariant.

Describe the bridge footprint and each string's contact location. Decide whether
the bridge is rigid, elastic, lumped, or included in the full structural mesh.
If the bridge is represented both in an effective body mobility and as explicit
mass/stiffness, investigate double-counting. Distinguish the empirical response
of the assembled instrument from the response of a bare component.

Define force distribution and velocity observation as an adjoint pair under the
model's inner product. A spatial interpolation rule is not correct merely
because it is smooth. Write the discrete power identity and test it with
independent random finite vectors, including rotated and multi-contact cases.

## P09-T02: choose a coupled time update

Derive the full connected update before splitting code into components. Identify
which variables are simultaneous and which, if any, are delayed. A one-sample
feedback delay can change passivity and resonances even when each isolated
component is passive. Do not infer stability of the connection from isolated
unit tests.

For an implicit solve, specify matrix structure, factorization reuse, residual
criteria, and failure policy. For a passivity-preserving formulation, show the
actual discrete energy/work balance including contact, damping, and port terms.
For a controlled approximation, bound its error and explicitly limit the
supported step/band/parameter range. Avoid combining two independently proved
schemes and asserting that the composite proof follows automatically.

Start with a string connected to one passive resonator with an analytic or
independently assembled reference. Verify the fixed-boundary limit, soft-support
limit, and zero-coupling limit. These tests localize sign and normalization
defects before the body has many modes.

## P09-T03: introduce all physical strings

Represent the target instrument's physical strings as persistent state, with
course membership and tuning described separately. A paired course does not
mean two identical oscillators with a stereo spread. It can have distinct string
parameters and contact timings while sharing bridge/body motion.

Excite one string and observe the others through the common coupling. Test
sympathetic transfer, energy exchange, and beating for controlled detuning.
With losses removed, the declared closed system's total energy should follow
the expected balance. With losses added, energy transfer between strings must
not be mistaken for global energy growth or decay.

Check invariance under reordering of string storage and equivalent port ordering.
A vector index must not accidentally determine a different physical delay. Use
test cases with unequal strings so a symmetric implementation error cannot
hide behind identical parameters.

## P09-T04: compare response functions, not only audio

Compute driving-point and transfer mobilities at the bridge ports. Inspect
resonance splitting, antiresonances, phase, reciprocity where the physical model
requires it, and low-frequency compliance. Report the reference and sampling
conventions. Magnitude-only agreement can conceal a phase error that changes
feedback behavior.

Compare full-body and reduced-body couplings at the same ports. A reduced model
qualified in isolation may fail after connection if omitted flexibility becomes
important. Repeat truncation studies in the connected configuration and retain
the coupled error budget. Do not tune string damping to conceal reduction error.

## P09-T05: handle preparation and state changes

Specify what happens when tuning, body asset, bridge position, or sample rate
changes while the instrument rings. Some changes represent external work; some
are reconfiguration operations that require a declared state transition. They
must not silently reuse incompatible mode coordinates or factorization data.

Prepare new immutable coefficients and validated assets away from the callback.
Define a safe ownership transfer and retirement policy. If a state projection
or crossfade is used, quantify its consequences and label it a production
transition rather than an exact mechanical event. The old valid model remains
available if preparation fails.

## P09-T06: qualify the connected system

Use long decays, repeated plucks, simultaneous contacts, retuning, and extreme
admissible parameter combinations. Track individual subsystem energy and the
global work residual. Include the deliberately wrong coupling sign in a test
fixture or independent diagnostic experiment to demonstrate that the energy
test can detect the relevant failure.

Acceptance requires bounded supported behavior, understood energy exchange,
convergence or controlled approximation evidence, and reproducible coupled
responses. It does not establish radiated setar fidelity until the acoustic and
measurement phases also pass.

> Derive and implement the specified string/bridge/body connection with explicit
> effort/flow signs and adjoint maps. Verify a one-resonator reference, limiting
> cases, multi-string transfer, and coupled reduction error. Show the discrete
> work balance and any effect of time staggering. Do not hide instability with
> clipping, extra decay, or an undocumented output filter. Deliver a coupling
> diagram and evidence that a second reviewer can reproduce from the manifest.
