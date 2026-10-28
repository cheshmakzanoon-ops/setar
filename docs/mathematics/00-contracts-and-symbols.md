# Mathematical contracts and notation

## Scope and evidence status

This volume specifies mathematical models and verification procedures for a
physical-model setar engine. It is not evidence that the repository already
implements those models, that any parameter set describes a measured instrument,
or that numerical correctness establishes perceptual fidelity. Its intended
output is an implementable, reviewable sequence of model contracts. Implementation
status and phase gates belong to docs/ROADMAP.md; measurement provenance belongs
to the research documentation. These chapters define the evidence a phase must
produce before its sound is used to justify a subsequent modeling decision.

The default physical scope is persistent strings, bridge and body degrees of
freedom, and mechanically represented excitation and stopping contacts. The core
uses C++20 without a JUCE dependency. JUCE may transport events and audio at plugin
or application boundaries; it does not determine a force law, integration rule,
unit convention, or stability argument. A recording can validate a model or
identify its parameters, but recorded-note playback is not the synthesis mechanism.

There are three assertions that a contributor must not collapse:

1. A mathematical assertion follows from explicitly stated assumptions and a
   derivation, such as positivity of energy for a fixed linear operator.
2. An implementation assertion follows from an executed test of particular
   source code, compiler options, numerical precision, and input data.
3. An instrument assertion follows from empirical comparison with permitted,
   provenance-tracked setar measurements and, where appropriate, listening evidence.

Passing the first two is necessary for interpreting the third, but does not imply
it. A perfect solution of an unsuitable differential equation remains an unsuitable
instrument model. A pleasing waveform does not prove that an unstable update is
acceptable. Numerical effects that improve one isolated sound can still destroy
the physical relationships needed for a different articulation.

## Coordinate and sign contract

Use a right-handed instrument-fixed frame documented with measured geometry.
For a straight reference string, x increases from nut toward bridge. The two
transverse coordinates are y, approximately in the soundboard plane across
courses, and z, directed away from the top. These descriptive directions are
not a replacement for a calibrated frame: handedness and the orientation matrix
must be recorded. Displacements are measured from a specified static equilibrium.

For scalar derivations, \(u(x,t)\) means one transverse displacement in metres.
For vector derivations, \(\mathbf r=(y,z)^T\) means two transverse displacements.
Longitudinal displacement, when included, is \(w\). A positive force performs
positive work when its collocated velocity has the same sign. At a connection,
the force on one component is the negative of the force on the other after both
have been expressed in the same frame.

A contact gap \(g\) is positive for separation. Penetration is
\(\eta=\max(0,-g)\) when a nonnegative penetration variable is needed. The signed
compression \(s=-g\) is often preferable because a one-sided potential enforces
separation itself. A normal points in the direction of increasing gap. For a
fixed surface below a string, a compressive force acts on the string along that
normal. Changing a normal or a contact location changes geometry; it is not
merely a harmless replacement of an array index.

## Symbol table

| Symbol | Meaning | SI unit or convention |
| --- | --- | --- |
| \(x,L,h\) | Material coordinate, reference length, spatial step | m |
| \(t,k\) | Physical time and numerical step | s |
| \(f_s=1/k\) | Internal solver sample frequency | Hz |
| \(u,y,z,w\) | Displacement | m |
| \(v,u_t\) | Velocity | m/s |
| \(\mu\) | Linear mass density | kg/m |
| \(\rho\) | Volumetric density, when separately used | kg/m³ |
| \(A\) | Cross-sectional area | m² |
| \(T,T_0\) | Axial tension and its reference value | N |
| \(E\) | Young's modulus | Pa |
| \(I\) | Second moment of area | m⁴ |
| \(EI\) | Flexural rigidity | N m² |
| \(c=\sqrt{T/\mu}\) | Ideal-string wave speed | m/s |
| \(\kappa=\sqrt{EI/\mu}\) | Stiffness parameter | m²/s |
| \(\sigma_0\) | Frequency-independent decay parameter | 1/s |
| \(\sigma_1\) | Spatially dependent decay parameter | m²/s |
| \(f(x,t)\) | Applied transverse force density | N/m |
| \(F(t)\) | Point translational force | N |
| \(J\) | Translational impulse | N s |
| \(H\) | Stored energy | J |
| \(P,Q\) | Supplied power, nonnegative dissipated power | W |
| \(\mathbf q\) | Generalized coordinate vector | Basis dependent |
| \(M,C,K\) | Mass, damping, stiffness matrices | Basis dependent |
| \(S=-D_{xx}\) | Positive negative-Laplacian matrix | 1/m² |
| \(W\) | Spatial quadrature matrix | m for nodal string quadrature |
| \(\lambda_j\) | Generalized stiffness eigenvalue | 1/s² |
| \(\omega_j=\sqrt{\lambda_j}\) | Undamped angular frequency | rad/s |
| \(\xi_j\) | Spatial wavenumber | rad/m |
| \(\Phi\) | Lumped contact potential | J |
| \(\phi\) | Contact potential density | J/m |
| \(\alpha\) | Contact power exponent | Dimensionless |

Radians are dimensionless in dimensional analysis, but retaining rad/s prevents
confusion between angular frequency and cycles per second. Convert an eigenvalue
to hertz using \(\sqrt{\lambda_j}/(2\pi)\), not \(\lambda_j/(2\pi)\).
The source papers use several density symbols; transcribe their physical meaning
rather than copying an identifier. Code should prefer descriptive names such as
linearDensityKgPerM, flexuralRigidityNm2, and internalStepSeconds.

Matrix units need care when displacement and rotation coexist. A rotational
coordinate is dimensionless, its conjugate load is moment in N m, and mixed matrix
blocks have different units. A matrix's numerical entries are not all kilograms
merely because it is called a mass matrix. A change of coordinate scaling changes
the matrix representation without changing physical energy.

## Time-grid operators

For \(\mathbf q^n\) approximating \(\mathbf q(nk)\), define

\[
\delta_{t+}\mathbf q^n=(\mathbf q^{n+1}-\mathbf q^n)/k,\qquad
\delta_{t-}\mathbf q^n=(\mathbf q^n-\mathbf q^{n-1})/k,
\]
\[
\delta_{t0}\mathbf q^n=(\mathbf q^{n+1}-\mathbf q^{n-1})/(2k),\qquad
\delta_{tt}\mathbf q^n=(\mathbf q^{n+1}-2\mathbf q^n+\mathbf q^{n-1})/k^2.
\]

The adjacent-state mean is
\(\overline{\mathbf q}=(\mathbf q^{n+1}+\mathbf q^n)/2\). It is not the
three-level mean \((\mathbf q^{n+1}+2\mathbf q^n+\mathbf q^{n-1})/4\).
Every equation must identify the intended mean. Saying a force is evaluated at
the midpoint is insufficient when both integer and half-integer states exist.

The exact identity

\[
\delta_{t-}\mathbf q^n=\delta_{t0}\mathbf q^n
-\frac{k}{2}\delta_{tt}\mathbf q^n
\]

explains why changing a backward loss term to a centered term changes both the
update matrix and the discrete energy. It also exposes a first-order perturbation
that is easy to overlook when the wave terms are centered. An accuracy label
must describe the complete scheme, including boundaries, initialization, forcing,
contact activation, and parameter updates.

## Selecting a model is a controlled experiment

Begin with the simplest model that can answer the next empirical question. An
ideal string checks travel speed and reflection. A stiff lossy string addresses
inharmonicity and decay. Two polarizations address directional bridge coupling.
A geometric nonlinearity addresses amplitude-dependent pitch and cross-plane
interaction. Each addition changes the hypothesis being tested; mathematical
availability alone does not justify an addition.

Write a component's inputs, states, outputs, energy, and power ports before its
update loop. For example, bridge force and bridge velocity must be determined by
a causality convention or simultaneous solve. Treating both connected components
as independent velocity sources overconstrains the port; treating both as unrelated
force sources leaves its motion unspecified. The chosen computation must retain
the underlying bidirectional interaction.

Preserve string states through ordinary note events. A pluck changes a force or
contact state, not the existence of the string. A fret changes constraints and
energy pathways, not necessarily the allocated domain. A tuning control may alter
tension, speaking length, or a reduced-model parameter, but these are distinct
physical operations. They cannot share an undocumented state update.

## Equation provenance and independent derivation

An imported equation needs title, authors, exact DOI or URL, section or equation
number, source notation, and repository notation. A newly derived equation needs
its assumptions and derivation instead of an invented attribution. If an integrator
is new but its PDE is published, distinguish those facts. A paper's stability
result does not automatically apply to a different force time level.

Primary anchors for this volume are:

- Stefan Bilbao and Alberto Torin, *Numerical Simulation of String/Barrier
  Collisions: The Fretboard*, sections 2 and 3, especially equations (1), (2),
  (3), (14), and (17):
  <https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>.
  The physical string terms and spatial stability expression were checked
  against the paper. The repository gives its own explicit energy derivation.
- Stefan Bilbao, Alberto Torin, and Vasileios Chatziioannou, *Numerical Modeling
  of Collisions in Musical Instruments*, DOI
  <https://doi.org/10.3813/AAA.918813>, author manuscript
  <https://arxiv.org/pdf/1405.2589>. The potential formulation is relevant;
  it does not establish convergence of every proposed contact implementation.
- Julius O. Smith III, *Physical Audio Signal Processing*, section *The Stiff
  String*: <https://www.dsprelated.com/freebooks/pasp/Stiff_String.html>.
  This author-written text supplies independent model context, not setar-specific
  parameters or perceptual validation.
- Alberto Torin, Brian Hamilton, and Stefan Bilbao, *An Energy Conserving Finite
  Difference Scheme for the Simulation of Collisions in Snare Drums*, sections
  2.6 and 3.3–3.5:
  <https://www.dafx.de/paper-archive/2014/dafx14_alberto_torin_an_energy_conserving_fini.pdf>.
  This is a cross-domain example of contact and subsystem energy accounting.
  Its membrane and acoustic parameters are not setar parameters.
- Julien Bensa, Stefan Bilbao, Richard Kronland-Martinet, Julius O. Smith III,
  and Thierry Voinier, *Computational Modeling of Stiff Piano Strings Using
  Digital Waveguides and Finite Differences*, sections 4–5:
  <https://www.pure.ed.ac.uk/ws/files/11222295/ActaAcustica2005.pdf>.
  This source compares approximation error and computational cost. Its
  parameterized finite-difference family must not be confused with the fixed
  scheme specified in this repository.

These are source locators, not permission to bundle papers. Keep original equation
audits and bibliographic records in the repository. Where a formula is rederived
here, the derivation is the direct justification for its use. Source snippets,
OCR text, and typographic signs can be wrong; a dimensional and energetic check
is required even when the cited work is authoritative.

## Entry and rejection gates

A contributor must identify the meaning of every state, all places energy is
stored, all power inputs, all losses, variable coefficients, boundary conditions,
force time levels, failed-step behavior, and an independent reference test. If
the answers depend on guessing a plugin default, reject the numerical contract.

The immediate next action is to derive the continuous power balance, then choose
discrete operators and port mappings. Optimization comes after those contracts.
Nothing in this volume establishes a one-to-one imitation of an acoustic setar;
that remains an empirical objective with uncertainty and a finite validity domain.
