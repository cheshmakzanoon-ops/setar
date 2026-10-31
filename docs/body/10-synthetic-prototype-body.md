# Body 10 — Synthetic physics-derived body for the playable prototype

## Status and claim boundary

**Status: implemented and numerically verified for a declared synthetic fixture.**
[`body_model.hpp`](../../include/setar/instrument/body_model.hpp) and
[`body_model.cpp`](../../src/instrument/body_model.cpp) build the `BodyConfig`
that the prototype engine couples to the four strings through bridge springs.
Every number below is a **synthetic placeholder** chosen inside generic
hardwood and air ranges. None is a measurement of a setar, and the plate is not
the real pear-shaped mulberry soundboard. The body is labelled
`Provenance::Synthetic`, and its label string says so.

This work order is a partial, prototype-scale contribution to
[ENG-P08-T03](../../roadmap/engineering-tasks.json) (passive reduction with
port semantics) and exercises a synthetic subset of `REQ-BOD-002` (mobility
reciprocity/passivity) and `REQ-BOD-003` (passive reduction). It does **not**
satisfy the measured-mobility part of `REQ-BOD-002`: no force/velocity
calibration exists. The route that must replace this fixture is the measured
FRF and modal identification protocol in
[Body 06](06-measured-frfs-and-modal-identification.md) and the passive
reduction rules in [Body 08](08-passive-reduction-and-residual-flexibility.md).

## Coordinates, units and signs

- Plate frame: origin at a soundboard corner; `x_p` (m) along the strings in
  `[0, a]` with `x_p = 0` at the neck end; `y_p` (m) across in `[0, b]`.
- Plate displacement `w` is positive **away from the cavity interior**, the
  same sign as `Polarization::Vertical` of the strings.
- The air-plug coordinate `xi` in the opening is positive **outward**.
- Bridge contact `c = (s, p)` has displacement
  `w_c = sum_j psi_j[s][p] * eta_j` (dimensionless participation, m per m).
- Radiation weights are in Pa·s²/m and multiply modal **acceleration**.
  Observation never feeds back into mechanics.

## Model

### Orthotropic Kirchhoff plate

Simply supported thin plate with grain along `x_p`. Reciprocity fixes the
minor Poisson ratio `nu_RL = nu_LR E_R / E_L`, and the plane-stress compliance
is positive definite exactly when `nu_LR^2 < E_L / E_R` (with positive moduli).
`validate()` rejects any other set: an indefinite tensor describes no elastic
material, and the code does not "repair" it. Flexural rigidities:

```text
D11 = E_L h^3 / (12 (1 - nu_LR nu_RL))      D22 = E_R h^3 / (12 (1 - nu_LR nu_RL))
D12 = nu_LR D22 = nu_RL D11                  D66 = G h^3 / 12
omega_mn^2 = pi^4/(rho h) [ D11 (m/a)^4 + 2 (D12 + 2 D66) (m/a)^2 (n/b)^2 + D22 (n/b)^4 ]
W_mn(x, y) = sin(m pi x / a) sin(n pi y / b),   modal mass M_p = rho h a b / 4
```

`reciprocal_poisson_pair()` screens an *independently reported* pair
`(nu_LR, nu_RL)` against `nu_LR/E_L = nu_RL/E_R`. It only accepts or rejects; it
never substitutes a corrected value. This is the executable form of the audit
in [Body 01](01-orthotropic-material-audit.md).

### Helmholtz cavity and bridge mass

The bowl is rigid; the enclosed air acts as one stiffness on the total outward
volume displacement `Delta V = sum_j V_j eta_j + A xi`, where the plate mode
volume is `V_mn = 4ab/(m n pi^2)` for odd `m, n` and zero otherwise:

```text
kinetic  T = 1/2 sum_j M_p eta_j'^2 + 1/2 m_a xi'^2 + 1/2 m_b (sum_j W_j(x_b) eta_j')^2
potential U = 1/2 sum_j M_p omega_j^2 eta_j^2 + 1/2 kappa (Delta V)^2
m_a = rho_air A L_eff,   kappa = rho_air c^2 / V,   f_H = c/(2 pi) sqrt(A/(V L_eff))
```

The cavity pressure `p = -kappa Delta V` acts on each coordinate through its
volume, which is the gradient of `U`. A lumped bridge mass `m_b` sits at the mean
bridge point. Plate modes below `coupling_basis_factor * max_frequency_hz`
(1.5 × 7 kHz by default, 68 plate modes) plus the air plug form the symmetric
generalized problem `K phi = lambda M phi`, solved by Cholesky reduction and a
cyclic Jacobi eigensolver. Coupled modes below `max_frequency_hz` are retained.

Each coupled mode is scaled so its dominant mass-weighted component equals one;
its modal mass is then `phi^T M phi`. In the uncoupled limit a plate mode keeps
`M_p` and the shape `W_mn`, which the tests check.

### Damping (approximation)

Physical damping is diagonal in the uncoupled coordinates,
`C = diag(2 zeta_p omega_j M_p, 2 zeta_c omega_H m_a)`. Each coupled mode
receives `zeta_r = phi_r^T C phi_r / (2 omega_r m_r)`. **This projection drops
the off-diagonal modal damping terms**; it is first-order accurate for light
damping and well separated modes and is not a measured loss model. Radiation
damping is not derived from the radiated power; it is folded into the declared
`zeta` values.

### Lateral bridge mode

One synthetic oscillator gives the horizontal string polarization a finite
bridge impedance and loss. Its participation is 1 for every string in the
horizontal direction and 0 vertically; plate and cavity modes have zero
horizontal participation. There is therefore no vertical/horizontal structural
cross-coupling in this body.

### Observation

```text
p_o = rho0 / (4 pi r_o) * sum_r ( Q_r + d_o Q1_r ) * eta_r''
Q_r  = sum_j phi_jr V_j + phi_ar A + (lateral: A_lat)
Q1_r = sum_j phi_jr X_j + phi_ar A (x_open - a/2),   X_mn = -2 a^2 b /(m n pi^2) (m even, n odd)
```

`d_o` (1/m) is a static synthetic dipole weight that decorrelates the two
channels. It is not a frequency-dependent radiation model, room model or
microphone calibration (see `REQ-ACO-002`).

## Synthetic design values

| Quantity | Value | Rationale (synthetic) |
| --- | --- | --- |
| Plate `a × b × h` | 0.20 m × 0.13 m × 3.0 mm | Small-instrument soundboard scale; rectangular stand-in |
| Density `rho` | 650 kg/m³ | Generic medium-density hardwood |
| `E_L`, `E_R`, `G_LR` | 9.5 GPa, 0.9 GPa, 0.7 GPa | Generic hardwood ratios (`E_R/E_L` ≈ 0.095) |
| `nu_LR` (`nu_RL`) | 0.37 (0.0351 by reciprocity) | Admissible: `0.37 < sqrt(E_L/E_R) = 3.25` |
| Plate damping `zeta_p` | 0.012 | Light structural damping placeholder |
| Cavity `V`, `A`, `L_eff` | 1.3 L, 2.0 cm², 7.3 mm | Small bowl; several small holes lumped as one opening |
| Cavity `c`, `rho_air`, `zeta_c` | 343 m/s, 1.2 kg/m³, 0.03 | Room-temperature air; damping placeholder |
| Opening centre | (0.05, 0.065) m | Near the neck end; used only for the dipole moment |
| Bridge points `(x_p, y_p)` | x = 0.145 m; y = 57, 62, 67, 70 mm | Lower region; across-bridge order assumed monotone in string index |
| Bridge mass | 1.5 g | Small wooden bridge placeholder |
| Lateral mode | 1600 Hz, 40 g, `zeta` 0.05, 2 cm² radiating area | Placeholder horizontal impedance |
| Observation | `r` = 0.5 m, `d` = ±4 m⁻¹, `rho0` = 1.2 kg/m³ | Observation-only stereo placeholder |
| Retention | 7 kHz, basis factor 1.5 | Bounds runtime cost |

Resulting synthetic body: **48 modes from 155.3 Hz to 6992 Hz**. The uncoupled
plate (1,1) at 202.1 Hz and Helmholtz frequency 250.6 Hz split into coupled
modes at 155.3 Hz and 314.1 Hz (157.9 Hz and 319.9 Hz without the bridge mass).
Only one retained mode, the lateral mode, has horizontal participation.

## Executable verification

All in [`body_tests.cpp`](../../tests/instrument/body_tests.cpp); oracles are
coded in the test, not reused from the implementation.

| CTest | Independent oracle and acceptance |
| --- | --- |
| `materials.reciprocity` | 201 random admissible sets: `S12 = S21`, `D12 = nu_LR D22 = nu_RL D11`; consistent pair accepted, contradictory/5 %-defective/indefinite pairs rejected |
| `materials.positive_energy` | Positive minors and complementary energy for 2000 random stresses; `nu_LR` at 1.0, 1.01 and 2 × `sqrt(E_L/E_R)` rejected by `flexural_rigidity`, `validate` and `build_body` |
| `body.plate_frequency_oracle` | Finite-difference orthotropic plate (`D11 Lx² ⊗ I + 2(D12+2D66) Lx ⊗ Ly + D22 I ⊗ Ly²`) solved by subspace iteration on 7×5, 15×11, 31×23 grids: first four frequencies converge monotonically at second order (error ratio > 3.3); finest error < 0.5 % (0.11 %–0.47 % observed) |
| `body.cavity_coupling_oracle` | Basis restricted so only (1,1) has net volume: `build_body` matches the closed-form 2-DOF plate/Helmholtz roots to 1e-10; volume-free modes keep analytic mass, shape and dipole weight; scaling `V` and `A` together drives the shift from 0.28 to 5.5e-5 monotonically; cavity disabled recovers the plate |
| `body.mobility_reciprocity` | `Y_ij = Y_ji` for all 8 × 8 contacts at seven frequencies; uncoupled body equals the analytic plate modal sum to 1e-9; horizontal mobility equals the lateral oscillator |
| `body.mobility_passivity` | 2001 log-spaced frequencies, 10 Hz–20 kHz: `Re Y_ii ≥ 0` and the 8 × 8 `Re Y` (Hermitian part of a reciprocal `Y`) has no eigenvalue below `-1e-12 max Re Y_ii` (observed `-3e-16`); modes finite, sorted, positive mass, `zeta` in [0, 1) |
| `body.invalid_design` | 28 field mutations rejected; disabled subsystems not validated; mobility rejects bad indices, negative frequency and zero modal mass |

The tests also pass under GCC AddressSanitizer/UndefinedBehaviorSanitizer and
compile warning-free with Clang. Passivity of the modal sum is structural
(`zeta >= 0`, `M > 0`); the test guards against sign or normalization errors,
it does not demonstrate passivity of a measured body.

## Limitations

1. Rectangular simply supported plate, not the setar's curved, pear-shaped,
   edge-glued soundboard; no bars, finish, joints, preload or string downbearing
   (see [Body 02](02-joints-finish-and-preload.md)).
2. Rigid bowl: bowl flexibility and bowl–soundboard coupling are absent; the
   cavity is a single lumped Helmholtz stiffness without higher cavity modes
   ([Acoustics 01](../acoustics/01-cavity-and-structural-acoustic-coupling.md)).
3. The opening is not removed from the plate stiffness or mass.
4. Projected diagonal damping; no radiation damping derived from radiated power;
   no frequency-dependent or causal material loss
   ([Body 07](07-damping-and-causal-materials.md)).
5. Truncation without residual flexibility: no static correction for the modes
   above `max_frequency_hz` ([Body 08](08-passive-reduction-and-residual-flexibility.md)).
6. The bridge is a point mass with point contacts per string; no rocking,
   multi-axis rotation or vertical/horizontal cross-mobility.
7. Observation is a static monopole plus dipole weight, not exterior radiation,
   directivity or a calibrated microphone.

## Replacing the fixture with evidence

A measured body must arrive as calibrated multi-axis bridge FRFs with
uncertainty, identified poles and residues with physical scaling, and
held-out reciprocity/passivity checks ([Body 06](06-measured-frfs-and-modal-identification.md),
[Body 05](05-eigenmodes-normalization-and-tracking.md)). The `BodyConfig`
interface already accepts per-contact participations, so identified residues
`psi_c psi_d / M` can replace these modes without changing the engine. Keep the
same `body.mobility_*` tests, add the measured FRF as a held-out comparison, and
record the replacement as a new provenance class rather than editing these
synthetic values in place.
