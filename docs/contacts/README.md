# Contact mechanics: nail, finger, fret, and release

Contact is where performance intent becomes mechanical work. This volume proposes
models and experiments for that translation. It does not claim that a formula
for a contact potential is already a stable implementation, or that a generic
guitar contact model has been validated for Iranian setar technique.

Read the chapters in this dependency order:

1. [Finite fingernail contact and release](01-finite-nail-contact.md) establishes
   gap, normal, potential, moving geometry, and work conventions.
2. [Friction, flesh, and stroke direction](02-friction-flesh-direction.md) adds
   tangential behavior and distinguishes physical geometry from a tone knob.
3. [Multi-string sweeps](03-multi-string-sweeps.md) makes one hand trajectory
   interact with four persistent strings rather than triggering independent notes.
4. [Compliant frets and stopping](04-compliant-frets-and-stopping.md) models a
   changed constraint on an existing string instead of simply scaling pitch.
5. [Finger damping and release](05-finger-damping-and-release.md) treats energy
   removal, partial stops, and release transients explicitly.
6. [Buzz and contact validation](06-buzz-and-contact-validation.md) separates
   physical intermittent contact from numerical chatter and cosmetic noise.

The numerical reference is intentionally a separate concern. Consult
[the numerics volume](../numerics/) before choosing a time integrator. A passive
continuous force law does not make arbitrary explicit force insertion passive.
The implicit mass-matrix and discrete-gradient reference must be derived as a
combined system; it must not be spliced into an explicit string update without a
new argument and tests.

Primary readings include Pavlidou and Richardson's [string–finger interaction
study](https://www.ioa.org.uk/system/files/proceedings/m_pavlidou_and_be_richardson_the_string-finger_interaction_in_the_classical_guitar_theoretical_model_and_exper.pdf),
which considers multi-directional motion and nail friction in a guitar context;
Bilbao, Torin, and Chatziioannou's [2015 collision
paper](https://doi.org/10.3813/AAA.918813), which develops energy-based collision
methods; and Bilbao and Torin's [articulated string/fretboard
study](https://www.research.ed.ac.uk/en/publications/numerical-modeling-and-sound-synthesis-for-articulated-stringfret/).
Read the relevant model and numerical sections, not only the abstracts. These
sources motivate methods. They do not provide measured setar nail stiffness,
friction, fret compliance, or gesture trajectories.

For the Pavlidou–Richardson paper, the primary indexed opening identifies
Proceedings of the Institute of Acoustics, volume 19, part 5 (1997), and section 2,
The Model. The direct PDF request returned an access error during this pass even
though the primary text was indexed. Treat that as a retrieval limitation and
obtain the full original before reproducing its equations; do not silently
substitute an unrelated secondary implementation.

Every contact experiment needs a coordinate frame, a signed gap convention,
identified or synthetic material parameters, a velocity definition, and a work
ledger. Moving contact locations require the velocity associated with that
motion; interpolating string velocity alone is insufficient. Rotating contact
frames require equally careful treatment of normal and tangential motion.

An audio callback cannot run an unbounded contact search or nonlinear solve.
The proposed real-time path therefore has prepared geometry, bounded candidate
sets, explicit iteration limits, and a verified failure policy. A hard limit on
iterations makes execution bounded; it does not make an unconverged force
correct. Diagnostics must distinguish capacity failure, solver failure,
out-of-domain input, and ordinary separation.
