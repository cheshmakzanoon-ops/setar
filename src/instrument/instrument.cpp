// Four-string setar prototype engine. Governing equations, time levels and the
// energy ledger are derived in docs/numerics/15-modal-contact-solver.md.
//
// Per sample (time level n -> n+1):
//   1. every modal bank predicts its free update q_free[n+1] = a q[n] - b q[n-1];
//   2. prescribed actuator positions u[n+1] are advanced (nail surface, finger pads);
//   3. the active interaction channels z_c = B_c q - u_c are assembled in priority order
//      (bridge springs, nails, finger pads, fret obstacles, fret stick springs, finger
//      lateral dampers);
//   4. s = z[n+1] - z[n-1] solves s - b - M F(s) = 0 (SmallNewtonSolver, discrete gradient);
//   5. q[n+1] = q_free[n+1] + G B^T F; excluded fret candidates are re-checked once;
//   6. observation p[n] = sum_j w_j (eta[n+1] - 2 eta[n] + eta[n-1]) / dt^2, i.e. output
//      sample n is available after level n+1 is computed (one-sample internal latency);
//   7. banks commit, actuator histories advance.
// Every buffer is sized in prepare(); process() performs no allocation and never throws.

#include "setar/instrument/instrument.hpp"

#include "setar/instrument/numerics.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace setar::instrument {
namespace {

namespace nm = numerics;
constexpr double pi = std::numbers::pi_v<double>;
constexpr std::size_t bank_count = 2 * string_count + 1;
constexpr std::uint32_t body_bank = 2 * string_count;
constexpr std::uint32_t no_key = std::numeric_limits<std::uint32_t>::max();
constexpr std::size_t bridge_channels = string_count * polarization_count;
// A fret candidate is assembled when its gap is below this at any of the three levels.
constexpr double fret_margin_m = 1.0e-6;
constexpr std::size_t max_modes_per_string = 4096;

enum class ChannelKind : std::uint8_t { Bridge, Nail, FingerPad, FingerLateral, Fret, Stick };

struct Term {
    std::uint32_t bank = 0;
    const double* shape = nullptr;
    double coefficient = 0.0;
    // String bank: index into the string's static shapes (0 bridge, 1+f fret f), -1 dynamic.
    // Body bank: bridge channel index s*2+p.
    std::int32_t static_id = -1;
};

struct Channel {
    ChannelKind kind = ChannelKind::Bridge;
    std::uint8_t string_index = 0;
    std::uint8_t slot = 0;
    std::uint32_t fret = 0;
    std::uint32_t key = no_key;
    std::uint32_t term_count = 0;
    std::array<Term, 2> terms{};
    nm::ChannelLaw law{};
    double u_prev = 0.0;
    double u_cur = 0.0;
    double u_next = 0.0;
    double z_prev = 0.0;
    double z_cur = 0.0;
    double z_free = 0.0;
};

struct NailState {
    NailPhase phase = NailPhase::Idle;
    double x_m = 0.0;
    double dv = 0.0;
    double dh = 0.0;
    double speed = 0.0;
    double force_limit = 0.0;
    double travel_limit = 0.0;
    double u_start = 0.0;
    double u_prev = 0.0;
    double u_cur = 0.0;
    double u_next = 0.0;
    double force = 0.0;
    bool restarted = false;
    [[nodiscard]] bool active() const noexcept {
        return phase == NailPhase::Approaching || phase == NailPhase::InContact;
    }
};

struct FingerState {
    bool active = false;
    bool releasing = false;
    double x_m = 0.0;
    double x_target = 0.0;
    double slide_speed = 0.0;
    double u_prev = 0.0;
    double u_cur = 0.0;
    double u_next = 0.0;
    double height_target = 0.0;
    double pad_speed = 0.0;
    double release_gap = 0.0;
    double force = 0.0;
};

struct StringData {
    std::size_t modes = 0;
    double total_length_m = 0.0;
    std::vector<double> bridge_shape;
    std::vector<double> fret_shapes;      // frets * modes
    std::vector<double> fret_height;      // fret-top vertical coordinate (negative)
    std::vector<double> static_compliance; // (frets+1)^2, string-bank part
    double min_fret_gap = std::numeric_limits<double>::infinity();
    std::vector<double> nail_shape;
    std::array<std::vector<double>, 2> finger_shape;
    std::vector<double> stick_anchor;     // per fret
    std::vector<std::uint64_t> fret_stamp;  // step at which the fret channel was assembled
    std::vector<std::uint64_t> stick_stamp; // step at which the stick channel was assembled
    // Rigorous lower bound of each fret gap at the current level n: set to the exact gap when
    // evaluated, then decreased every step by sum|q[n+1]-q[n]| >= |gap change| (|phi| <= 1).
    std::vector<double> fret_slack;
};

void require(bool ok, const std::string& message) {
    if (!ok) {
        throw std::invalid_argument(message);
    }
}

double move_toward(double value, double target, double step) noexcept {
    if (target > value) {
        return std::min(target, value + step);
    }
    return std::max(target, value - step);
}

void note_fault(ProcessSummary& summary, FaultCode code) noexcept {
    if (summary.first_fault == FaultCode::None) {
        summary.first_fault = code;
    }
}

} // namespace

struct Instrument::Impl {
    InstrumentConfig config{};
    ProcessSpec spec{};
    PreparedInfo info{};
    double dt = 0.0;
    std::array<nm::ModalBank, bank_count> banks{};
    std::array<StringData, string_count> strings{};
    std::size_t fret_count = 0;
    std::array<std::vector<double>, bridge_channels> body_shapes{};
    std::array<double, bridge_channels * bridge_channels> body_compliance{};
    std::array<double, bridge_channels * bridge_channels> bridge_block{}; // full bridge-bridge M
    std::array<std::vector<double>, observation_count> radiation{}; // w * gain / dt^2
    std::array<NailState, string_count> nails{};
    std::array<std::array<FingerState, 2>, string_count> fingers{};
    std::array<std::array<double, polarization_count>, string_count> bridge_force{};

    std::vector<Channel> channels;
    std::size_t channel_count = 0;
    std::vector<double> compliance, rhs, zprev_buf, sol, force;
    // Schur-complement workspace (see solve()).
    std::vector<std::size_t> linear_index, nonlinear_index, lin_pivot;
    std::vector<double> alpha, beta, lin_matrix, work_vector, force_l0, coupling, reduced_rhs,
        reduced_compliance, reduced_z_prev, reduced_s, reduced_force;
    std::vector<nm::ChannelLaw> reduced_law;
    // Warm start of contact channels from the previous step (per ledger key).
    std::vector<double> key_s;
    std::vector<std::uint64_t> key_contact_stamp;
    std::vector<nm::ChannelLaw> laws;
    nm::SmallNewtonSolver solver;

    // Ledger state.
    EnergyLedger ledger{};
    bool baseline_pending = true;
    std::vector<double> key_value;
    std::vector<std::uint64_t> key_stamp;
    std::vector<std::uint8_t> key_dissipative;
    std::vector<std::uint32_t> last_keys;
    std::size_t last_key_count = 0;
    std::uint64_t step_counter = 0;
    std::uint64_t sample_clock = 0;
    std::uint32_t last_active_channels = 0;

    [[nodiscard]] std::span<const double> shape(const Term& t) const noexcept {
        return {t.shape, banks[t.bank].size()};
    }

    [[nodiscard]] double gather(const Channel& c, int level) const noexcept {
        double out = 0.0;
        for (std::uint32_t i = 0; i < c.term_count; ++i) {
            const Term& t = c.terms[i];
            const nm::ModalBank& bank = banks[t.bank];
            const double v = level == 0   ? bank.gather_previous(shape(t))
                             : level == 1 ? bank.gather_current(shape(t))
                                          : bank.gather_next(shape(t));
            out += t.coefficient * v;
        }
        return out;
    }

    [[nodiscard]] double pair_compliance(const Channel& a, const Channel& b) const noexcept {
        if (a.kind == ChannelKind::Bridge && b.kind == ChannelKind::Bridge) {
            return bridge_block[a.key * bridge_channels + b.key]; // constant, built in prepare()
        }
        double out = 0.0;
        for (std::uint32_t i = 0; i < a.term_count; ++i) {
            const Term& ta = a.terms[i];
            for (std::uint32_t j = 0; j < b.term_count; ++j) {
                const Term& tb = b.terms[j];
                if (ta.bank != tb.bank) {
                    continue;
                }
                double v = 0.0;
                if (ta.bank == body_bank) {
                    v = body_compliance[static_cast<std::size_t>(ta.static_id) * bridge_channels +
                                        static_cast<std::size_t>(tb.static_id)];
                } else if (ta.static_id >= 0 && tb.static_id >= 0) {
                    const StringData& sd = strings[ta.bank / 2];
                    v = sd.static_compliance[static_cast<std::size_t>(ta.static_id) * (fret_count + 1) +
                                             static_cast<std::size_t>(tb.static_id)];
                } else {
                    v = banks[ta.bank].compliance(shape(ta), shape(tb));
                }
                out += ta.coefficient * tb.coefficient * v;
            }
        }
        return out;
    }

    void zero_state() noexcept {
        for (auto& bank : banks) {
            bank.reset();
        }
        for (auto& n : nails) {
            n = NailState{};
        }
        for (auto& pair : fingers) {
            for (auto& f : pair) {
                f = FingerState{};
            }
        }
        for (auto& sd : strings) {
            std::fill(sd.fret_stamp.begin(), sd.fret_stamp.end(), std::uint64_t{0});
            std::fill(sd.stick_stamp.begin(), sd.stick_stamp.end(), std::uint64_t{0});
            std::fill(sd.fret_slack.begin(), sd.fret_slack.end(), -std::numeric_limits<double>::infinity());
        }
        for (auto& row : bridge_force) {
            row.fill(0.0);
        }
        ledger = EnergyLedger{};
        baseline_pending = true;
        last_key_count = 0;
        std::fill(key_stamp.begin(), key_stamp.end(), std::uint64_t{0});
        std::fill(key_contact_stamp.begin(), key_contact_stamp.end(), std::uint64_t{0});
        // Step stamps start at 2 so that stamp 0 never means "present last step".
        step_counter = 2;
        last_active_channels = 0;
    }

    [[nodiscard]] std::uint32_t nail_key(std::size_t s) const noexcept {
        return static_cast<std::uint32_t>(bridge_channels + s);
    }
    [[nodiscard]] std::uint32_t finger_key(std::size_t s, std::size_t slot) const noexcept {
        return static_cast<std::uint32_t>(bridge_channels + string_count + s * 2 + slot);
    }
    [[nodiscard]] std::uint32_t fret_key(std::size_t s, std::size_t f) const noexcept {
        return static_cast<std::uint32_t>(bridge_channels + 3 * string_count + s * fret_count + f);
    }
    [[nodiscard]] std::uint32_t stick_key(std::size_t s, std::size_t f) const noexcept {
        return static_cast<std::uint32_t>(bridge_channels + 3 * string_count +
                                          string_count * fret_count + s * fret_count + f);
    }

    // Returns nullptr (and counts) when the channel capacity is exhausted.
    Channel* push_channel(ProcessSummary& summary) noexcept {
        if (channel_count >= channels.size()) {
            ++summary.capacity_overflows;
            note_fault(summary, FaultCode::ChannelCapacityExceeded);
            return nullptr;
        }
        Channel& c = channels[channel_count++];
        c = Channel{};
        return &c;
    }

    // One pass over each term's modes for all three levels; same summation order as
    // ModalBank::gather_*, hence bitwise-identical values.
    void finish_channel(Channel& c) const noexcept {
        double zp = 0.0;
        double zc = 0.0;
        double zn = 0.0;
        for (std::uint32_t i = 0; i < c.term_count; ++i) {
            const Term& t = c.terms[i];
            const nm::ModalBank& bank = banks[t.bank];
            const double* prev = bank.previous().data();
            const double* cur = bank.current().data();
            const double* next = bank.next().data();
            const double* w = t.shape;
            double a = 0.0;
            double b = 0.0;
            double d = 0.0;
            for (std::size_t k = 0; k < bank.size(); ++k) {
                a += prev[k] * w[k];
                b += cur[k] * w[k];
                d += next[k] * w[k];
            }
            zp += t.coefficient * a;
            zc += t.coefficient * b;
            zn += t.coefficient * d;
        }
        c.z_prev = zp - c.u_prev;
        c.z_cur = zc - c.u_cur;
        c.z_free = zn - c.u_next;
    }

    void assemble(ProcessSummary& summary) noexcept {
        channel_count = 0;
        const ContactConfig& cc = config.contacts;
        // 1. Bridge springs (always present; grounded when the body has no modes).
        for (std::size_t s = 0; s < string_count; ++s) {
            for (std::size_t p = 0; p < polarization_count; ++p) {
                Channel* c = push_channel(summary);
                if (c == nullptr) {
                    return;
                }
                c->kind = ChannelKind::Bridge;
                c->string_index = static_cast<std::uint8_t>(s);
                c->key = static_cast<std::uint32_t>(s * 2 + p);
                const std::int32_t bridge_id = static_cast<std::int32_t>(s * 2 + p);
                c->terms[0] = Term{static_cast<std::uint32_t>(s * 2 + p), strings[s].bridge_shape.data(),
                                   1.0, 0};
                c->term_count = 1;
                if (banks[body_bank].size() > 0) {
                    c->terms[1] = Term{body_bank, body_shapes[s * 2 + p].data(), -1.0, bridge_id};
                    c->term_count = 2;
                }
                c->law.kind = nm::ChannelLawKind::LinearSpring;
                c->law.stiffness = config.bridge.stiffness_n_per_m[p];
                c->law.damping_n_s_per_m = config.bridge.damping_n_s_per_m[p];
                finish_channel(*c);
            }
        }
        // 2. Nails.
        for (std::size_t s = 0; s < string_count; ++s) {
            NailState& nail = nails[s];
            if (!nail.active()) {
                continue;
            }
            Channel* c = push_channel(summary);
            if (c == nullptr) {
                continue;
            }
            c->kind = ChannelKind::Nail;
            c->string_index = static_cast<std::uint8_t>(s);
            c->key = nail_key(s);
            if (nail.dv != 0.0) {
                c->terms[c->term_count++] =
                    Term{static_cast<std::uint32_t>(2 * s), strings[s].nail_shape.data(), nail.dv, -1};
            }
            if (nail.dh != 0.0) {
                c->terms[c->term_count++] = Term{static_cast<std::uint32_t>(2 * s + 1),
                                                 strings[s].nail_shape.data(), nail.dh, -1};
            }
            c->law.kind = nm::ChannelLawKind::OneSidedPower;
            c->law.stiffness = cc.nail.stiffness;
            c->law.exponent = cc.nail.exponent;
            c->u_prev = nail.u_prev;
            c->u_cur = nail.u_cur;
            c->u_next = nail.u_next;
            finish_channel(*c);
            c->law.damping_n_s_per_m = c->z_cur < 0.0 ? cc.nail.damping_n_s_per_m : 0.0;
        }
        // 3. Finger pads (vertical): z = -p_v + u_pad, penetration = p_v - u_pad.
        for (std::size_t s = 0; s < string_count; ++s) {
            for (std::size_t slot = 0; slot < 2; ++slot) {
                const FingerState& f = fingers[s][slot];
                if (!f.active) {
                    continue;
                }
                Channel* c = push_channel(summary);
                if (c == nullptr) {
                    continue;
                }
                c->kind = ChannelKind::FingerPad;
                c->string_index = static_cast<std::uint8_t>(s);
                c->slot = static_cast<std::uint8_t>(slot);
                c->key = finger_key(s, slot);
                c->terms[0] = Term{static_cast<std::uint32_t>(2 * s), strings[s].finger_shape[slot].data(),
                                   -1.0, -1};
                c->term_count = 1;
                c->law.kind = nm::ChannelLawKind::OneSidedPower;
                c->law.stiffness = cc.finger.stiffness;
                c->law.exponent = cc.finger.exponent;
                c->u_prev = -f.u_prev;
                c->u_cur = -f.u_cur;
                c->u_next = -f.u_next;
                finish_channel(*c);
                c->law.damping_n_s_per_m = c->z_cur < 0.0 ? cc.finger.damping_n_s_per_m : 0.0;
            }
        }
        // 4. Fret obstacles, screened by a modal amplitude bound (|phi| <= 1).
        for (std::size_t s = 0; s < string_count && fret_count > 0; ++s) {
            assemble_frets(s, summary);
        }
        // 5. Stick spring at the stopping fret of each pressing pad (the first fret on the
        //    bridge side of the pad). It engages when that fret is in contact at level n and
        //    is held while the pad keeps pressing, so transient fret buzz stays frictionless.
        // 6. Lateral finger damping while a pad is in contact.
        const std::size_t base_count = channel_count;
        const bool stick_enabled =
            cc.fret_stick_stiffness_n_per_m > 0.0 || cc.fret_stick_damping_n_s_per_m > 0.0;
        for (std::size_t i = 0; i < base_count && stick_enabled; ++i) {
            const Channel& pad = channels[i];
            if (pad.kind != ChannelKind::FingerPad || pad.z_cur >= 0.0) {
                continue;
            }
            const std::size_t si = pad.string_index;
            const double x_pad = fingers[si][pad.slot].x_m;
            std::size_t fi = 0;
            while (fi < fret_count && config.frets[fi].position_m <= x_pad) {
                ++fi;
            }
            if (fi == fret_count || strings[si].stick_stamp[fi] == step_counter) {
                continue;
            }
            const bool held = strings[si].stick_stamp[fi] + 1 == step_counter;
            bool fret_contact = false;
            for (std::size_t j = 0; j < base_count; ++j) {
                const Channel& fc = channels[j];
                fret_contact = fret_contact || (fc.kind == ChannelKind::Fret && fc.string_index == si &&
                                                fc.fret == fi && fc.z_cur < 0.0);
            }
            if (!held && !fret_contact) {
                continue;
            }
            Channel* c = push_channel(summary);
            if (c == nullptr) {
                continue;
            }
            c->kind = ChannelKind::Stick;
            c->string_index = static_cast<std::uint8_t>(si);
            c->fret = static_cast<std::uint32_t>(fi);
            c->key = stick_key(si, fi);
            c->terms[0] = Term{static_cast<std::uint32_t>(2 * si + 1),
                               strings[si].fret_shapes.data() + fi * strings[si].modes, 1.0,
                               static_cast<std::int32_t>(fi + 1)};
            c->term_count = 1;
            c->law.kind = nm::ChannelLawKind::LinearSpring;
            c->law.stiffness = cc.fret_stick_stiffness_n_per_m;
            c->law.damping_n_s_per_m = cc.fret_stick_damping_n_s_per_m;
            finish_channel(*c);
            if (!held) {
                // Onset anchor at level n-1/2: minimizes the booked switching energy
                // K/8*(z[n]-z[n-1])^2 of a spring born between two time levels.
                strings[si].stick_anchor[fi] = 0.5 * (c->z_cur + c->z_prev);
            }
            strings[si].stick_stamp[fi] = step_counter;
            c->law.anchor_m = strings[si].stick_anchor[fi];
        }
        if (cc.finger_lateral_damping_n_s_per_m > 0.0) {
            for (std::size_t i = 0; i < base_count; ++i) {
                const Channel& src = channels[i];
                if (src.kind != ChannelKind::FingerPad || src.z_cur >= 0.0) {
                    continue;
                }
                const std::size_t s = src.string_index;
                Channel* c = push_channel(summary);
                if (c == nullptr) {
                    continue;
                }
                c->kind = ChannelKind::FingerLateral;
                c->string_index = src.string_index;
                c->slot = src.slot;
                c->terms[0] = Term{static_cast<std::uint32_t>(2 * s + 1),
                                   strings[s].finger_shape[src.slot].data(), 1.0, -1};
                c->term_count = 1;
                c->law.kind = nm::ChannelLawKind::Damper;
                c->law.damping_n_s_per_m = cc.finger_lateral_damping_n_s_per_m;
                finish_channel(*c);
            }
        }
    }

    // Bounds of the vertical string displacement anywhere: |p(x)| <= amplitude at level n
    // and |p_level(x) - p_n(x)| <= change for the other two levels.
    void vertical_bounds(std::size_t s, double& amplitude, double& change) const noexcept {
        const nm::ModalBank& bank = banks[2 * s];
        const auto prev = bank.previous();
        const auto cur = bank.current();
        const auto next = bank.next();
        amplitude = 0.0;
        change = 0.0;
        for (std::size_t i = 0; i < cur.size(); ++i) {
            amplitude += std::abs(cur[i]);
            change += std::max(std::abs(prev[i] - cur[i]), std::abs(next[i] - cur[i]));
        }
    }

    void add_fret_channel(std::size_t s, std::size_t f, double z_cur, ProcessSummary& summary) noexcept {
        StringData& sd = strings[s];
        Channel* c = push_channel(summary);
        if (c == nullptr) {
            return;
        }
        const ContactConfig& cc = config.contacts;
        c->kind = ChannelKind::Fret;
        c->string_index = static_cast<std::uint8_t>(s);
        c->fret = static_cast<std::uint32_t>(f);
        c->key = fret_key(s, f);
        c->terms[0] = Term{static_cast<std::uint32_t>(2 * s), sd.fret_shapes.data() + f * sd.modes, 1.0,
                           static_cast<std::int32_t>(f + 1)};
        c->term_count = 1;
        c->law.kind = nm::ChannelLawKind::OneSidedPower;
        c->law.stiffness = cc.fret.stiffness;
        c->law.exponent = cc.fret.exponent;
        c->u_prev = c->u_cur = c->u_next = sd.fret_height[f];
        c->z_prev = gather(*c, 0) - c->u_prev;
        c->z_cur = z_cur;
        c->z_free = gather(*c, 2) - c->u_next;
        c->law.damping_n_s_per_m = z_cur < 0.0 ? cc.fret.damping_n_s_per_m : 0.0;
        sd.fret_stamp[f] = step_counter;
    }

    void assemble_frets(std::size_t s, ProcessSummary& summary) noexcept {
        StringData& sd = strings[s];
        double amplitude = 0.0;
        double change = 0.0;
        vertical_bounds(s, amplitude, change);
        if (amplitude + change < sd.min_fret_gap - fret_margin_m) {
            return;
        }
        const nm::ModalBank& bank = banks[2 * s];
        for (std::size_t f = 0; f < fret_count; ++f) {
            if (sd.fret_slack[f] - change >= fret_margin_m) {
                continue; // the exact test below would also skip
            }
            const std::span<const double> shp(sd.fret_shapes.data() + f * sd.modes, sd.modes);
            const double z_cur = bank.gather_current(shp) - sd.fret_height[f];
            sd.fret_slack[f] = z_cur;
            if (z_cur - change >= fret_margin_m) {
                continue;
            }
            const double z_prev = bank.gather_previous(shp) - sd.fret_height[f];
            const double z_free = bank.gather_next(shp) - sd.fret_height[f];
            if (std::min({z_prev, z_cur, z_free}) < fret_margin_m) {
                add_fret_channel(s, f, z_cur, summary);
            }
        }
    }

    // After a scatter: frets not assembled this step that now penetrate at level n+1.
    bool add_violating_frets(ProcessSummary& summary) noexcept {
        bool added = false;
        for (std::size_t s = 0; s < string_count && fret_count > 0; ++s) {
            StringData& sd = strings[s];
            const nm::ModalBank& bank = banks[2 * s];
            double amplitude = 0.0;
            double change = 0.0;
            const auto cur = bank.current();
            const auto next = bank.next();
            for (std::size_t i = 0; i < cur.size(); ++i) {
                amplitude += std::abs(cur[i]);
                change += std::abs(next[i] - cur[i]);
            }
            if (amplitude + change < sd.min_fret_gap) {
                continue;
            }
            for (std::size_t f = 0; f < fret_count; ++f) {
                if (sd.fret_stamp[f] == step_counter || sd.fret_slack[f] - change >= 0.0) {
                    continue;
                }
                const std::span<const double> shp(sd.fret_shapes.data() + f * sd.modes, sd.modes);
                const double z_cur = bank.gather_current(shp) - sd.fret_height[f];
                if (z_cur - change >= 0.0) {
                    continue;
                }
                if (bank.gather_next(shp) - sd.fret_height[f] < 0.0) {
                    add_fret_channel(s, f, z_cur, summary);
                    added = true;
                }
            }
        }
        return added;
    }

    // In-place LU with partial pivoting of the n x n row-major matrix `a`; false if singular.
    static bool lu_factor(double* a, std::size_t* piv, std::size_t n) noexcept {
        for (std::size_t col = 0; col < n; ++col) {
            std::size_t best = col;
            for (std::size_t r = col + 1; r < n; ++r) {
                if (std::abs(a[r * n + col]) > std::abs(a[best * n + col])) {
                    best = r;
                }
            }
            piv[col] = best;
            if (best != col) {
                for (std::size_t c = 0; c < n; ++c) {
                    std::swap(a[col * n + c], a[best * n + c]);
                }
            }
            const double pivot = a[col * n + col];
            if (pivot == 0.0 || !std::isfinite(pivot)) {
                return false;
            }
            for (std::size_t r = col + 1; r < n; ++r) {
                const double factor = a[r * n + col] / pivot;
                a[r * n + col] = factor;
                if (factor != 0.0) {
                    for (std::size_t c = col + 1; c < n; ++c) {
                        a[r * n + c] -= factor * a[col * n + c];
                    }
                }
            }
        }
        return true;
    }

    static void lu_solve(const double* a, const std::size_t* piv, std::size_t n, double* x) noexcept {
        for (std::size_t r = 0; r < n; ++r) {
            std::swap(x[r], x[piv[r]]);
            double sum = x[r];
            for (std::size_t c = 0; c < r; ++c) {
                sum -= a[r * n + c] * x[c];
            }
            x[r] = sum;
        }
        for (std::size_t r = n; r-- > 0;) {
            double sum = x[r];
            for (std::size_t c = r + 1; c < n; ++c) {
                sum -= a[r * n + c] * x[c];
            }
            x[r] = sum / a[r * n + r];
        }
    }

    // Solves s - b - M F(s) = 0 for all channels. Linear channels (bridge/stick springs,
    // dampers: F = alpha - beta*s) are eliminated exactly by a Schur complement, so the
    // Newton iteration only runs on the one-sided contacts with the reduced compliance
    // M' = M_NN - M_NL (beta^-1 + M_LL)^-1 M_LN (still symmetric positive semidefinite).
    // Initial guesses for contacts are taken from sol[] (free motion or warm start).
    nm::NewtonResult solve() noexcept {
        const std::size_t m = channel_count;
        for (std::size_t r = 0; r < m; ++r) {
            const Channel& a = channels[r];
            for (std::size_t c = r; c < m; ++c) {
                const double v = pair_compliance(a, channels[c]);
                compliance[r * m + c] = v;
                compliance[c * m + r] = v;
            }
            rhs[r] = a.z_free - a.z_prev;
        }
        std::size_t ml = 0;
        std::size_t mn = 0;
        for (std::size_t c = 0; c < m; ++c) {
            const nm::ChannelLaw& law = channels[c].law;
            if (law.kind == nm::ChannelLawKind::OneSidedPower) {
                nonlinear_index[mn++] = c;
            } else {
                linear_index[ml++] = c;
                const double k = law.kind == nm::ChannelLawKind::LinearSpring ? law.stiffness : 0.0;
                beta[c] = 0.5 * k + law.damping_n_s_per_m / (2.0 * dt);
                alpha[c] = -k * (channels[c].z_prev - law.anchor_m);
            }
        }
        auto M = [&](std::size_t r, std::size_t c) { return compliance[r * m + c]; };
        // A = I + M_LL * beta, factorized once.
        for (std::size_t i = 0; i < ml; ++i) {
            for (std::size_t j = 0; j < ml; ++j) {
                lin_matrix[i * ml + j] = (i == j ? 1.0 : 0.0) + M(linear_index[i], linear_index[j]) * beta[linear_index[j]];
            }
        }
        nm::NewtonResult result;
        if (!lu_factor(lin_matrix.data(), lin_pivot.data(), ml)) {
            result.finite = false;
            return result;
        }
        // F_L0 = alpha - beta * A^-1 (b_L + M_LL alpha).
        for (std::size_t i = 0; i < ml; ++i) {
            double v = rhs[linear_index[i]];
            for (std::size_t j = 0; j < ml; ++j) {
                v += M(linear_index[i], linear_index[j]) * alpha[linear_index[j]];
            }
            work_vector[i] = v;
        }
        lu_solve(lin_matrix.data(), lin_pivot.data(), ml, work_vector.data());
        for (std::size_t i = 0; i < ml; ++i) {
            const std::size_t c = linear_index[i];
            force_l0[i] = alpha[c] - beta[c] * work_vector[i];
        }
        // C = beta * A^-1 * M_LN (ml x mn), column by column.
        for (std::size_t j = 0; j < mn; ++j) {
            for (std::size_t i = 0; i < ml; ++i) {
                work_vector[i] = M(linear_index[i], nonlinear_index[j]);
            }
            lu_solve(lin_matrix.data(), lin_pivot.data(), ml, work_vector.data());
            for (std::size_t i = 0; i < ml; ++i) {
                coupling[i * mn + j] = beta[linear_index[i]] * work_vector[i];
            }
        }
        for (std::size_t i = 0; i < mn; ++i) {
            const std::size_t ci = nonlinear_index[i];
            double v = rhs[ci];
            for (std::size_t l = 0; l < ml; ++l) {
                v += M(ci, linear_index[l]) * force_l0[l];
            }
            reduced_rhs[i] = v;
            for (std::size_t j = 0; j < mn; ++j) {
                double w = M(ci, nonlinear_index[j]);
                for (std::size_t l = 0; l < ml; ++l) {
                    w -= M(ci, linear_index[l]) * coupling[l * mn + j];
                }
                reduced_compliance[i * mn + j] = w;
            }
            reduced_law[i] = channels[ci].law;
            reduced_z_prev[i] = channels[ci].z_prev;
            reduced_s[i] = sol[ci];
        }
        if (mn > 0) {
            result = solver.solve(mn, std::span<const double>(reduced_compliance.data(), mn * mn),
                                  std::span<const double>(reduced_rhs.data(), mn),
                                  std::span<const nm::ChannelLaw>(reduced_law.data(), mn),
                                  std::span<const double>(reduced_z_prev.data(), mn), dt,
                                  std::span<double>(reduced_s.data(), mn),
                                  std::span<double>(reduced_force.data(), mn), spec.max_newton_iterations,
                                  spec.newton_tolerance_m);
        } else {
            result.converged = true;
        }
        for (std::size_t i = 0; i < mn; ++i) {
            sol[nonlinear_index[i]] = reduced_s[i];
            force[nonlinear_index[i]] = reduced_force[i];
        }
        // Back-substitute the linear channels: F_L = F_L0 - C F_N, s_L = b_L + M_L* F.
        for (std::size_t i = 0; i < ml; ++i) {
            double f = force_l0[i];
            for (std::size_t j = 0; j < mn; ++j) {
                f -= coupling[i * mn + j] * reduced_force[j];
            }
            force[linear_index[i]] = f;
        }
        for (std::size_t i = 0; i < ml; ++i) {
            const std::size_t c = linear_index[i];
            double v = rhs[c];
            for (std::size_t k = 0; k < m; ++k) {
                v += M(c, k) * force[k];
            }
            sol[c] = v;
        }
        return result;
    }

    void scatter() noexcept {
        for (std::size_t c = 0; c < channel_count; ++c) {
            const Channel& ch = channels[c];
            for (std::uint32_t i = 0; i < ch.term_count; ++i) {
                const Term& t = ch.terms[i];
                banks[t.bank].add_force(shape(t), t.coefficient * force[c]);
            }
        }
    }

    // Book stored-energy discontinuities caused by changing the channel set or geometry
    // between steps (see the header ledger definition). Returns sum of Vbar_before.
    double book_channel_changes() noexcept {
        double v_before_total = 0.0;
        for (std::size_t c = 0; c < channel_count; ++c) {
            const Channel& ch = channels[c];
            if (ch.key == no_key) {
                continue;
            }
            const double v_before = 0.5 * (nm::channel_potential_j(ch.law, ch.z_cur) +
                                           nm::channel_potential_j(ch.law, ch.z_prev));
            v_before_total += v_before;
            if (baseline_pending) {
                continue;
            }
            const bool was_present = key_stamp[ch.key] + 1 == step_counter;
            if (!was_present) {
                ledger.switching_j += v_before; // onset with nonzero potential
            } else if (ch.kind == ChannelKind::Nail && nails[ch.string_index].restarted) {
                ledger.dissipation_j += key_value[ch.key]; // previous stroke slips off
                ledger.switching_j += v_before;
            } else if (ch.kind == ChannelKind::FingerPad) {
                ledger.external_work_j += v_before - key_value[ch.key]; // pad relocation work
            } else {
                ledger.switching_j += v_before - key_value[ch.key]; // static geometry: roundoff
            }
        }
        for (std::size_t c = 0; c < channel_count; ++c) {
            if (channels[c].key != no_key) {
                key_stamp[channels[c].key] = step_counter;
            }
        }
        if (!baseline_pending) {
            for (std::size_t i = 0; i < last_key_count; ++i) {
                const std::uint32_t k = last_keys[i];
                if (key_stamp[k] == step_counter) {
                    continue;
                }
                // Removed: release losses for nail slip-off, pad lift-off and stick slip.
                if (key_dissipative[k] != 0) {
                    ledger.dissipation_j += key_value[k];
                } else {
                    ledger.switching_j -= key_value[k];
                }
            }
        }
        last_key_count = 0;
        for (std::size_t c = 0; c < channel_count; ++c) {
            if (channels[c].key != no_key) {
                last_keys[last_key_count++] = channels[c].key;
            }
        }
        return v_before_total;
    }

    void step(ProcessSummary& summary, double& out_left, double& out_right) noexcept {
        const bool ledger_on = spec.enable_energy_ledger;
        double h_before = 0.0;
        if (ledger_on) {
            for (const auto& bank : banks) {
                h_before += bank.energy_j();
            }
        }
        for (auto& bank : banks) {
            bank.predict();
        }
        for (auto& nail : nails) {
            if (nail.active()) {
                nail.u_next = nail.u_cur + nail.speed * dt;
            }
        }
        for (auto& pair : fingers) {
            for (auto& f : pair) {
                if (f.active) {
                    const double target = f.releasing ? std::numeric_limits<double>::infinity()
                                                      : f.height_target;
                    f.u_next = move_toward(f.u_cur, target, f.pad_speed * dt);
                }
            }
        }
        assemble(summary);
        for (std::size_t c = 0; c < channel_count; ++c) {
            const Channel& ch = channels[c];
            // Warm start a contact that was penetrating at the end of the previous step.
            const bool warm = ch.key != no_key && key_contact_stamp[ch.key] + 1 == step_counter;
            sol[c] = warm ? key_s[ch.key] : ch.z_free - ch.z_prev;
        }
        nm::NewtonResult result = solve();
        scatter();
        const std::size_t first_pass_channels = channel_count;
        if (result.finite && add_violating_frets(summary)) {
            // Undo the scatter by re-predicting, then solve once more with the added frets.
            for (auto& bank : banks) {
                bank.predict();
            }
            for (std::size_t c = first_pass_channels; c < channel_count; ++c) {
                Channel& ch = channels[c];
                ch.z_free = gather(ch, 2) - ch.u_next;
                sol[c] = ch.z_free - ch.z_prev;
            }
            const nm::NewtonResult second = solve();
            // Report the per-solve maximum so the value never exceeds max_newton_iterations.
            result.iterations = std::max(result.iterations, second.iterations);
            result.residual_m = second.residual_m;
            result.converged = second.converged;
            result.finite = second.finite;
            scatter();
        }
        for (std::size_t c = 0; c < channel_count; ++c) {
            const Channel& ch = channels[c];
            if (ch.key != no_key && ch.law.kind == nm::ChannelLawKind::OneSidedPower &&
                ch.z_prev + sol[c] < 0.0) {
                key_s[ch.key] = sol[c];
                key_contact_stamp[ch.key] = step_counter;
            }
        }
        last_active_channels = static_cast<std::uint32_t>(channel_count);
        summary.max_active_channels = std::max(summary.max_active_channels, last_active_channels);
        summary.max_newton_iterations = std::max(summary.max_newton_iterations, result.iterations);
        bool finite = result.finite;
        if (!result.converged && result.finite) {
            ++summary.newton_failures;
            note_fault(summary, FaultCode::NewtonNotConverged);
        }

        // Observation (before commit: previous = n-1, current = n, next = n+1).
        const nm::ModalBank& body = banks[body_bank];
        double left = 0.0;
        double right = 0.0;
        {
            const auto prev = body.previous();
            const auto cur = body.current();
            const auto next = body.next();
            for (std::size_t j = 0; j < body.size(); ++j) {
                const double accel = next[j] - 2.0 * cur[j] + prev[j];
                left += radiation[0][j] * accel;
                right += radiation[1][j] * accel;
            }
        }
        // The float output is what the host receives: an observation that is not finite after
        // the cast (beyond float range) is a runaway state and is contained, never clipped.
        finite = finite && std::isfinite(static_cast<float>(left)) && std::isfinite(static_cast<float>(right));
        if (!finite) {
            contain(summary);
            out_left = 0.0;
            out_right = 0.0;
            return;
        }

        // Ledger: potentials and work/dissipation of this step (geometry n).
        double v_before = 0.0;
        double v_after = 0.0;
        double work = 0.0;
        double dissipation = 0.0;
        if (ledger_on) {
            v_before = book_channel_changes();
            for (const auto& bank : banks) {
                dissipation += bank.step_dissipation_j();
            }
            for (std::size_t c = 0; c < channel_count; ++c) {
                Channel& ch = channels[c];
                const double z_next = gather(ch, 2) - ch.u_next;
                const double va = 0.5 * (nm::channel_potential_j(ch.law, z_next) +
                                         nm::channel_potential_j(ch.law, ch.z_cur));
                if (ch.key != no_key) {
                    key_value[ch.key] = va;
                }
                v_after += va;
                work += 0.5 * force[c] * (ch.u_next - ch.u_prev);
                dissipation += ch.law.damping_n_s_per_m * sol[c] * sol[c] / (4.0 * dt);
            }
        }

        // Actuator state machines (uses the final forces of this step).
        for (std::size_t c = 0; c < channel_count; ++c) {
            const Channel& ch = channels[c];
            if (ch.kind == ChannelKind::Nail) {
                NailState& nail = nails[ch.string_index];
                nail.force = force[c];
                const double z_next = ch.z_prev + sol[c];
                if (nail.phase == NailPhase::Approaching && z_next < 0.0) {
                    nail.phase = NailPhase::InContact;
                }
                if ((nail.phase == NailPhase::InContact && force[c] >= nail.force_limit) ||
                    nail.u_next - nail.u_start >= nail.travel_limit) {
                    nail.phase = NailPhase::Released;
                    nail.force = 0.0;
                }
            } else if (ch.kind == ChannelKind::FingerPad) {
                FingerState& f = fingers[ch.string_index][ch.slot];
                f.force = force[c];
                if (f.releasing) {
                    const double z_next = ch.z_prev + sol[c];
                    const double string_next = -(gather(ch, 2)); // vertical displacement at pad
                    if (z_next >= 0.0 && force[c] <= 0.0 && f.u_next >= string_next + f.release_gap) {
                        f.active = false;
                        f.releasing = false;
                        f.force = 0.0;
                    }
                }
            } else if (ch.kind == ChannelKind::Bridge) {
                // Force on the body is the reaction of the string-side generalized force.
                bridge_force[ch.string_index][ch.key % 2] = -force[c];
            }
        }
        for (auto& nail : nails) {
            nail.restarted = false;
        }

        for (auto& bank : banks) {
            bank.commit();
        }
        for (std::size_t si = 0; si < string_count && fret_count > 0; ++si) {
            const auto cur = banks[2 * si].current();
            const auto prev = banks[2 * si].previous();
            double moved = 0.0;
            for (std::size_t i = 0; i < cur.size(); ++i) {
                moved += std::abs(cur[i] - prev[i]);
            }
            for (double& slack : strings[si].fret_slack) {
                slack -= moved;
            }
        }
        for (auto& nail : nails) {
            if (nail.active() || nail.phase == NailPhase::Released) {
                nail.u_prev = nail.u_cur;
                nail.u_cur = nail.u_next;
            }
        }
        for (std::size_t si = 0; si < string_count; ++si) {
            for (std::size_t slot = 0; slot < 2; ++slot) {
                FingerState& f = fingers[si][slot];
                if (!f.active) {
                    continue;
                }
                f.u_prev = f.u_cur;
                f.u_cur = f.u_next;
                if (f.x_m != f.x_target) {
                    f.x_m = move_toward(f.x_m, f.x_target, f.slide_speed * dt);
                    numerics::fill_sine_shape(strings[si].finger_shape[slot],
                                              pi * f.x_m / strings[si].total_length_m);
                }
            }
        }

        if (ledger_on) {
            double h_after = 0.0;
            std::size_t b = 0;
            for (const auto& bank : banks) {
                const double e = bank.energy_j();
                h_after += e;
                if (b < 2 * string_count) {
                    if (b % 2 == 0) {
                        ledger.string_energy_j[b / 2] = e;
                    } else {
                        ledger.string_energy_j[b / 2] += e;
                    }
                } else {
                    ledger.body_energy_j = e;
                }
                ++b;
            }
            if (baseline_pending) {
                ledger.initial_stored_j = h_before + v_before;
                baseline_pending = false;
            }
            ledger.interaction_potential_j = v_after;
            ledger.external_work_j += work;
            ledger.dissipation_j += dissipation;
            ledger.residual_j = (h_after + v_after - ledger.initial_stored_j) - ledger.external_work_j +
                                ledger.dissipation_j - ledger.switching_j;
        }
        ++step_counter;
        out_left = left;
        out_right = right;
    }

    void contain(ProcessSummary& summary) noexcept {
        const std::uint64_t clock = sample_clock;
        zero_state();
        sample_clock = clock;
        ++summary.containment_resets;
        note_fault(summary, FaultCode::NonFiniteState);
    }

    bool valid_x(std::size_t s, double x) const noexcept {
        return std::isfinite(x) && x > 0.0 && x < config.strings[s].bridge_position_m;
    }

    double displacement_at(std::size_t bank_index, std::span<const double> shp) const noexcept {
        return banks[bank_index].gather_current(shp);
    }

    // Applies one validated event; returns false if rejected.
    bool apply(const GestureEvent& e) noexcept {
        if (e.kind == GestureKind::Silence) {
            const std::uint64_t clock = sample_clock;
            zero_state();
            sample_clock = clock;
            return true;
        }
        if (e.string_index >= string_count) {
            return false;
        }
        const std::size_t si = e.string_index;
        StringData& sd = strings[si];
        const double theta_scale = pi / sd.total_length_m;
        switch (e.kind) {
        case GestureKind::NailStroke: {
            if (!valid_x(si, e.position_m) || !std::isfinite(e.angle_rad) || !std::isfinite(e.speed_m_per_s) ||
                e.speed_m_per_s <= 0.0 || !std::isfinite(e.force_n) || e.force_n <= 0.0 ||
                !std::isfinite(e.gap_m) || e.gap_m < 0.0 || !std::isfinite(e.travel_m) || e.travel_m <= e.gap_m) {
                return false;
            }
            NailState& nail = nails[si];
            const bool was_active = nail.active();
            nail = NailState{};
            nail.restarted = was_active;
            nail.x_m = e.position_m;
            nail.dv = std::sin(e.angle_rad);
            nail.dh = std::cos(e.angle_rad);
            nail.speed = e.speed_m_per_s;
            nail.force_limit = e.force_n;
            nail.travel_limit = e.travel_m;
            numerics::fill_sine_shape(sd.nail_shape, theta_scale * e.position_m);
            const double p = nail.dv * displacement_at(2 * si, sd.nail_shape) +
                             nail.dh * displacement_at(2 * si + 1, sd.nail_shape);
            nail.u_cur = p - e.gap_m;
            nail.u_prev = nail.u_cur - nail.speed * dt;
            nail.u_start = nail.u_cur;
            nail.phase = NailPhase::Approaching;
            return true;
        }
        case GestureKind::FingerPress: {
            if (e.slot >= 2 || !valid_x(si, e.position_m) || !std::isfinite(e.height_m) ||
                !std::isfinite(e.speed_m_per_s) || e.speed_m_per_s <= 0.0 || !std::isfinite(e.gap_m) ||
                e.gap_m < 0.0) {
                return false;
            }
            FingerState& f = fingers[si][e.slot];
            if (!f.active) {
                f.x_m = e.position_m;
                f.x_target = e.position_m;
                f.slide_speed = 0.0;
                numerics::fill_sine_shape(sd.finger_shape[e.slot], theta_scale * e.position_m);
                f.u_cur = displacement_at(2 * si, sd.finger_shape[e.slot]) + e.gap_m;
                f.u_prev = f.u_cur;
                f.u_next = f.u_cur;
                f.force = 0.0;
            } else {
                // Retarget an active pad physically: it keeps its position and vertical state
                // and slides to the new position at the event speed (never a position jump).
                f.x_target = e.position_m;
                f.slide_speed = e.speed_m_per_s;
            }
            f.active = true;
            f.releasing = false;
            f.height_target = e.height_m;
            f.pad_speed = e.speed_m_per_s;
            f.release_gap = e.gap_m;
            return true;
        }
        case GestureKind::FingerMove: {
            if (e.slot >= 2 || !fingers[si][e.slot].active || !valid_x(si, e.position_m) ||
                !std::isfinite(e.speed_m_per_s) || e.speed_m_per_s <= 0.0) {
                return false;
            }
            FingerState& f = fingers[si][e.slot];
            f.x_target = e.position_m;
            f.slide_speed = e.speed_m_per_s;
            return true;
        }
        case GestureKind::FingerRelease: {
            if (e.slot >= 2 || !std::isfinite(e.speed_m_per_s) || e.speed_m_per_s <= 0.0 ||
                !std::isfinite(e.gap_m) || e.gap_m < 0.0) {
                return false;
            }
            FingerState& f = fingers[si][e.slot];
            if (f.active) { // releasing an inactive pad is an idempotent no-op
                f.releasing = true;
                f.pad_speed = e.speed_m_per_s;
                f.release_gap = e.gap_m;
            }
            return true;
        }
        case GestureKind::Silence:
            break;
        }
        return false;
    }
};

Instrument::Instrument() = default;
Instrument::~Instrument() = default;
Instrument::Instrument(Instrument&&) noexcept = default;
Instrument& Instrument::operator=(Instrument&&) noexcept = default;

void Instrument::prepare(const InstrumentConfig& config, const ProcessSpec& spec) {
    validate(config);
    validate(spec);
    auto next = std::make_unique<Impl>();
    Impl& im = *next;
    im.config = config;
    im.spec = spec;
    im.dt = 1.0 / spec.sample_rate_hz;
    const double cap_hz = std::min(config.max_mode_frequency_hz, 0.45 * spec.sample_rate_hz);
    im.fret_count = config.frets.size();
    im.info.sample_rate_hz = spec.sample_rate_hz;
    std::size_t dof = 0;
    for (std::size_t si = 0; si < string_count; ++si) {
        const StringConfig& st = config.strings[si];
        std::vector<double> mass, omega, decay;
        const double modal_mass = st.linear_density_kg_per_m * st.total_length_m / 2.0;
        for (std::size_t n = 1;; ++n) {
            const double k = static_cast<double>(n) * pi / st.total_length_m;
            const double w2 = st.tension_n / st.linear_density_kg_per_m * k * k +
                              st.bending_stiffness_n_m2 / st.linear_density_kg_per_m * k * k * k * k;
            const double w = std::sqrt(w2);
            if (w / (2.0 * pi) >= cap_hz) {
                break;
            }
            const double sigma = st.loss.decay_constant_per_s + st.loss.decay_wavenumber_squared_m2_per_s * k * k +
                                 st.loss.decay_frequency_squared_s * w2;
            if (sigma >= w) {
                break; // truncate at the first overdamped mode
            }
            require(n <= max_modes_per_string,
                    "strings[" + std::to_string(si) + "] retains more than 4096 modes; lower max_mode_frequency_hz");
            mass.push_back(modal_mass);
            omega.push_back(w);
            decay.push_back(sigma);
        }
        require(!mass.empty(), "strings[" + std::to_string(si) +
                                   "] retains no underdamped mode below the mode-frequency cap");
        im.banks[2 * si].prepare(mass, omega, decay, im.dt);
        im.banks[2 * si + 1].prepare(mass, omega, decay, im.dt);
        StringData& sd = im.strings[si];
        sd.modes = mass.size();
        sd.total_length_m = st.total_length_m;
        sd.bridge_shape.assign(sd.modes, 0.0);
        numerics::fill_sine_shape(sd.bridge_shape, pi * st.bridge_position_m / st.total_length_m);
        sd.fret_shapes.assign(im.fret_count * sd.modes, 0.0);
        sd.fret_height.assign(im.fret_count, 0.0);
        for (std::size_t f = 0; f < im.fret_count; ++f) {
            const double x = config.frets[f].position_m;
            numerics::fill_sine_shape(std::span<double>(sd.fret_shapes.data() + f * sd.modes, sd.modes),
                                      pi * x / st.total_length_m);
            sd.fret_height[f] = fret_top_height_m(st, x);
            sd.min_fret_gap = std::min(sd.min_fret_gap, -sd.fret_height[f]);
        }
        const std::size_t ns = im.fret_count + 1;
        sd.static_compliance.assign(ns * ns, 0.0);
        auto static_shape = [&](std::size_t id) {
            return id == 0 ? std::span<const double>(sd.bridge_shape)
                           : std::span<const double>(sd.fret_shapes.data() + (id - 1) * sd.modes, sd.modes);
        };
        for (std::size_t a = 0; a < ns; ++a) {
            for (std::size_t b = a; b < ns; ++b) {
                const double v = im.banks[2 * si].compliance(static_shape(a), static_shape(b));
                sd.static_compliance[a * ns + b] = v;
                sd.static_compliance[b * ns + a] = v;
            }
        }
        sd.nail_shape.assign(sd.modes, 0.0);
        sd.finger_shape[0].assign(sd.modes, 0.0);
        sd.finger_shape[1].assign(sd.modes, 0.0);
        sd.stick_anchor.assign(im.fret_count, 0.0);
        sd.fret_stamp.assign(im.fret_count, 0);
        sd.stick_stamp.assign(im.fret_count, 0);
        sd.fret_slack.assign(im.fret_count, -std::numeric_limits<double>::infinity());
        im.info.string_modes[si] = sd.modes;
        dof += 2 * sd.modes;
    }
    {
        std::vector<double> mass, omega, decay;
        std::vector<std::size_t> kept;
        for (std::size_t j = 0; j < config.body.modes.size(); ++j) {
            const BodyMode& mode = config.body.modes[j];
            if (mode.frequency_hz >= cap_hz) {
                continue;
            }
            const double w = 2.0 * pi * mode.frequency_hz;
            mass.push_back(mode.modal_mass_kg);
            omega.push_back(w);
            decay.push_back(mode.damping_ratio * w);
            kept.push_back(j);
        }
        im.banks[body_bank].prepare(mass, omega, decay, im.dt);
        for (std::size_t c = 0; c < bridge_channels; ++c) {
            im.body_shapes[c].assign(kept.size(), 0.0);
            for (std::size_t i = 0; i < kept.size(); ++i) {
                im.body_shapes[c][i] = config.body.modes[kept[i]].bridge_participation[c / 2][c % 2];
            }
        }
        for (std::size_t a = 0; a < bridge_channels; ++a) {
            for (std::size_t b = 0; b < bridge_channels; ++b) {
                im.body_compliance[a * bridge_channels + b] =
                    im.banks[body_bank].compliance(im.body_shapes[a], im.body_shapes[b]);
            }
        }
        for (std::size_t a = 0; a < bridge_channels; ++a) {
            for (std::size_t b = 0; b < bridge_channels; ++b) {
                // Same string and polarization share the string bank (static id 0).
                const double string_part = a == b ? im.strings[a / 2].static_compliance[0] : 0.0;
                const double body_part =
                    im.banks[body_bank].size() > 0 ? im.body_compliance[a * bridge_channels + b] : 0.0;
                im.bridge_block[a * bridge_channels + b] = string_part + body_part;
            }
        }
        for (std::size_t o = 0; o < observation_count; ++o) {
            im.radiation[o].assign(kept.size(), 0.0);
            for (std::size_t i = 0; i < kept.size(); ++i) {
                im.radiation[o][i] = config.body.modes[kept[i]].radiation_weight[o] * spec.output_gain_per_pa /
                                     (im.dt * im.dt);
            }
        }
        im.info.body_modes = kept.size();
        dof += kept.size();
    }
    im.info.total_degrees_of_freedom = dof;
    const std::size_t cap = spec.max_interaction_channels;
    im.channels.assign(cap, Channel{});
    im.compliance.assign(cap * cap, 0.0);
    im.rhs.assign(cap, 0.0);
    im.zprev_buf.assign(cap, 0.0);
    im.sol.assign(cap, 0.0);
    im.force.assign(cap, 0.0);
    im.laws.assign(cap, nm::ChannelLaw{});
    im.linear_index.assign(cap, 0);
    im.nonlinear_index.assign(cap, 0);
    im.lin_pivot.assign(cap, 0);
    for (auto* v : {&im.alpha, &im.beta, &im.work_vector, &im.force_l0, &im.reduced_rhs, &im.reduced_z_prev,
                    &im.reduced_s, &im.reduced_force}) {
        v->assign(cap, 0.0);
    }
    im.lin_matrix.assign(cap * cap, 0.0);
    im.coupling.assign(cap * cap, 0.0);
    im.reduced_compliance.assign(cap * cap, 0.0);
    im.reduced_law.assign(cap, nm::ChannelLaw{});
    im.solver.prepare(cap);
    const std::size_t keys = bridge_channels + 3 * string_count + 2 * string_count * im.fret_count;
    im.key_value.assign(keys, 0.0);
    im.key_stamp.assign(keys, 0);
    im.key_dissipative.assign(keys, 0);
    im.key_s.assign(keys, 0.0);
    im.key_contact_stamp.assign(keys, 0);
    for (std::size_t si = 0; si < string_count; ++si) {
        im.key_dissipative[im.nail_key(si)] = 1;
        im.key_dissipative[im.finger_key(si, 0)] = 1;
        im.key_dissipative[im.finger_key(si, 1)] = 1;
        for (std::size_t f = 0; f < im.fret_count; ++f) {
            im.key_dissipative[im.stick_key(si, f)] = 1;
        }
    }
    im.last_keys.assign(cap, 0);
    im.zero_state();
    im.sample_clock = 0;
    impl_ = std::move(next);
}

bool Instrument::prepared() const noexcept { return impl_ != nullptr; }

const PreparedInfo& Instrument::info() const noexcept {
    static const PreparedInfo empty{};
    return impl_ ? impl_->info : empty;
}

const InstrumentConfig& Instrument::config() const noexcept {
    static const InstrumentConfig empty{};
    return impl_ ? impl_->config : empty;
}

const ProcessSpec& Instrument::spec() const noexcept {
    static const ProcessSpec empty{};
    return impl_ ? impl_->spec : empty;
}

void Instrument::reset() noexcept {
    if (impl_) {
        impl_->zero_state();
        impl_->sample_clock = 0;
    }
}

ProcessSummary Instrument::process(std::span<const GestureEvent> events, std::span<float> left,
                                   std::span<float> right) noexcept {
    ProcessSummary summary;
    if (!impl_) {
        std::fill(left.begin(), left.end(), 0.0f);
        std::fill(right.begin(), right.end(), 0.0f);
        summary.first_fault = FaultCode::NotPrepared;
        summary.rejected_events = static_cast<std::uint32_t>(events.size());
        return summary;
    }
    Impl& im = *impl_;
    if (left.size() != right.size() || left.size() > im.spec.max_block_frames) {
        std::fill(left.begin(), left.end(), 0.0f);
        std::fill(right.begin(), right.end(), 0.0f);
        summary.first_fault = FaultCode::InvalidBlock;
        summary.rejected_events = static_cast<std::uint32_t>(events.size());
        return summary;
    }
    std::size_t event_count = events.size();
    if (event_count > im.spec.max_events_per_block) {
        summary.rejected_events += static_cast<std::uint32_t>(event_count - im.spec.max_events_per_block);
        note_fault(summary, FaultCode::EventOverflow);
        event_count = im.spec.max_events_per_block;
    }
    const std::size_t frames = left.size();
    std::size_t e = 0;
    bool have_last = false;
    std::uint32_t last_offset = 0;
    std::uint32_t last_sequence = 0;
    for (std::size_t i = 0; i < frames; ++i) {
        while (e < event_count && events[e].sample_offset <= i) {
            const GestureEvent& ev = events[e++];
            const bool ordered = ev.sample_offset == i &&
                                 (!have_last || ev.sample_offset > last_offset ||
                                  (ev.sample_offset == last_offset && ev.sequence > last_sequence));
            if (!ordered || !im.apply(ev)) {
                ++summary.rejected_events;
                note_fault(summary, FaultCode::InvalidEvent);
                continue;
            }
            have_last = true;
            last_offset = ev.sample_offset;
            last_sequence = ev.sequence;
        }
        double l = 0.0;
        double r = 0.0;
        im.step(summary, l, r);
        left[i] = static_cast<float>(l);
        right[i] = static_cast<float>(r);
        ++im.sample_clock;
    }
    if (e < event_count) {
        summary.rejected_events += static_cast<std::uint32_t>(event_count - e); // offsets >= frames
        note_fault(summary, FaultCode::InvalidEvent);
    }
    summary.processed_frames = static_cast<std::uint32_t>(frames);
    return summary;
}

InstrumentSnapshot Instrument::snapshot() const noexcept {
    InstrumentSnapshot out;
    if (!impl_) {
        return out;
    }
    const Impl& im = *impl_;
    out.sample_clock = im.sample_clock;
    out.active_channels = im.last_active_channels;
    out.body_energy_j = im.banks[body_bank].energy_j();
    for (std::size_t si = 0; si < string_count; ++si) {
        StringSnapshot& ss = out.strings[si];
        ss.nail_phase = im.nails[si].phase;
        ss.nail_force_n = im.nails[si].active() ? im.nails[si].force : 0.0;
        for (std::size_t slot = 0; slot < 2; ++slot) {
            ss.finger_active[slot] = im.fingers[si][slot].active;
            ss.finger_position_m[slot] = im.fingers[si][slot].x_m;
            ss.finger_force_n[slot] = im.fingers[si][slot].active ? im.fingers[si][slot].force : 0.0;
        }
        ss.bridge_force_n = im.bridge_force[si];
        ss.energy_j = im.banks[2 * si].energy_j() + im.banks[2 * si + 1].energy_j();
    }
    return out;
}

EnergyLedger Instrument::ledger() const noexcept { return impl_ ? impl_->ledger : EnergyLedger{}; }

namespace {
std::vector<double> probe_shape(std::size_t modes, double x_m, double length_m) {
    std::vector<double> shape(modes);
    numerics::fill_sine_shape(shape, pi * x_m / length_m);
    return shape;
}
} // namespace

double Instrument::string_displacement_m(std::size_t string_index, Polarization p, double x_m) const {
    require(impl_ != nullptr, "instrument is not prepared");
    require(string_index < string_count, "string index out of range");
    const auto& sd = impl_->strings[string_index];
    require(std::isfinite(x_m) && x_m >= 0.0 && x_m <= sd.total_length_m, "probe position out of range");
    const auto shp = probe_shape(sd.modes, x_m, sd.total_length_m);
    return impl_->banks[2 * string_index + static_cast<std::size_t>(p)].gather_current(shp);
}

double Instrument::string_velocity_m_per_s(std::size_t string_index, Polarization p, double x_m) const {
    require(impl_ != nullptr, "instrument is not prepared");
    require(string_index < string_count, "string index out of range");
    const auto& sd = impl_->strings[string_index];
    require(std::isfinite(x_m) && x_m >= 0.0 && x_m <= sd.total_length_m, "probe position out of range");
    const auto shp = probe_shape(sd.modes, x_m, sd.total_length_m);
    const auto& bank = impl_->banks[2 * string_index + static_cast<std::size_t>(p)];
    return (bank.gather_current(shp) - bank.gather_previous(shp)) / impl_->dt;
}

void Instrument::set_string_modal_displacement(std::size_t string_index, Polarization p,
                                               std::span<const double> modal_displacement_m) {
    require(impl_ != nullptr, "instrument is not prepared");
    require(string_index < string_count, "string index out of range");
    auto& bank = impl_->banks[2 * string_index + static_cast<std::size_t>(p)];
    require(modal_displacement_m.size() == bank.size(), "modal displacement size must equal the mode count");
    for (double q : modal_displacement_m) {
        require(std::isfinite(q), "modal displacement must be finite");
    }
    bank.set_displacement(modal_displacement_m);
    auto& slack = impl_->strings[string_index].fret_slack;
    std::fill(slack.begin(), slack.end(), -std::numeric_limits<double>::infinity());
    impl_->ledger = EnergyLedger{};
    impl_->baseline_pending = true;
}

} // namespace setar::instrument
