# Body 03 — Bridge contact, multiport mobility, and string feedback

## Required architecture

This is a proposed, unimplemented coupling specification for P09. It consumes verified string boundary ports and a verified structural model. It produces a bidirectionally coupled instrument reference, not a one-way body effect. The body changes the force and velocity seen by every connected string; those changed string motions alter subsequent excitation.

A body filter applied after an independently synthesized string may reproduce some spectral coloration. It cannot, by itself, reproduce energy exchange, sympathetic excitation, body-dependent decay, or coupled frequency shifts. Such a filter can remain an observation approximation or an explicitly limited prototype, but it must not be described as the coupled physical engine.

The bridge is not merely a frequency response curve. It has geometry, mass, contact compliance, possible rocking, and load-dependent friction. Whether bridge internal modes matter in a selected band is an empirical and numerical question. A rigid-bridge approximation can be justified by comparison, not by the bridge appearing visually short.

## Power-conjugate ports

Let \(\mathbf f\) contain forces entering the body and \(\mathbf v\) the collocated velocities in the same directions. Then input power is

\[
P=\mathbf f^T\mathbf v.
\]

Each translational force has units N and its velocity m/s. Optional rotational ports pair moment N·m with angular velocity rad/s. Do not connect a displacement sample to a mobility input or a force sample to a pressure boundary without the required derivative, area, and coordinate mappings.

For structural coordinates \(\mathbf q\), define

\[
M\ddot{\mathbf q}+C\dot{\mathbf q}+K\mathbf q=B\mathbf f,
\qquad \mathbf v=B^T\dot{\mathbf q}.
\]

The same matrix and its transpose preserve work. If \(B\) distributes a unit force over several nodes, its weights must produce the correct resultant and moment. Port coordinates, local frames, and footprint definitions belong in the asset manifest.

For a modal basis \(\Phi\), the coupling is \(B_r=\Phi^TB\), and observed port velocity is \(B_r^T\dot{\boldsymbol\eta}\). Changing normalization without updating both sides changes coupling strength and energy.

## Multiport mobility

With Laplace variable s, the structural mobility is

\[
Y(s)=sB^T(s^2M+sC+K)^{-1}B.
\]

Its diagonal entries are driving-point mobilities. Off-diagonal entries describe cross-coupling between directions and contact locations. For a linear reciprocal structure with consistent ports, \(Y=Y^T\). Reciprocity uses transpose, not conjugate transpose, for this complex symmetric frequency-response relation.

Passivity requires the Hermitian part of \(Y(i\omega)\) to be positive semidefinite where the mobility representation is valid, together with the appropriate analytic conditions for a full positive-real model. Checking only the diagonal real parts is insufficient. A matrix can have individually plausible driving-point terms but an active combination of simultaneous forces.

Near zero frequency, free-body inertia can produce singular behavior. Do not force a free-free mobility into a grounded stiffness model merely to avoid that singularity. Define the reference frame and treatment of rigid-body motion explicitly.

## String-to-bridge kinematics

At each string contact, resolve boundary velocity into the string’s local transverse and longitudinal directions. The local directions depend on the static geometry and, in a nonlinear model, may evolve. Force transformations must use the conjugate mapping so the sum of string and body boundary work cancels.

If \(\mathbf v_s=T\mathbf v_b\), define \(\mathbf f_s\) as force entering the string from the bridge. Force entering the body from the string is then \(\mathbf f_b=-T^T\mathbf f_s\). Consequently \(\mathbf f_s^T\mathbf v_s+\mathbf f_b^T\mathbf v_b=0\): internal connection work cancels. Derive this from virtual work rather than guessing signs from a drawing.

Several strings may share the same bridge but have different contact locations and lever arms. A one-point bridge can miss rocking and lateral coupling. Begin with the smallest port set that represents the physical directions under study, then compare with a richer set. Retaining three translations at every contact may be unnecessary in a low-band model, but omitting a direction requires evidence.

Include afterlength segments and tail coupling when they affect the target behavior. Replacing them by fixed endpoints is an approximation that changes bridge loading and possible resonances. Record the boundary assumption for each segment.

## Rigid bridge and distributed feet

A rigid bridge model has translation and rotation. Velocity at a contact point with lever arm \(\mathbf r\) is

\[
\mathbf v(\mathbf r)=\mathbf V+\boldsymbol\Omega\times\mathbf r.
\]

The corresponding generalized load is force plus moment \(\mathbf r\times\mathbf F\). This relation gives a compact, work-consistent way to connect string contacts to distributed feet.

Rigid does not mean massless. Bridge inertia can matter even below its first elastic resonance. A massless bridge imposes an algebraic relation; a massive rigid bridge adds states. Compare both with a flexible bridge reference over the intended frequency and force range.

Foot contact pressure need not be uniform. Static downforce, soundboard curvature, bridge geometry, and rocking determine its distribution. A distributed patch model should reproduce resultant force and moment and should not create tensile contact where none is physically supported.

## Friction and microslip

For ideal Coulomb friction, tangential force opposes slip and satisfies a magnitude bound tied to normal force. In sticking, force is whatever is required within that bound; it is not automatically zero. A regularized law must preserve nonpositive frictional work and disclose the velocity scale introduced by smoothing.

Friction coefficients are not universal wood constants. Surface finish, contact pressure, humidity, wear, and orientation can matter. Treat unknown friction as a bounded hypothesis and design safe measurements if it proves influential. Do not tune it solely to make attacks brighter.

A tangential compliance can model presliding storage, but it requires a state and an energy function. Combining an elastic tangential spring with slip must release or dissipate energy consistently. Resetting the spring state arbitrarily at every zero crossing can inject or remove energy without accounting.

Distinguish bridge-foot friction from string-groove contact. The latter can affect segment tension and longitudinal motion. Each contact has its own geometry and state. One global friction parameter cannot represent both without an explicit effective approximation.

## Time discretization and feedback

For a midpoint structural update,

\[
M\frac{\mathbf v^{n+1}-\mathbf v^n}{k}
=-K\bar{\mathbf q}-C\bar{\mathbf v}+B\mathbf f^{n+1/2},
\]

the port velocity is \(B^T\bar{\mathbf v}\). Connected subsystems must exchange force at the same work-conjugate time level. A one-sample-delayed force is not automatically passive merely because each isolated subsystem is stable.

Coordinate this contract with [energy and passive ports](../mathematics/03-energy-and-passive-ports.md) and [boundary coupling](../numerics/02-boundaries-and-coupling.md). Explicit string schemes may use staggered energies and port variables. Their connection to a midpoint body requires a full discrete derivation, not an informal average.

A nonlinear joint or contact should use the numerical framework’s energy-consistent potential gradient or another explicitly proved method. If a bounded iteration is used in real time, define residual limits and a safe failure policy. Never silently clamp an unstable solution and call the resulting sound physical.

## Verification ladder

Start with one string connected to one passive oscillator. Compare against a small independently assembled linear system. Check coupled poles, energy exchange, decay, and limiting cases: rigid body endpoint, very heavy body, weak coupling, and zero damping.

Next connect two strings through a shared body coordinate. Excite one and verify transfer to the other. With equal strings and symmetric ports, symmetric and antisymmetric combinations provide analytical structure for checking mode splitting. Deliberately alter one port sign; the test must detect the resulting reciprocity or energy error.

Then use multiple bridge coordinates. Verify invariance under a rigid rotation of the whole coordinate system. Check that simultaneous forces in an eigenvector of the mobility’s Hermitian part cannot extract unaccounted energy from a passive body.

Finally introduce distributed feet, bridge inertia, preload, and friction one mechanism at a time. Compare against a slower nonlinear reference before reducing the contact representation.

## Measurement and identification

Measure or estimate a matrix of bridge responses, not only one microphone spectrum. Driving-point mobility constrains loading; cross-mobility constrains interactions. Microphone pressure is an observation transfer and cannot directly substitute for bridge velocity.

Use multiple excitation directions where safe and measurable. Document force and velocity signs, calibration, location uncertainty, and sensor loading. A reciprocal comparison requires the same physical ports and boundary state when excitation and response are exchanged.

Separate contact nonlinearity from structural nonlinearity by amplitude sweeps and controlled preload changes only when authorized. A changing FRF can arise from slipping feet, sensor effects, or support changes; it does not uniquely identify wood nonlinearity.

## Acceptance evidence

The gate requires a port manifest, matrix-unit checks, reciprocity residuals, passivity checks, coupled energy tests, limiting-case tests, and held-out cross-response predictions. A body-filter demo does not satisfy the gate.

Primary conceptual reading is Julius O. Smith’s author-hosted “Bridge Modeling” and “Building a Synthetic Guitar Bridge Admittance”: https://www.dsprelated.com/freebooks/pasp/Bridge_Modeling.html and https://www.dsprelated.com/freebooks/pasp/Building_Synthetic_Guitar_Bridge.html. These discuss bridge admittance and passive coupling; they do not supply setar-specific parameters.

The runtime handoff contains the coupled model and observation mapping separately. Preserve physical loading even if a later audio renderer uses a compact radiation filter. That separation is what permits efficient sound output without turning the body back into a one-way effect.
