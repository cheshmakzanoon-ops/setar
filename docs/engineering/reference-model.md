# Exact finite-mode stiff-string reference: mathematical and software contract

## Purpose, exclusions, and evidence type

This reference makes a narrowly defined physical calculation reproducible. It
does not attempt to capture all the mechanics of a setar. Its inputs are a
uniform string length, static tension, linear density, bending stiffness, modal
decay coefficients, an initial displacement, and an observation location. Its
outputs are state coordinates, a point displacement, and finite-mode mechanical
energy. All default inputs have the evidence type `synthetic`.

The implementation is in `include/setar/reference.hpp` and `src/reference.cpp`.
It is not an FDTD implementation. It has no spatial grid, no finite-difference
boundary closure, no contact iteration, and no CFL-limited explicit time
integrator. The [numerical handbook](../numerics/README.md) describes broader
proposed methods; do not transfer those methods' stability claims to this code,
or infer that those methods exist because their derivations are present.

The model is appropriate as a verification baseline for finite-mode evolution,
parameter units, observation timing, passive free decay, deterministic resets,
and file/report plumbing. It is not a validation baseline for fingernail release,
movable fret contact, longitudinal motion, two-polarization coupling, nonlinear
tension, body feedback, acoustic radiation, or performer expression. Those
features need their own governing equations, data, numerical method, and tests.

## Coordinates, units, and boundary assumptions

Let x increase from one string endpoint to the other, over the interval from
zero to length L. Let u(x,t) be a transverse displacement in meters. Its positive
direction is arbitrary but fixed; a negative initial pluck is a sign-reversed
experiment, not an invalid physical state. There is only one displacement
polarization in this fixture.

| Symbol | Code field | Unit | Required sign |
| --- | --- | --- | --- |
| L | `length_m` | m | strictly positive |
| T | `tension_n` | N | strictly positive |
| μ | `linear_density_kg_per_m` | kg/m | strictly positive |
| EI | `bending_stiffness_n_m2` | N m² | nonnegative |
| σ₀ | `decay_constant_per_s` | s⁻¹ | nonnegative |
| β | `decay_frequency_squared_s` | s | nonnegative |
| qₙ | `displacement_m` | m | signed |
| vₙ | `velocity_m_per_s` | m/s | signed |
| ωₙ | `angular_frequency_rad_per_s` | rad/s | strictly positive |
| mₙ | `mass_kg` | kg | strictly positive |

The boundary is simply supported: displacement and bending moment vanish at
each endpoint. For a uniform Euler–Bernoulli-type string model with constant EI,
this means u = 0 and uₓₓ = 0 at x = 0 and x = L. It is not a clamped boundary,
which would instead constrain displacement and slope. Real bridge and nut
termination behavior is not established by choosing either mathematical ideal.

Ignoring damping initially, the reference continuum equation is

$$
\mu u_{tt} = T u_{xx} - EI u_{xxxx}.
$$

The sign of the fourth-derivative term is important. Positive EI supplies
restoring bending stiffness; changing that sign makes high spatial frequencies
unstable. A plausible low-frequency sound is not evidence that this term is
correct. The implemented characteristic-frequency test evaluates the positive
sum of tension and bending contributions independently.

## Modal normalization and characteristic equation

Use a finite expansion with unit-peak sine mode shapes:

$$
u_N(x,t) = \sum_{n=1}^{N} q_n(t)\sin(n\pi x/L).
$$

The mode coordinates qₙ therefore have units of displacement. They are not
mass-normalized coordinates and not directly sound-pressure amplitudes. The
spatial integral of the squared mode shape is L/2, so each mode's mass is

$$
m_n = \mu L/2.
$$

Let kₙ = nπ/L. Orthogonality reduces the finite system to independent modes with

$$
\omega_n^2 = (T/\mu) k_n^2 + (EI/\mu) k_n^4.
$$

Both terms have units s⁻². Setting EI to zero recovers exact harmonic spacing,
ωₙ = nω₁. With positive EI, the ratio of frequency to mode number increases with
n. The report calls the departure from the tension-only harmonic a dispersion
quantity; it does not call it an FFT estimate or an observed acoustic partial.

The natural frequency in hertz is ωₙ/(2π). The oscillation frequency after
underdamped loss is slightly lower, and the renderer exports both. Mixing angular
frequency with hertz inside a damping law or a test is a dimensional error,
not a harmless convention change. The suffixes in the public structures make
that distinction explicit.

Modal mass appears even though it cancels from a free oscillator's displacement
equation. It is needed to assign physical energy and to interpret a later force
or impulse. A renderer that can generate a convincing sinusoid without a mass
does not yet define a physically coherent coupling port.

## Damping model and its limits

Each retained mode obeys

$$
\ddot q_n + 2\sigma_n\dot q_n + \omega_n^2 q_n = 0,
\qquad
\sigma_n = \sigma_0 + \beta\omega_n^2.
$$

The coefficient β has units seconds because it multiplies an angular-frequency
square. This is an explicitly chosen phenomenological modal damping law. It is
not automatically identical to a spatial mixed-derivative damping coefficient
in an FDTD string PDE. A spatial term can depend on kₙ², whereas this expression
depends on the full stiff-string ωₙ² and thus also contains a kₙ⁴ contribution.
Do not copy a published spatial-loss coefficient into this field without deriving
the intended relationship and checking units.

The current exact transition supports the underdamped domain only:
0 ≤ σₙ < ωₙ for every retained mode. Zero damping is supported. Critical and
overdamped cases are rejected rather than being forced through an inappropriate
square root. Extending the domain requires new formulas and tests; changing the
validation inequality alone would be incorrect.

This decay law describes amplitude behavior for an isolated mode, not a
guaranteed decay time for a microphone signal. The observed point displacement
can have cancellations and beating. Its instantaneous absolute value need not
decrease monotonically even though total mechanical energy does. The report's
passivity check uses energy, not the visual envelope of a waveform.

## Exact sampled transition

For one mode, define the sampling interval Δt = 1/fₛ, the damped angular
frequency ωd = sqrt(ω² − σ²), the factor r = exp(−σΔt), and
s = sin(ωdΔt)/ωd. The next sampled state is

$$
\begin{bmatrix}q^{j+1}\\v^{j+1}\end{bmatrix}
= r
\begin{bmatrix}
\cos(\omega_d\Delta t)+\sigma s & s\\
-\omega^2s & \cos(\omega_d\Delta t)-\sigma s
\end{bmatrix}
\begin{bmatrix}q^j\\v^j\end{bmatrix}.
$$

The implementation precomputes these four coefficients during `prepare()`.
`advance()` then performs a bounded number of additions and multiplications per
mode. Trigonometric functions, exponentials, parameter checks, and vector
allocation are not in that render path.

Near critical damping, directly subtracting ω² and σ² can lose relative
precision. The implementation evaluates the product (ω − σ)(ω + σ) before taking
the square root. It also evaluates s as Δt times sinc(ωdΔt), with the limiting
value one when the argument is zero. This avoids dividing a tiny sine by a tiny
frequency without acknowledging the limit. The independent resonator test
checks a decay coefficient immediately below the critical boundary against the
critical limiting solution.

“Exact” here refers to the mathematical solution of the selected finite,
uncoupled, linear ODE. Floating-point coefficient evaluation and repeated matrix
multiplication still introduce error. The spatial representation is truncated.
The boundary, material law, decay law, and initial condition are assumptions.
An exact step is not an exact reproduction of an instrument.

No time-discretization truncation error is expected for this autonomous finite
system. The sample-rate agreement test therefore compares states at the same
physical time rather than looking for the second-order slope of a finite-
difference integrator. A later forced, coupled, or nonlinear method must not
inherit that statement unless it actually preserves the necessary assumptions.

## Observation bandwidth is not a CFL condition

The code requires each natural frequency to be strictly below 0.45 times the
sample rate. This is an observation-domain guard that leaves a margin below
Nyquist. It is not a derived stability bound for an explicit spatial scheme.
The exact modal transition would remain an algebraically defined discrete
oscillator outside that observation band, but its sampled output could alias and
be misinterpreted. The reference rejects that situation.

Sample rates between 8,000 and 384,000 Hz are accepted. These are explicit support
limits for this scaffold, not fundamental physical limits. A valid parameter
set at one rate can be rejected at another because its highest retained mode
crosses the observation guard. The code does not silently remove modes when the
sample rate changes, because doing so would change the state dimension and
excitation energy without an explicit request.

The mode-count limit is 128. It bounds preparation storage, the stack scratch
space used for excitation coefficients, and per-sample arithmetic. It is not a
claim that 128 modes are sufficient for any particular setar, gesture, sample
rate, or accuracy target. Choose the truncation as part of the experiment and
retain it in metadata.

## Initial displacement and the sharp-triangle caveat

Let a be the interior pluck position as a fraction of length and h the signed
triangle height in meters. The ideal piecewise-linear initial shape rises from
zero at x = 0 to h at x = aL, then falls to zero at x = L. Its Fourier sine
coefficient is

$$
q_n(0)=\frac{2h\sin(n\pi a)}{n^2\pi^2 a(1-a)},
\qquad v_n(0)=0.
$$

The code initializes only the first N coefficients. It therefore initializes a
smooth, finite sine projection, not an exact sharp triangle in physical space.
An independent Simpson-quadrature test integrates the triangle against each sine
mode to verify this formula without sharing the production helper.

This distinction is especially important when EI is nonzero. A sharp triangle
has a discontinuity in slope and does not have finite continuum bending energy.
Its modal amplitudes decrease approximately as 1/n², while the bending part of
modal stiffness grows as n⁴. Increasing N can therefore improve the displacement
reconstruction while increasing the initial bending energy without convergence.
There is no contradiction: convergence in displacement norm is weaker than
convergence in the norm required by the bending-energy functional.

For that reason, the convergence test measures displacement reconstruction error
and explicitly does not claim energy convergence of this excitation. Keep N
fixed when comparing decay or repeated renders of this fixture. A physically
motivated excitation study should replace the sharp shape with a smooth finite-
energy profile or, preferably, derive an initial/contact state from the actual
excitation mechanism. It must then demonstrate the relevant energy convergence.

The initial segment slope is bounded by 0.1 in magnitude. This prevents the
fixture API from accepting obviously large displacements while still claiming a
small-displacement model. The number is a declared support guard, not a measured
setar linearity threshold. It does not repair the triangle's curvature issue and
does not prove that nonlinear terms are negligible for every accepted input.

`configure_pluck()` validates and stores the coefficient set without changing
the current state. `retrigger()` copies that stored initial displacement into
the live modes and sets velocity to zero. `pluck()` combines the two operations
for offline convenience. Resetting a string silences its current state but
retains the configured excitation; preparing it again clears both live and
configured state.

## Energy and passivity

The finite-mode energy is

$$
H = \sum_{n=1}^{N}\frac{m_n}{2}
\left(v_n^2+\omega_n^2q_n^2\right).
$$

For the selected equations, its derivative is

$$
\frac{dH}{dt}=-\sum_{n=1}^{N}2m_n\sigma_n v_n^2\leq0.
$$

This establishes passive free decay in exact arithmetic. When every σₙ is zero,
H is conserved. The tests separately check lossless conservation and damped
monotonicity with rounding allowances. They do not normalize state, clip modes,
or inject corrective energy to make those tests pass. A numerically visible
energy defect must be understood rather than hidden by postprocessing.

The energy refers to the retained string coordinates. It is not radiated acoustic
energy, body energy, or amplifier energy. Multiplying the observed displacement
by a PCM gain does not change H. An observation point at either endpoint is
silent while the internal modes can still carry positive energy. The endpoint
test explicitly checks that separation.

An external impulse on the standalone `PassiveResonator` changes velocity by
I/m, where I is an impulse in N s. From rest, the resulting kinetic energy is
I²/(2m). That relationship is independently tested. The impulse is an external
energy input; passivity does not mean its energy jump must be nonpositive.
The current string class has no general external-force or bridge-work port.

## Timing and state ownership

`displacement()` observes the current state without advancing it. `tick()` returns
that observation and then advances once. Thus the first output sample after
retrigger is the displacement at t = 0, not at Δt. CSV sample index j corresponds
to j/fₛ, and the final exported time for K samples is (K − 1)/fₛ. The nominal file
duration is K/fₛ. Both notions appear in metadata so the difference cannot be
mistaken for an off-by-one numerical error.

`render(span)` is repeated `tick()` on caller-owned memory. Splitting the same
sample sequence into differently sized blocks must not alter it. The block-
partition test includes empty spans and irregular block lengths, then compares
the resulting samples exactly within one executable.

One thread owns a live model. `prepare()`, excitation configuration, and render
operations must not run concurrently on the same instance. The vectors are
private, and `modes()` returns a read-only span for inspection, not an invitation
to mutate state from another thread. That view becomes invalid when preparation
replaces the underlying vectors. A future telemetry interface must respect this
lifetime rather than publishing a raw span to a UI thread.

## Invalid inputs and transactional preparation

The API rejects non-finite physical parameters, nonpositive length/tension/
density, negative stiffness or loss, unsupported rates and mode counts,
out-of-band frequencies, and non-underdamped modes. Derived mass, frequencies,
initial energy, and standalone-resonator initial state are also checked for
unsupported numerical overflow or underflow where relevant.

Validation happens before replacing an existing string's vectors. Preparation
constructs candidate vectors and swaps them into place only after successful
completion. A rejected parameter set therefore leaves the old prepared state
usable. Excitation configuration similarly computes its candidate amplitudes
before updating the stored excitation. Tests verify this failure behavior.

An unprepared model observes silence. Operations that need a prepared physical
configuration, such as setting an excitation or applying an impulse, throw a
logic error. This distinction lets a boundary return silence safely during an
unsupported lifecycle state while still surfacing incorrect control sequencing
outside the callback. It is not permission to swallow all model errors in an
offline experiment.

## Acceptance and next extensions

Accept a change to this reference only after its parameter domain, units,
normalization, timing, energy meaning, and failure behavior remain explicit.
Add independent tests before widening a claim. For example, critical damping
needs its own exact transition and near-boundary continuity tests; a force port
needs a work convention and independently solved forced response; a smooth
excitation needs a regularity argument and energy-convergence evidence.

The most important next boundary is not a prettier waveform. It is the boundary
between a verified synthetic fixture and an observation of a real instrument.
Crossing that boundary requires a specimen, measurement protocol, uncertainty,
identified parameter mapping, and held-out validation. This reference is useful
precisely because it leaves those missing pieces visible.
