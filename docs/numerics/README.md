# Numerical engineering volume

These chapters separate physical assumptions, numerical correctness, implementation
evidence, and empirical instrument fidelity. Follow the mathematical foundations
first. The numbering below is local to this volume; master phase identifiers and
completion gates are defined in docs/ROADMAP.md.

1. [Numerical roadmap and evidence](00-numerical-roadmap.md)
2. [Stiff lossy string FDTD](01-stiff-string-fdtd.md)
3. [Boundary closures and coupling](02-boundaries-and-coupling.md)
4. [Two polarizations](03-two-polarizations.md)
5. [Moving frets, retuning, and domains](04-moving-frets-and-retuning.md)
6. [Modal reduction](05-modal-reduction.md)
7. [Finite elements](06-finite-elements.md)
8. [Time integration](07-time-integration.md)
9. [Dispersion and dissipation](08-dispersion-and-dissipation.md)
10. [Nonlinear algorithms](09-nonlinear-algorithms.md)
11. [Floating point and real-time constraints](10-floating-point-and-realtime.md)
12. [Verification and analytic benchmarks](11-verification-and-benchmarks.md)
13. [Convergence and golden data](12-convergence-and-golden-data.md)
14. [Reference algorithm blueprints](13-reference-algorithms.md)
15. [Passive loss in the implicit-midpoint string: executed verification](14-damped-midpoint-loss-verification.md)

The matching implementation, review, and debugging prompts are in prompts/numerics.
Equations and test recipes are specifications, not proof of an executed test.
