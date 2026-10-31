#include "setar/instrument/performance.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <locale>
#include <numbers>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace setar::instrument {
namespace {

constexpr double pi = std::numbers::pi_v<double>;

void require(bool valid, const std::string& message) {
    if (!valid) {
        throw std::invalid_argument(message);
    }
}

bool positive(double v) { return std::isfinite(v) && v > 0.0; }
bool nonnegative(double v) { return std::isfinite(v) && v >= 0.0; }

// Local structural checks only; the engine's validate(InstrumentConfig) is authoritative.
void check_config(const InstrumentConfig& config) {
    for (std::size_t s = 0; s < string_count; ++s) {
        const StringConfig& sc = config.strings[s];
        require(positive(sc.bridge_position_m),
                "config: string " + std::to_string(s) + " bridge_position_m must be positive");
        require(std::isfinite(sc.action_at_nut_m) && std::isfinite(sc.action_at_bridge_m),
                "config: string " + std::to_string(s) + " action must be finite");
    }
    double previous = 0.0;
    for (std::size_t k = 0; k < config.frets.size(); ++k) {
        const double x = config.frets[k].position_m;
        require(std::isfinite(x) && x > previous,
                "config: fret positions must be finite, positive and strictly increasing");
        for (const StringConfig& sc : config.strings) {
            require(x < sc.bridge_position_m, "config: every fret must lie before the bridge");
        }
        previous = x;
    }
}

void check_dynamics(const StrokeDynamics& d, const char* name) {
    require(positive(d.speed_m_per_s),
            std::string("settings: ") + name + ".speed_m_per_s must be positive");
    require(positive(d.release_force_n),
            std::string("settings: ") + name + ".release_force_n must be positive");
}

double cents_of(double ratio) { return 1200.0 * std::log2(ratio); }

enum class Direction { Down, Up };

// d = (sin a, cos a) in (vertical, horizontal). Down: d_h < 0; up: d_h > 0; both lean
// toward the soundboard (d_v < 0) by stroke_angle_rad. Placeholder convention.
double stroke_angle(Direction dir, double elevation) {
    return dir == Direction::Down ? -pi + elevation : -elevation;
}

const std::array<std::array<std::size_t, 2>, 3> course_strings{{{0, 0}, {1, 1}, {2, 3}}};
const std::array<std::size_t, 3> course_sizes{1, 1, 2};

double parse_number(std::string_view text) {
    // from_chars for double is missing on some supported Apple toolchains; keep this
    // strict and locale independent (same policy as apps/render_reference.cpp).
    double value = 0.0;
    std::istringstream input{std::string(text)};
    input.imbue(std::locale::classic());
    input >> std::noskipws >> value;
    if (text.empty() || !input || !input.eof() || !std::isfinite(value)) {
        throw std::invalid_argument("invalid number '" + std::string(text) + "'");
    }
    return value;
}

std::size_t parse_index(std::string_view text) {
    const double v = parse_number(text);
    if (v < 0.0 || v != std::floor(v) || v > 1.0e6) {
        throw std::invalid_argument("invalid nonnegative integer '" + std::string(text) + "'");
    }
    return static_cast<std::size_t>(v);
}

std::string_view trim(std::string_view s) {
    const auto first = s.find_first_not_of(" \t\r");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = s.find_last_not_of(" \t\r");
    return s.substr(first, last - first + 1);
}

std::vector<std::string_view> split_words(std::string_view s) {
    std::vector<std::string_view> out;
    std::size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) {
            ++i;
        }
        std::size_t j = i;
        while (j < s.size() && s[j] != ' ' && s[j] != '\t') {
            ++j;
        }
        if (j > i) {
            out.push_back(s.substr(i, j - i));
        }
        i = j;
    }
    return out;
}

struct FingerState {
    bool active = false;
    double position_m = 0.0;
};

class Parser {
  public:
    Parser(const InstrumentConfig& config, const PerformanceSettings& settings)
        : config_(config), settings_(settings) {}

    Score run(std::string_view text) {
        std::size_t line_number = 0;
        std::size_t start = 0;
        while (start <= text.size()) {
            const std::size_t end = std::min(text.find('\n', start), text.size());
            ++line_number;
            line_ = line_number;
            std::string_view line = text.substr(start, end - start);
            line = trim(line.substr(0, std::min(line.find('#'), line.size())));
            if (!line.empty()) {
                try {
                    handle(line);
                } catch (const std::invalid_argument& e) {
                    const std::string what = e.what();
                    if (what.rfind("line ", 0) == 0) {
                        throw;
                    }
                    fail(what);
                }
            }
            if (end == text.size()) {
                break;
            }
            start = end + 1;
        }
        require(!score_.status.empty(), "score: missing required 'status' header");
        require(ended_, "score: missing required 'end <time>' command");
        std::stable_sort(
            score_.gestures.begin(), score_.gestures.end(),
            [](const TimedGesture& a, const TimedGesture& b) { return a.time_s < b.time_s; });
        return std::move(score_);
    }

  private:
    [[noreturn]] void fail(const std::string& message) const {
        throw std::invalid_argument("line " + std::to_string(line_) + ": " + message);
    }

    // Key/value arguments; every key must be consumed exactly once.
    struct Args {
        std::vector<std::pair<std::string_view, std::string_view>> items;
        std::vector<bool> used;
    };

    Args parse_args(const std::vector<std::string_view>& words, std::size_t first) const {
        Args args;
        for (std::size_t i = first; i < words.size(); ++i) {
            const auto eq = words[i].find('=');
            if (eq == std::string_view::npos || eq == 0 || eq + 1 == words[i].size()) {
                fail("expected key=value, got '" + std::string(words[i]) + "'");
            }
            const std::string_view key = words[i].substr(0, eq);
            for (const auto& item : args.items) {
                if (item.first == key) {
                    fail("duplicate key '" + std::string(key) + "'");
                }
            }
            args.items.emplace_back(key, words[i].substr(eq + 1));
        }
        args.used.assign(args.items.size(), false);
        return args;
    }

    static const std::string_view* find(Args& args, std::string_view key) {
        for (std::size_t i = 0; i < args.items.size(); ++i) {
            if (args.items[i].first == key) {
                args.used[i] = true;
                return &args.items[i].second;
            }
        }
        return nullptr;
    }

    std::string_view need(Args& args, std::string_view key) const {
        const std::string_view* v = find(args, key);
        if (v == nullptr) {
            fail("missing required key '" + std::string(key) + "'");
        }
        return *v;
    }

    void finish(const Args& args) const {
        for (std::size_t i = 0; i < args.items.size(); ++i) {
            if (!args.used[i]) {
                fail("unknown key '" + std::string(args.items[i].first) + "' for this command");
            }
        }
    }

    // "@<seconds>" or "<beats>" (requires a tempo header).
    double parse_time(std::string_view token) const {
        if (!token.empty() && token.front() == '@') {
            return parse_number(token.substr(1));
        }
        if (tempo_bpm_ <= 0.0) {
            fail("time '" + std::string(token) +
                 "' is in beats but no 'tempo' header was given (use @seconds)");
        }
        return parse_number(token) * 60.0 / tempo_bpm_;
    }

    std::size_t string_arg(Args& args) const {
        const std::size_t s = parse_index(need(args, "string"));
        if (s >= string_count) {
            fail("string must be 0..3, got " + std::to_string(s));
        }
        return s;
    }

    std::uint8_t slot_arg(Args& args) const {
        const std::string_view* v = find(args, "slot");
        if (v == nullptr) {
            return 0;
        }
        const std::size_t slot = parse_index(*v);
        if (slot > 1) {
            fail("slot must be 0 or 1");
        }
        return static_cast<std::uint8_t>(slot);
    }

    std::size_t fret_value(std::string_view text) const {
        const std::size_t k = parse_index(text);
        if (k > config_.frets.size()) {
            fail("fret " + std::to_string(k) + " exceeds the fret map (" +
                 std::to_string(config_.frets.size()) + " frets)");
        }
        return k;
    }

    StrokeDynamics dyn_arg(Args& args) const {
        const std::string_view* v = find(args, "dyn");
        if (v == nullptr || *v == "mf") {
            return settings_.mf;
        }
        if (*v == "pp")
            return settings_.pp;
        if (*v == "p")
            return settings_.p;
        if (*v == "mp")
            return settings_.mp;
        if (*v == "f")
            return settings_.f;
        if (*v == "ff")
            return settings_.ff;
        fail("dyn must be one of pp, p, mp, mf, f, ff");
    }

    Direction dir_arg(Args& args) const {
        const std::string_view* v = find(args, "dir");
        if (v == nullptr || *v == "down") {
            return Direction::Down;
        }
        if (*v == "up") {
            return Direction::Up;
        }
        fail("dir must be down or up");
    }

    double duration_arg(std::string_view token) const {
        const double d = parse_time(token);
        if (!(d > 0.0)) {
            fail("duration must be positive");
        }
        return d;
    }

    double finger_position(std::size_t s, std::size_t fret) const {
        const double x = finger_position_for_fret(config_, fret, settings_);
        if (!(x > 0.0) || x >= config_.strings[s].bridge_position_m) {
            fail("finger position for fret " + std::to_string(fret) + " lies outside the string");
        }
        return x;
    }

    void add(double time, const GestureEvent& event) {
        if (time < 0.0) {
            fail("an expanded gesture falls before t = 0 (finger lead time " +
                 std::to_string(settings_.finger_lead_time_s) + " s); start later");
        }
        score_.gestures.push_back(TimedGesture{time, event});
        latest_gesture_s_ = std::max(latest_gesture_s_, time);
    }

    // A finger change for a stroke at `stroke_time` must not precede an earlier stroke
    // on the same string; otherwise that earlier note would sound at the new position.
    void check_finger_time(std::size_t s, double time) const {
        if (time < last_stroke_s_[s]) {
            fail("finger change at t = " + std::to_string(time) +
                 " s precedes the previous stroke on string " + std::to_string(s) + " (t = " +
                 std::to_string(last_stroke_s_[s]) + " s); space the notes further or use 'slide'");
        }
    }

    GestureEvent pad_press(std::size_t s, std::uint8_t slot, double x, double height) const {
        GestureEvent e;
        e.kind = GestureKind::FingerPress;
        e.string_index = static_cast<std::uint8_t>(s);
        e.slot = slot;
        e.position_m = x;
        e.height_m = height;
        e.speed_m_per_s = settings_.finger_speed_m_per_s;
        e.gap_m = settings_.finger_gap_m;
        return e;
    }

    GestureEvent pad_release(std::size_t s, std::uint8_t slot) const {
        GestureEvent e;
        e.kind = GestureKind::FingerRelease;
        e.string_index = static_cast<std::uint8_t>(s);
        e.slot = slot;
        e.position_m = fingers_[s][slot].position_m;
        e.speed_m_per_s = settings_.finger_speed_m_per_s;
        e.gap_m = settings_.finger_gap_m;
        return e;
    }

    GestureEvent nail(std::size_t s, Direction dir, const StrokeDynamics& d,
                      double from_bridge) const {
        GestureEvent e;
        e.kind = GestureKind::NailStroke;
        e.string_index = static_cast<std::uint8_t>(s);
        e.position_m = config_.strings[s].bridge_position_m - from_bridge;
        if (!(e.position_m > 0.0)) {
            fail("pluck point lies outside the string");
        }
        e.angle_rad = stroke_angle(dir, settings_.stroke_angle_rad);
        e.speed_m_per_s = d.speed_m_per_s;
        e.force_n = d.release_force_n;
        e.gap_m = settings_.nail_gap_m;
        e.travel_m = settings_.nail_travel_m;
        return e;
    }

    double from_bridge_arg(Args& args) const {
        const std::string_view* v = find(args, "from_bridge");
        if (v == nullptr) {
            return settings_.pluck_distance_from_bridge_m;
        }
        const double d = parse_number(*v);
        if (!(d > 0.0)) {
            fail("from_bridge must be positive (m)");
        }
        return d;
    }

    // Applies the fret request of a stroke command `finger_lead_time_s` before it.
    void apply_fret(std::size_t s, std::size_t fret, double stroke_time) {
        const double t = stroke_time - settings_.finger_lead_time_s;
        FingerState& finger = fingers_[s][0];
        if (fret == 0) {
            if (finger.active) {
                check_finger_time(s, t);
                add(t, pad_release(s, 0));
                finger.active = false;
            }
            return;
        }
        const double x = finger_position(s, fret);
        if (finger.active && finger.position_m == x) {
            return; // already stopped there
        }
        check_finger_time(s, t);
        add(t,
            pad_press(s, 0, x, fret_top_height_m(config_.strings[s], x) - settings_.press_depth_m));
        finger = FingerState{true, x};
    }

    void stroke(std::size_t s, double time, Direction dir, const StrokeDynamics& d,
                double from_bridge) {
        add(time, nail(s, dir, d, from_bridge));
        last_stroke_s_[s] = std::max(last_stroke_s_[s], time);
    }

    void handle(std::string_view line) {
        const std::vector<std::string_view> words = split_words(line);
        const std::string_view command = words.front();
        if (command == "title" || command == "status") {
            const std::string_view rest = trim(line.substr(command.size()));
            if (rest.empty()) {
                fail(std::string(command) + " needs text");
            }
            (command == "title" ? score_.title : score_.status) = std::string(rest);
            return;
        }
        if (command == "tempo") {
            if (words.size() != 2) {
                fail("tempo takes exactly one value (beats per minute)");
            }
            if (tempo_bpm_ > 0.0) {
                fail("tempo may be declared only once");
            }
            if (any_command_) {
                fail("tempo must precede every timed command");
            }
            tempo_bpm_ = parse_number(words[1]);
            if (!(tempo_bpm_ > 0.0) || tempo_bpm_ > 1000.0) {
                fail("tempo must be in (0, 1000] beats per minute");
            }
            return;
        }
        if (ended_) {
            fail("no command may follow 'end'");
        }
        if (words.size() < 2 || words[1].find('=') != std::string_view::npos) {
            fail("command '" + std::string(command) + "' needs a time before its keys");
        }
        const double t = parse_time(words[1]);
        if (t < 0.0) {
            fail("time must be nonnegative");
        }
        if (t < last_command_s_) {
            fail("command times must be nondecreasing in file order");
        }
        last_command_s_ = t;
        any_command_ = true;
        Args args = parse_args(words, 2);

        if (command == "pluck") {
            const std::size_t s = string_arg(args);
            const std::string_view* fret = find(args, "fret");
            const StrokeDynamics d = dyn_arg(args);
            const Direction dir = dir_arg(args);
            const double from_bridge = from_bridge_arg(args);
            finish(args);
            if (fret != nullptr) {
                apply_fret(s, fret_value(*fret), t);
            }
            stroke(s, t, dir, d, from_bridge);
        } else if (command == "strum") {
            const std::string_view* course = find(args, "course");
            const std::string_view* list = find(args, "strings");
            if ((course == nullptr) == (list == nullptr)) {
                fail("strum needs exactly one of course=<0..2> or strings=a,b,...");
            }
            std::vector<std::size_t> strings;
            if (course != nullptr) {
                const std::size_t c = parse_index(*course);
                if (c >= course_sizes.size()) {
                    fail("course must be 0, 1 or 2");
                }
                for (std::size_t i = 0; i < course_sizes[c]; ++i) {
                    strings.push_back(course_strings[c][i]);
                }
            } else {
                if (list->back() == ',' || list->front() == ',' ||
                    list->find(",,") != std::string_view::npos) {
                    fail("strings list has an empty entry");
                }
                std::string_view rest = *list;
                while (!rest.empty()) {
                    const auto comma = std::min(rest.find(','), rest.size());
                    const std::size_t s = parse_index(rest.substr(0, comma));
                    if (s >= string_count) {
                        fail("strings entries must be 0..3");
                    }
                    if (std::find(strings.begin(), strings.end(), s) != strings.end()) {
                        fail("strings entries must be distinct");
                    }
                    strings.push_back(s);
                    rest = comma == rest.size() ? std::string_view{} : rest.substr(comma + 1);
                }
            }
            const StrokeDynamics d = dyn_arg(args);
            const Direction dir = dir_arg(args);
            const double from_bridge = from_bridge_arg(args);
            finish(args);
            // A +h (up) stroke meets strings in ascending index order, a -h (down) stroke in
            // descending order, assuming lateral bridge order equals index order (placeholder).
            std::sort(strings.begin(), strings.end());
            if (dir == Direction::Down) {
                std::reverse(strings.begin(), strings.end());
            }
            for (std::size_t i = 0; i < strings.size(); ++i) {
                stroke(strings[i], t + static_cast<double>(i) * settings_.course_sweep_delay_s, dir,
                       d, from_bridge);
            }
        } else if (command == "tremolo") {
            const std::size_t s = string_arg(args);
            const std::string_view* fret = find(args, "fret");
            const double duration = duration_arg(need(args, "dur"));
            const double rate = parse_number(need(args, "rate"));
            if (!(rate > 0.0) || rate > 100.0) {
                fail("rate must be in (0, 100] strokes per second");
            }
            const StrokeDynamics d = dyn_arg(args);
            Direction dir = dir_arg(args);
            const double from_bridge = from_bridge_arg(args);
            finish(args);
            const auto count = static_cast<std::size_t>(std::floor(duration * rate + 1.0e-9));
            if (count == 0) {
                fail("tremolo duration shorter than one stroke period");
            }
            if (fret != nullptr) {
                apply_fret(s, fret_value(*fret), t);
            }
            for (std::size_t i = 0; i < count; ++i) {
                stroke(s, t + static_cast<double>(i) / rate, dir, d, from_bridge);
                dir = dir == Direction::Down ? Direction::Up : Direction::Down;
            }
        } else if (command == "press") {
            const std::size_t s = string_arg(args);
            const std::uint8_t slot = slot_arg(args);
            const std::size_t fret = fret_value(need(args, "fret"));
            finish(args);
            if (fret == 0) {
                fail("press needs fret >= 1 (use release for the open string)");
            }
            const double x = finger_position(s, fret);
            check_finger_time(s, t);
            add(t, pad_press(s, slot, x,
                             fret_top_height_m(config_.strings[s], x) - settings_.press_depth_m));
            fingers_[s][slot] = FingerState{true, x};
        } else if (command == "slide") {
            const std::size_t s = string_arg(args);
            const std::uint8_t slot = slot_arg(args);
            const std::size_t fret = fret_value(need(args, "to_fret"));
            const double duration = duration_arg(need(args, "dur"));
            finish(args);
            if (fret == 0) {
                fail("slide needs to_fret >= 1");
            }
            FingerState& finger = fingers_[s][slot];
            if (!finger.active) {
                fail("slide needs an active finger on string " + std::to_string(s) + " slot " +
                     std::to_string(slot));
            }
            const double x = finger_position(s, fret);
            if (x == finger.position_m) {
                fail("slide target equals the current finger position");
            }
            GestureEvent e;
            e.kind = GestureKind::FingerMove;
            e.string_index = static_cast<std::uint8_t>(s);
            e.slot = slot;
            e.position_m = x;
            e.speed_m_per_s = std::abs(x - finger.position_m) / duration;
            add(t, e);
            finger.position_m = x;
        } else if (command == "release") {
            const std::size_t s = string_arg(args);
            const std::uint8_t slot = slot_arg(args);
            finish(args);
            if (!fingers_[s][slot].active) {
                fail("release needs an active finger on string " + std::to_string(s) + " slot " +
                     std::to_string(slot));
            }
            add(t, pad_release(s, slot));
            fingers_[s][slot].active = false;
        } else if (command == "touch") {
            const std::size_t s = string_arg(args);
            const std::uint8_t slot = slot_arg(args);
            const double at = parse_number(need(args, "at"));
            finish(args);
            if (!(at > 0.0 && at < 1.0)) {
                fail("touch at= must be a fraction of the speaking length in (0, 1)");
            }
            const double x = at * config_.strings[s].bridge_position_m;
            add(t, pad_press(s, slot, x, -settings_.touch_depth_m));
            fingers_[s][slot] = FingerState{true, x};
        } else if (command == "damp") {
            const std::size_t s = string_arg(args);
            const std::string_view* dur = find(args, "dur");
            finish(args);
            const double hold = dur == nullptr ? settings_.damp_hold_s : duration_arg(*dur);
            const double x =
                settings_.damp_position_fraction * config_.strings[s].bridge_position_m;
            add(t, pad_press(s, 1, x, -settings_.touch_depth_m));
            fingers_[s][1] = FingerState{true, x};
            add(t + hold, pad_release(s, 1));
            fingers_[s][1].active = false;
        } else if (command == "silence") {
            finish(args);
            GestureEvent e;
            e.kind = GestureKind::Silence;
            add(t, e);
            for (auto& string_fingers : fingers_) {
                string_fingers[0].active = false;
                string_fingers[1].active = false;
            }
        } else if (command == "end") {
            finish(args);
            if (latest_gesture_s_ > t) {
                fail("a gesture at t = " + std::to_string(latest_gesture_s_) + " s lies after end");
            }
            score_.end_time_s = t;
            ended_ = true;
        } else {
            fail("unknown command '" + std::string(command) + "'");
        }
    }

    const InstrumentConfig& config_;
    const PerformanceSettings& settings_;
    Score score_{};
    std::size_t line_ = 0;
    double tempo_bpm_ = 0.0;
    double last_command_s_ = 0.0;
    double latest_gesture_s_ = 0.0;
    bool any_command_ = false;
    bool ended_ = false;
    std::array<std::array<FingerState, 2>, string_count> fingers_{};
    std::array<double, string_count> last_stroke_s_{
        {-std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(),
         -std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity()}};
};

} // namespace

double finger_position_for_fret(const InstrumentConfig& config, std::size_t fret,
                                const PerformanceSettings& settings) {
    require(fret >= 1 && fret <= config.frets.size(), "fret must be 1..number of frets");
    require(nonnegative(settings.finger_offset_behind_fret_m),
            "settings: finger_offset_behind_fret_m must be >= 0");
    const double x = config.frets[fret - 1].position_m;
    const double previous = fret == 1 ? 0.0 : config.frets[fret - 2].position_m;
    require(std::isfinite(x) && x > previous,
            "fret positions must be finite and strictly increasing");
    return x - std::min(settings.finger_offset_behind_fret_m, 0.4 * (x - previous));
}

PerformanceSettings default_performance_settings() {
    // SYNTHETIC placeholders chosen to lie in the order-of-magnitude ranges stated in
    // docs/performance/07-score-format.md. Not measured, not practitioner-reviewed.
    PerformanceSettings s;
    s.pluck_distance_from_bridge_m = 0.12;
    s.finger_offset_behind_fret_m = 0.008;
    s.press_depth_m = 0.0008;
    s.finger_speed_m_per_s = 0.1;
    s.finger_gap_m = 0.001;
    s.finger_lead_time_s = 0.06;
    s.nail_gap_m = 0.0008;
    s.nail_travel_m = 0.008;
    s.stroke_angle_rad = 0.15;
    s.course_sweep_delay_s = 0.008;
    s.pp = {0.12, 0.2};
    s.p = {0.18, 0.35};
    s.mp = {0.25, 0.55};
    s.mf = {0.35, 0.8};
    s.f = {0.5, 1.2};
    s.ff = {0.7, 1.8};
    s.touch_depth_m = 0.0001;
    s.damp_position_fraction = 0.37;
    s.damp_hold_s = 0.2;
    return s;
}

void validate(const PerformanceSettings& s) {
    require(positive(s.pluck_distance_from_bridge_m),
            "settings: pluck_distance_from_bridge_m must be positive");
    require(nonnegative(s.finger_offset_behind_fret_m),
            "settings: finger_offset_behind_fret_m must be >= 0");
    require(nonnegative(s.press_depth_m), "settings: press_depth_m must be >= 0");
    require(positive(s.finger_speed_m_per_s), "settings: finger_speed_m_per_s must be positive");
    require(nonnegative(s.finger_gap_m), "settings: finger_gap_m must be >= 0");
    require(nonnegative(s.finger_lead_time_s), "settings: finger_lead_time_s must be >= 0");
    require(nonnegative(s.nail_gap_m), "settings: nail_gap_m must be >= 0");
    require(std::isfinite(s.nail_travel_m) && s.nail_travel_m > s.nail_gap_m,
            "settings: nail_travel_m must exceed nail_gap_m");
    require(std::isfinite(s.stroke_angle_rad) && s.stroke_angle_rad >= 0.0 &&
                s.stroke_angle_rad < pi / 2.0,
            "settings: stroke_angle_rad must be in [0, pi/2)");
    require(nonnegative(s.course_sweep_delay_s), "settings: course_sweep_delay_s must be >= 0");
    check_dynamics(s.pp, "pp");
    check_dynamics(s.p, "p");
    check_dynamics(s.mp, "mp");
    check_dynamics(s.mf, "mf");
    check_dynamics(s.f, "f");
    check_dynamics(s.ff, "ff");
    require(nonnegative(s.touch_depth_m), "settings: touch_depth_m must be >= 0");
    require(std::isfinite(s.damp_position_fraction) && s.damp_position_fraction > 0.0 &&
                s.damp_position_fraction < 1.0,
            "settings: damp_position_fraction must be in (0, 1)");
    require(positive(s.damp_hold_s), "settings: damp_hold_s must be positive");
}

Score parse_score(std::string_view text, const InstrumentConfig& config,
                  const PerformanceSettings& settings) {
    check_config(config);
    validate(settings);
    return Parser(config, settings).run(text);
}

std::vector<ScheduledGesture> schedule(const Score& score, double sample_rate_hz) {
    require(std::isfinite(sample_rate_hz) && sample_rate_hz > 0.0,
            "sample rate must be finite and positive");
    std::vector<ScheduledGesture> out;
    out.reserve(score.gestures.size());
    double previous = -std::numeric_limits<double>::infinity();
    for (const TimedGesture& g : score.gestures) {
        require(std::isfinite(g.time_s) && g.time_s >= 0.0,
                "gesture times must be finite and nonnegative");
        require(g.time_s >= previous, "score gestures must be sorted by time");
        previous = g.time_s;
        const double index = std::round(g.time_s * sample_rate_hz);
        require(index < 9.0e15, "gesture time exceeds the schedulable range");
        ScheduledGesture sg;
        sg.sample_index = static_cast<std::uint64_t>(index);
        sg.event = g.event;
        sg.event.sample_offset = 0; // block-relative offset is assigned by the renderer
        require(out.size() < std::numeric_limits<std::uint32_t>::max(), "too many gestures");
        sg.event.sequence = static_cast<std::uint32_t>(out.size());
        out.push_back(sg);
    }
    return out;
}

FretChoice choose_fret(double frequency_hz, const InstrumentConfig& config,
                       std::span<const double> open_frequency_hz,
                       std::span<const std::size_t> allowed, double max_abs_cents) {
    require(positive(frequency_hz), "requested frequency must be finite and positive");
    require(open_frequency_hz.size() == string_count,
            "open_frequency_hz needs one entry per string");
    require(std::isfinite(max_abs_cents) && max_abs_cents >= 0.0,
            "max_abs_cents must be finite and >= 0");
    check_config(config);
    std::array<std::size_t, string_count> all{0, 1, 2, 3};
    const std::span<const std::size_t> candidates =
        allowed.empty() ? std::span<const std::size_t>(all) : allowed;
    std::vector<FretChoice> options;
    for (const std::size_t s : candidates) {
        require(s < string_count, "allowed string index must be 0..3");
        require(positive(open_frequency_hz[s]), "open frequencies must be finite and positive");
        const double length = config.strings[s].bridge_position_m;
        for (std::size_t fret = 0; fret <= config.frets.size(); ++fret) {
            const double x = fret == 0 ? 0.0 : config.frets[fret - 1].position_m;
            FretChoice option;
            option.string_index = s;
            option.fret_number = fret;
            option.finger_position_m = x;
            option.nominal_frequency_hz = open_frequency_hz[s] * length / (length - x);
            option.cents_error = cents_of(option.nominal_frequency_hz / frequency_hz);
            if (std::abs(option.cents_error) <= max_abs_cents) {
                options.push_back(option);
            }
        }
    }
    const bool found = !options.empty();
    FretChoice best;
    if (found) {
        double smallest = std::abs(options.front().cents_error);
        for (const FretChoice& o : options) {
            smallest = std::min(smallest, std::abs(o.cents_error));
        }
        // Candidates within tie_cents of the smallest error are musically equivalent:
        // prefer the lower fret, then the lower string index.
        constexpr double tie_cents = 1.0;
        bool chosen = false;
        for (const FretChoice& o : options) {
            if (std::abs(o.cents_error) > smallest + tie_cents) {
                continue;
            }
            if (!chosen || o.fret_number < best.fret_number ||
                (o.fret_number == best.fret_number && o.string_index < best.string_index)) {
                best = o;
                chosen = true;
            }
        }
    }
    require(found, "no string/fret within " + std::to_string(max_abs_cents) + " cents of " +
                       std::to_string(frequency_hz) + " Hz");
    return best;
}

void MidiGestureMapper::prepare(const InstrumentConfig& config,
                                std::span<const double> open_frequency_hz,
                                const PerformanceSettings& settings, double reference_a4_hz) {
    check_config(config);
    validate(settings);
    require(positive(reference_a4_hz), "reference_a4_hz must be finite and positive");
    require(config.frets.size() <= 255, "the MIDI mapper supports at most 255 frets");
    require(open_frequency_hz.size() == string_count,
            "open_frequency_hz needs one entry per string");
    std::vector<NoteRoute> routes(128);
    for (std::size_t note = 0; note < routes.size(); ++note) {
        const double f = reference_a4_hz * std::exp2((static_cast<double>(note) - 69.0) / 12.0);
        try {
            const FretChoice c = choose_fret(f, config, open_frequency_hz, {}, 50.0);
            NoteRoute& r = routes[note];
            r.valid = true;
            r.string_index = static_cast<std::uint8_t>(c.string_index);
            r.fret_number = static_cast<std::uint8_t>(c.fret_number);
            r.cents_error = c.cents_error;
            if (c.fret_number > 0) {
                const double x = finger_position_for_fret(config, c.fret_number, settings);
                r.finger_position_m = x;
                r.finger_height_m =
                    fret_top_height_m(config.strings[c.string_index], x) - settings.press_depth_m;
            }
        } catch (const std::invalid_argument&) {
            // No fret within 50 cents: the route stays invalid and note_on emits nothing.
        }
    }
    std::vector<double> nail(string_count), damp(string_count);
    for (std::size_t s = 0; s < string_count; ++s) {
        nail[s] = config.strings[s].bridge_position_m - settings.pluck_distance_from_bridge_m;
        require(nail[s] > 0.0, "pluck point lies before the nut");
        damp[s] = settings.damp_position_fraction * config.strings[s].bridge_position_m;
    }
    routes_ = std::move(routes);
    nail_position_m_ = std::move(nail);
    damp_position_m_ = std::move(damp);
    sounding_note_.assign(string_count, -1);
    finger_down_.assign(string_count, 0);
    damp_down_.assign(string_count, 0);
    settings_ = settings;
    next_sequence_ = 0;
}

MidiGestureMapper::NoteRoute MidiGestureMapper::route(std::uint8_t note) const noexcept {
    return note < routes_.size() ? routes_[note] : NoteRoute{};
}

std::size_t MidiGestureMapper::note_on(std::uint8_t note, std::uint8_t velocity,
                                       std::uint32_t sample_offset,
                                       std::span<GestureEvent> out) noexcept {
    if (velocity == 0) {
        return note_off(note, sample_offset, out); // MIDI convention
    }
    if (note >= routes_.size() || velocity > 127 || !routes_[note].valid) {
        return 0;
    }
    const NoteRoute& r = routes_[note];
    const std::size_t s = r.string_index;
    const bool fretted = r.fret_number > 0;
    const std::size_t needed =
        (damp_down_[s] ? 1u : 0u) + ((fretted || finger_down_[s]) ? 1u : 0u) + 1u;
    if (out.size() < needed) {
        return 0;
    }
    // Interpolate the six dynamic levels pp..ff linearly over velocity 1..127.
    const std::array<StrokeDynamics, 6> levels{settings_.pp, settings_.p, settings_.mp,
                                               settings_.mf, settings_.f, settings_.ff};
    const double position = (static_cast<double>(velocity) - 1.0) / 126.0 * 5.0;
    const auto lower = std::min<std::size_t>(static_cast<std::size_t>(position), 4);
    const double w = position - static_cast<double>(lower);
    const double speed =
        (1.0 - w) * levels[lower].speed_m_per_s + w * levels[lower + 1].speed_m_per_s;
    const double force =
        (1.0 - w) * levels[lower].release_force_n + w * levels[lower + 1].release_force_n;

    std::size_t count = 0;
    auto base = [&](GestureKind kind, std::uint8_t slot) -> GestureEvent& {
        GestureEvent& e = out[count++];
        e = GestureEvent{};
        e.sample_offset = sample_offset;
        e.sequence = next_sequence_++;
        e.string_index = static_cast<std::uint8_t>(s);
        e.slot = slot;
        e.kind = kind;
        e.speed_m_per_s = settings_.finger_speed_m_per_s;
        e.gap_m = settings_.finger_gap_m;
        return e;
    };
    if (damp_down_[s]) {
        GestureEvent& e = base(GestureKind::FingerRelease, 1);
        e.position_m = damp_position_m_[s];
        damp_down_[s] = 0;
    }
    if (fretted) {
        GestureEvent& e = base(GestureKind::FingerPress, 0);
        e.position_m = r.finger_position_m;
        e.height_m = r.finger_height_m;
        finger_down_[s] = 1;
    } else if (finger_down_[s]) {
        base(GestureKind::FingerRelease, 0);
        finger_down_[s] = 0;
    }
    GestureEvent& stroke = base(GestureKind::NailStroke, 0);
    const double extra = speed * settings_.finger_lead_time_s;
    stroke.position_m = nail_position_m_[s];
    stroke.angle_rad = stroke_angle(Direction::Down, settings_.stroke_angle_rad);
    stroke.speed_m_per_s = speed;
    stroke.force_n = force;
    stroke.gap_m = settings_.nail_gap_m + extra;
    stroke.travel_m = settings_.nail_travel_m + extra;
    sounding_note_[s] = static_cast<std::int16_t>(note);
    return count;
}

std::size_t MidiGestureMapper::note_off(std::uint8_t note, std::uint32_t sample_offset,
                                        std::span<GestureEvent> out) noexcept {
    if (note >= routes_.size() || !routes_[note].valid) {
        return 0;
    }
    const std::size_t s = routes_[note].string_index;
    if (sounding_note_[s] != static_cast<std::int16_t>(note)) {
        return 0; // a later note owns the string
    }
    const std::size_t needed = 1u + (finger_down_[s] ? 1u : 0u);
    if (out.size() < needed) {
        return 0;
    }
    std::size_t count = 0;
    GestureEvent& damp = out[count++];
    damp = GestureEvent{};
    damp.sample_offset = sample_offset;
    damp.sequence = next_sequence_++;
    damp.string_index = static_cast<std::uint8_t>(s);
    damp.slot = 1;
    damp.kind = GestureKind::FingerPress;
    damp.position_m = damp_position_m_[s];
    damp.height_m = -settings_.touch_depth_m;
    damp.speed_m_per_s = settings_.finger_speed_m_per_s;
    damp.gap_m = settings_.finger_gap_m;
    damp_down_[s] = 1;
    if (finger_down_[s]) {
        GestureEvent& lift = out[count++];
        lift = GestureEvent{};
        lift.sample_offset = sample_offset;
        lift.sequence = next_sequence_++;
        lift.string_index = static_cast<std::uint8_t>(s);
        lift.slot = 0;
        lift.kind = GestureKind::FingerRelease;
        lift.speed_m_per_s = settings_.finger_speed_m_per_s;
        lift.gap_m = settings_.finger_gap_m;
        finger_down_[s] = 0;
    }
    sounding_note_[s] = -1;
    return count;
}

} // namespace setar::instrument
