# Cavity, radiation, and observation engineering

Status: proposed, unimplemented reference and reduction workflows. No measured radiation/directivity asset or calibrated microphone rendering is claimed.

Read in this order:

1. [Cavity and structural–acoustic coupling](01-cavity-and-structural-acoustic-coupling.md).
2. [Exterior BEM and radiation impedance](02-exterior-bem-and-radiation-impedance.md).
3. [Time-domain grids, boundaries, and PML](03-fdtd-boundaries-and-pml-validation.md).
4. [Microphones, room separation, and directivity](04-microphones-room-and-directivity.md).
5. [Radiation reduction and runtime transfers](05-radiation-reduction-and-runtime-transfer.md).
6. [Solver exchange and evidence packages](06-solver-exchange-and-evidence-packages.md).

The [body volume](../body/README.md) supplies mechanically verified states and ports. The acoustic volume returns both loading and observation. Loading must remain passive and participate in the coupled energy ledger; observation must preserve physical units, causality, propagation phase, and declared microphone geometry.

| Evidence question | Primary observable |
|---|---|
| Does air load the body correctly? | Coupled mechanical mobility and interface work |
| Does the model radiate the right power? | Boundary work and integrated exterior intensity |
| Is directional sound correct? | Complex pressure at held-out directions and distances |
| Is the recording comparison meaningful? | Calibration, room separation, and specimen state |
| Can it run safely in audio? | Reduced-model error, bounded cost, and callback audit |

BEM, FEM, and FDTD are alternative or complementary numerical routes, not competing fidelity labels. Each needs convergence and independent fixtures. k-Wave is an additional documented k-space reference option, not the same scheme as the baseline FDTD derivation.

Use [implementation work orders](../../prompts/acoustics/01-implementation-work-orders.md) to turn one unresolved question into a testable change. Do not attempt an all-at-once “realistic body” implementation.
