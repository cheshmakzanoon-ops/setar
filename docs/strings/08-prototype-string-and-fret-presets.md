# Prototype string, tuning and fret presets (synthetic)

**Status: implemented synthetic presets for the playable prototype.** The values
below come from
[`setar_preset.hpp`](../../include/setar/instrument/setar_preset.hpp) and
[`setar_preset.cpp`](../../src/instrument/setar_preset.cpp). The tests are in
[`preset_tests.cpp`](../../tests/instrument/preset_tests.cpp). Every number is a
**synthetic placeholder**, chosen to fall inside generic engineering ranges for thin
plain metal strings. None was measured on an Iranian setar. No tuning or fret map
here describes what any performer does. Measured inputs come from
[physical specification](02-physical-specification.md) and
[identification experiments](07-identification-experiments.md). Musically meaningful
tunings come from [performance and tuning](../ethnomusicology/performance-and-tuning.md)
and practitioner review.

## Task contract

- **Prerequisites:** the `setar::instrument` configuration contract in
  [`config.hpp`](../../include/setar/instrument/config.hpp): SI units, x measured
  from the nut, string indices 0..3. The engine-side admissibility check
  `validate(InstrumentConfig)` and the synthetic body builder
  `build_body(synthetic_setar_body_design())`. Both are separate modules.
- **Inputs:** `PresetOptions` takes a tuning id, a fret-map id, a speaking length and
  an afterlength. A value of 0 or an empty id selects the default. It also has a flag
  that leaves out the body.
- **Outputs:** a validated `InstrumentConfig` with four strings, 25 shared fret
  positions, a synthetic body (or an empty one), bridge springs and contact laws. It
  also exposes the tuning, fret-map and course tables, each with a status string.
- **Procedure:** derive each string's linear density and bending stiffness from a
  plain round cross-section. Solve for the tension that gives the target open
  frequency on the simply supported stiff string. Place frets at ideal-string
  positions. Attach synthetic loss, action, bridge and contact values. Run
  `validate()` before returning.
- **Failure handling:** an unknown id, a nonpositive or non-finite length, an
  infeasible (nonpositive) tension, or a non-increasing fret ratio throws
  `std::invalid_argument`. Values are never clamped.

## Strings

Each string is assumed to be plain, round and homogeneous, so
\(\mu=\rho\pi d^2/4\) and \(EI=E\pi d^4/64\). This assumption is the most fragile
part of the table: \(EI\) scales with \(d^4\). A wound or composite real string can
differ a lot. The material constants are generic handbook-scale values for steel and
for a brass-like copper alloy. They are not the alloy of any real setar string.

| String | Material (synthetic) | \(d\) (mm) | \(\rho\) (kg/m³) | \(E\) (GPa) | \(\mu\) (kg/m) | \(EI\) (N·m²) | \(T\) default tuning (N) |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | plain steel | 0.20 | 7850 | 200 | 2.466e-4 | 1.571e-5 | 31.22 (C4) |
| 1 | plain steel | 0.25 | 7850 | 200 | 3.853e-4 | 3.835e-5 | 27.44 (G3) |
| 2 | brass-like alloy | 0.35 | 8500 | 100 | 8.178e-4 | 7.366e-5 | 25.88 (C3) |
| 3 | plain steel | 0.20 | 7850 | 200 | 2.466e-4 | 1.571e-5 | 31.22 (C4) |

The default speaking length (nut to bridge) is **0.68 m**. The default afterlength
(bridge to tail anchor) is **0.04 m**, so the modal string spans 0.72 m. In the
alternative tuning, string 1 is F3 and its tension is 21.68 N.

Tension comes from the stiff-string inverse relation for a simply supported span,
\(T=\mu(2Lf)^2-EI(\pi/L)^2\), with \(L\) the speaking length. A nonpositive result
throws. The forward check in `preset.tension_inverse` recomputes
\(\omega_1^2=(T/\mu)k^2+(EI/\mu)k^4\) independently and matches each target to a
relative \(10^{-12}\). The test also requires every synthetic tension to lie in
10–60 N. This guards against unit mistakes; it is not a claim about real setar
tensions. Real terminations are not simply supported, and the bridge in the engine
is a stiff spring on a modal body. So the realized engine pitch differs slightly from
this formula. That residual belongs in engine pitch tests, not here.

Loss laws use the shared form
\(\sigma_n=\sigma_0+\sigma_1k_n^2+\sigma_2\omega_n^2\). They are synthetic: steel
strings 0, 1 and 3 use \(\sigma_0=0.8\) 1/s, \(\sigma_1=0\), \(\sigma_2=2\times10^{-9}\) s.
The brass-like string 2 uses \(\sigma_0=1.2\) 1/s, \(\sigma_2=3\times10^{-9}\) s. With
\(\sigma_2=2\times10^{-9}\) s the decay is about 2.8 1/s at 5 kHz and about 26 1/s
at 18 kHz. \(\sigma_1\) is zero because, for these nearly ideal strings,
\(k_n^2\) and \(\omega_n^2\) are almost proportional and could not be identified
separately (see [loss and dispersion](03-loss-and-dispersion.md)).

Action, the distance from the string rest line down to the fret-top line, rises
linearly from **0.6 mm** at the nut to **3.0 mm** at the bridge on every string. At the
highest default fret (x = 0.438 m) it is about 2.1 mm.

## Tunings

Both tunings take C4 from twelve-tone equal temperament with A4 = 440 Hz (261.626 Hz).
This is a convenient absolute reference only. The other strings are tied to it by
exact ratios. Both have status *"synthetic educational example, not a documented
performer tuning"*.

| Id | String 0 | String 1 | String 2 | String 3 |
| --- | --- | --- | --- | --- |
| `synthetic-c4-g3-c3-c4` (default) | C4 | G3 = C4·3/4 | C3 = C4/2 | C4 (octave above string 2) |
| `synthetic-c4-f3-c3-c4` | C4 | F3 = C4·2/3 | C3 | C4 |

Courses are performance groupings only: `{0}`, `{1}`, `{2, 3}`. Strings 2 and 3 form
the octave pair. Each keeps its own physical state. The stroke order inside a course
is listed for strokes in +h, which points toward string 3.

## Fret maps

Fret positions use the ideal-string relation \(x=L(1-1/r)\) for a frequency ratio
\(r\) above the open string, over string 0's speaking length. All strings share them,
like tied frets. This ignores pitch raised by stretching the string when pressed,
bending stiffness and fret height. The engine can show those effects; this map does
not compensate for them.

### `pythagorean-17-safi-al-din` (default, 25 frets)

The ratio set is the Pythagorean chain of fifths \(3^k\), \(k=-12,\dots,4\), each
brought into one octave by powers of two. That gives 17 steps per octave: twelve
limmas (256/243, about 90.22 cents) and five Pythagorean commas (531441/524288, about
23.46 cents), in the per-octave order L L C L L C L | L L C | L L C L L L C. Frets
18–25 repeat the first eight ratios doubled. Status: *"historical theory interval set
used as a synthetic fret map; not a measured setar fret map and not a statement of
performance practice"*.

Sourcing:

- The 17-tone octave division is commonly credited to Safi al-Din al-Urmawi's
  13th-century *Kitab al-Adwar*. The smallest interval (*bakiye*) is described as
  ranging up to the Pythagorean limma 256:243. See the reproduced
  *Kitab al-Adwar* fret diagram in
  [Tot, Bol Processor archive](https://bolprocessor.org/misc/figs/Antonije_Tot.pdf).
  Interval identities and cents follow
  [Pythagorean interval (Wikipedia)](https://en.wikipedia.org/wiki/Pythagorean_interval).
- The exact 17 ratios used here are a modern reconstruction derived from the
  construction above. A
  [Scala-style listing (Flutopedia)](https://www.flutopedia.com/csc_7tone_alt.htm)
  gives the same kind of 17-note Pythagorean set. The primary treatise was **not**
  consulted for this preset. Some scholarship questions whether the abjad notation
  should be read as a fixed Pythagorean 17-tone system at all; see the abstract
  [*The Abjad Notation and the Presentation of Cycles*](https://avesis.hacettepe.edu.tr/yayin/adbf4d13-adf8-4cfd-8b9f-b7f3e86b452c/the-abjad-notation-and-the-presentation-of-cycles).
  This preset is therefore labeled a historical-theory reconstruction, not a setar
  fret layout.

`preset.fret_positions` builds the chain of fifths again in exact integer arithmetic
and requires every stored numerator/denominator to match. It also checks that each
first-octave step is exactly a limma or a comma, with 12 limmas and 5 commas.

On the default 0.68 m length the frets run from **34.5 mm** (256/243) to **437.9 mm**
(2·1024/729). Comma steps put frets close together: 65536/59049 and 9/8 are
8.2 mm apart in the first octave and about 4.1 mm apart in the second. A gesture that
puts the finger pad a fixed distance behind a fret must use an offset smaller than
the gap to the previous fret. Otherwise the pad sits on, or behind, the previous
fret. This is a known problem of using a full theoretical interval set as frets. Real
setars carry a subset.

### `edo24-quarter-tone-grid` (25 frets)

This map has every 24-EDO step from 2 to 26 (100–1300 cents), giving frets from
38.2 mm to 359.1 mm. Status: *"equal-division approximation for engineering tests
only; synthetic, not a setar fret map"*. It exists to test the engine on uniform
quarter-tone spacing. It is not an approximation of any documented setar fret
placement.

## Bridge and contact laws

The bridge, finger, nail and fret laws are also synthetic. They were chosen so that
forces in the 0.1–5 N range give the penetrations in the table. The contact law forms
are defined in [`config.hpp`](../../include/setar/instrument/config.hpp).

| Quantity | Value | Rationale |
| --- | --- | --- |
| Bridge spring, vertical and horizontal | 1e6 N/m, damping 0 | Much stiffer than the string point stiffness at the plucking point (~3e2 N/m), so the bridge acts nearly rigid apart from body motion. |
| Fret contact | K = 1e9 N/m^1.5, α = 1.5, R = 0.05 N·s/m | 2 N gives about 1.6 µm penetration, so the fret is effectively rigid. R is below the string wave impedance (~0.09 N·s/m), so buzz contacts are lossy but do not absorb the waves. |
| Finger pad | K = 5e4 N/m^1.5, α = 1.5, R = 0.8 N·s/m | About 2.9 N at 1.5 mm compression (soft flesh). R is much larger than the string impedance, so a light touch strongly damps the string except near nodes (harmonics). |
| Nail | K = 1e6 N/m^1.5, α = 1.5, R = 0.02 N·s/m | 1 N gives about 63 µm penetration: stiff compared with the string, but not rigid. Finger compliance is not modeled separately. |
| Finger lateral damping | 0.5 N·s/m | Horizontal flesh damping while the pad touches the string. |
| Fret stick spring | 1e5 N/m, damping 0.05 N·s/m | Horizontal pinning while the string rests on a fret, in place of friction. |
| Mode truncation | 18 kHz | Declared reduction; the engine also caps modes at 0.45·fs. |

## Tests and acceptance evidence

| CTest | What it checks |
| --- | --- |
| `preset.tension_inverse` | Independent forward stiff-string frequency matches each target to a relative \(10^{-12}\). Checks the ideal-string limit and rejects infeasible tension. Recomputes the plain-string \(\mu\) and \(EI\). Tensions lie in 10–60 N. |
| `preset.fret_positions` | Fret positions strictly increase. \(L/(L-x)\) reproduces each ratio to \(10^{-12}\). Exact chain-of-fifths fractions. 12 limma + 5 comma steps. 24-EDO ratios. |
| `preset.config_admissibility` | Configs pass engine `validate()` with and without the body, and with the alternative tuning and fret map. Labels and statuses say "synthetic". Courses are {0}, {1}, {2,3}. Configured strings reproduce the tuning. 25 frets lie before the bridge. |
| `preset.tuning_cents` | Octave = 1200, unison = 0, fifth, limma ≈ 90.225, comma ≈ 23.460, quarter tone = 50 cents; tuning interval relations. |
| `preset.invalid_inputs` | Rejects non-finite, zero and negative inputs, unknown ids, empty maps, and non-increasing or unison ratios. |

These tests verify arithmetic and labeling only. They do not show that any value
represents a setar.

## Limitations and next actions

- Replace synthetic diameters, materials and tensions with measured records, including
  uncertainty and provenance, for an identified specimen. Wound strings need a
  dispersion fit, not \(d^4\).
- Replace the ideal-string fret placement with measured fret positions. Add
  pressing-sharpening compensation once the engine's fretted-pitch tests quantify the
  offset.
- Obtain a practitioner-reviewed tuning and fret-map set (REQ-PRF-002). The current
  ids are engineering placeholders.
- Contact stiffness and damping values need identification experiments
  ([finite nail contact](../contacts/01-finite-nail-contact.md)).
