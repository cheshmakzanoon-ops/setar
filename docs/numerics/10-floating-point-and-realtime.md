# Floating-point behavior and the real-time numerical contract

## Precision is part of the implemented model

The mathematical scheme is defined in real arithmetic. A C++ implementation
adds rounding, finite exponent range, compiler transformations, parallel
reductions, and platform-specific floating-point modes. These effects must be
measured rather than assumed negligible. They are especially visible in long
decays, near-contact cancellation, ill-conditioned systems, and marginally stable
high-frequency modes.

Use a clear double-precision reference first. A production implementation may
later use mixed precision or vectorized single precision if an error study
supports it. The audio buffer's float type does not require all mechanical
states, coefficients, and energy diagnostics to use float.

Inputs to this chapter are the selected numerical method, supported parameter
envelope, compiler configurations, and runtime platform. Outputs are explicit
precision choices, repeatability criteria, fault behavior, and profiling evidence.
JUCE belongs at the application boundary; the numerical core should be testable
without a plugin host.

## Scale and cancellation

Small physical displacements do not automatically imply poor relative precision.
The problem is often subtraction of nearby quantities or combinations with very
different scales. A displacement of microscopic magnitude can be represented
well, while a gap computed as the difference of two large absolute positions can
lose its significant digits.

Store local displacements relative to an appropriate equilibrium and compute
contact gaps from local geometry where possible. Preserve the exact physical
transform so this conditioning choice does not change the model. Do not subtract
two world-space coordinates with unnecessary large common offsets inside every
contact force evaluation.

Near equal endpoints, use stable divided differences for contact potential.
For small decay-step products, expressions such as \(1-\exp(-dk)\) should use
an expm1-style evaluation where appropriate. For nearly equal frequencies, beat
phase computation may need a difference formulation rather than subtracting two
large accumulated phases.

The explicit recurrence contains cancellation between current and previous
displacements. That is part of the method, but it motivates comparing coordinate
and velocity representations. A reformulation can change rounding and should
be verified against the exact discrete recurrence, not merely the continuum PDE.

## Energy diagnostics and summation

Summing many small modal energies or loss increments can accumulate error.
Use a deterministic reduction order in the strict reference. Pairwise or
compensated summation can improve diagnostic accuracy, particularly when total
external work is accumulated over many steps.

Do not compute a small energy defect by subtracting two independently rounded
large totals and then interpret every final bit as physical evidence. Report
both local work residuals and accumulated residuals, and estimate the expected
rounding scale. Independent analytic fixtures help determine whether a drift is
consistent with rounding or indicates a systematic sign error.

For the FDTD half-step energy, the cross-state expression can involve
cancellation. The positive decomposition using mean state and difference state
is valuable for checking positivity inside the strict stability region. Both
forms should agree to a reasonable rounding budget. Near the stability boundary,
the kinetic control matrix becomes ill conditioned, making energy diagnostics
more sensitive; this is another reason to retain a margin.

## Fused operations and compiler modes

Fused multiply-add can reduce rounding for individual expressions while changing
bit patterns relative to separate operations. Reassociation and approximate
transcendental functions can change larger parts of the algorithm. Distinguish
these transformations instead of treating all optimization as one switch.

GCC's primary optimization documentation states that finite-math-only assumptions
exclude NaNs and infinities:
<https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html>.
Clang documents floating-point contraction and its floating-point models at
<https://clang.llvm.org/docs/UsersManual.html#controlling-floating-point-behavior>.
Inspect the actual compiler version and effective build flags when testing;
defaults and grouped options can differ.

A nonfinite detector compiled under assumptions that nonfinite values cannot
occur may not provide the protection its source code appears to promise.
Compile safety checks under suitable semantics and test them with injected NaN
and infinity values. Do not assume that writing std::isfinite in the source
guarantees the intended machine behavior under every optimization mode.

Maintain a strict diagnostic configuration and an explicitly validated production
configuration. A faster build must pass physical metric budgets, fault-injection
checks, and cross-platform comparisons. It should not automatically inherit
the reference build's numerical evidence.

## Subnormal values and long decays

Subnormal numbers extend representable magnitudes below the normal range with
reduced precision. Some processor paths handle them differently in cost.
On relevant x86 systems, flush-to-zero and denormals-are-zero modes affect output
and input subnormal handling. Intel's primary documentation describes these
controls:
<https://www.intel.com/content/www/us/en/docs/dpcpp-cpp-compiler/developer-guide-reference/2024-0/set-the-ftz-and-daz-flags.html>.

Do not assume the same control register or semantics on every target. Establish
the platform's supported mode through its official documentation and runtime
tests. Preserve the surrounding application's expected floating-point state
when a scoped mode change is used.

Flushing very small states to zero introduces a numerical loss, even if it is far
below an intended audibility floor. Quantify it for the chosen state scaling and
precision. A modal coordinate with unusual normalization can reach the subnormal
range at a different physical energy than a displacement coordinate.

Never add random noise to prevent denormals or to conceal instability in the
mechanical core. That creates an unmodeled energy source and compromises
deterministic verification. If an explicit silence threshold is used for product
voice management, label its energy removal as a product-level state termination,
not as physical damping.

## Overflow, invalid inputs, and conversions

Validate all physical inputs before coefficient construction: finite length,
positive density, supported cross-section, nonnegative permitted damping, and
admissible force and contact parameters. Validate derived values as well.
Finite inputs can overflow when squared or raised to fractional powers.

Avoid converting an unchecked floating-point grid count directly to an integer.
First check its range and finiteness, then apply the stated rounding rule.
Array dimensions and byte-count products need integer overflow checks. A
numerically valid h is not sufficient if its corresponding state allocation
exceeds the prepared capacity.

For fractional contact powers, negative bases must be handled through the
one-sided compression law before exponentiation. Do not rely on a platform
power function to return a useful value for an invalid branch.

Use structured error results for invalid preparation. The engine should distinguish
unsupported physical configuration, insufficient preallocated capacity, failed
factorization, and a nonfinite runtime state. These are different problems with
different remedies.

## Real-time ownership and preparation

Separate preparation from stepping. Preparation validates parameters, chooses
the grid or modal basis, allocates arrays, builds maps, computes coefficients,
and factors fixed matrices. Stepping consumes a prepared immutable configuration
and mutable preallocated state.

The callback must not allocate, lock, read files, access the network, grow a
container, perform an unbounded geometry search, or run an unbounded nonlinear
iteration. Avoid exception paths that allocate or unwind through host code.
An error code or bounded diagnostic event can report failure without violating
the callback contract.

Host block size is an I/O partition, not a physical time step. Process identical
sample-accurate events consistently whether the host supplies one large block
or several smaller blocks. A contact event at a sample boundary must not move
because the block boundary changed.

Control data should be transferred as immutable snapshots or carefully designed
real-time-safe events. Do not permit a UI thread to modify one physical coefficient
while the callback reads a related coefficient from an older configuration.
The resulting hybrid may violate both units and stability.

## SIMD, parallelism, and deterministic ordering

SIMD can process independent modes, strings, or grid positions efficiently.
It must preserve data dependencies in a stencil and shared nonlinear solve.
If y and z share tension, evaluate that tension from a consistent pair of states.
If several strings share a bridge, their coupling force cannot be independently
computed from incompatible bridge states.

Parallel reductions change addition order and can change bits. The strict
reference may use a fixed order, while production may permit bounded metric
differences. Document the distinction. A race condition is not an acceptable
cross-platform numerical tolerance.

Use thread sanitizers and other diagnostic tools outside real-time performance
measurements. Their overhead invalidates ordinary callback timing claims, but
their findings can reveal defects that would otherwise appear as numerical
instability.

## Runtime failure containment

Keep accepted and proposed states distinguishable for nonlinear updates. On a
failed proposal, do not partially overwrite the accepted state and then attempt
to continue as though nothing happened. A complete rollback buffer or an
equivalent transactional design is needed if retries are supported.

A product may fade or mute a failed voice to protect output, but record that the
render is invalid. Never classify output limiting as an integrator correction.
Do not normalize state energy, clamp individual displacements, or randomize
the state to escape a failure.

Finiteness checks should be placed where they give useful localization without
unbounded cost: prepared coefficients, nonlinear residuals, accepted states, and
final output. A nonfinite output can originate from an observation filter rather
than the mechanics; maintain subsystem diagnostics.

## Verification and rejection gates

Compare strict scalar, optimized scalar, and SIMD implementations on identical
fixtures. Test long decay, zero input, tiny input, large supported input,
near-equal contact endpoints, nearly degenerate modes, and minimal grids.
Inject invalid coefficients and nonfinite states in a diagnostic build and verify
the exact rejection path.

Measure runtime across silence, steady decay, dense contact, and rapid control
changes. Silence can be a worst case for denormal-sensitive paths, while contact
can be a worst case for iteration count. Average cost of a pleasant pluck is
not enough.

Reject unexplained compiler-mode differences, platform-specific hidden clipping,
unbounded retries, and checks that disappear under the actual production flags.
The next action is to put these requirements into analytic and manufactured
benchmark fixtures, then define cross-platform golden-data policy.
