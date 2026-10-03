#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

#include "ambient/detector.hpp"

using namespace ambient;

static int g_failed = 0;
#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);        \
            ++g_failed;                                                          \
        }                                                                        \
    } while (0)

constexpr int kSr = 16000;
constexpr double kTwoPi = 6.283185307179586;  // M_PI is not available with -std=c++17 on MinGW
using Pcm = std::vector<std::int16_t>;

static std::int16_t to_pcm(double v) {
    return static_cast<std::int16_t>(std::lround(std::max(-1.0, std::min(1.0, v)) * 32767.0));
}

struct Lcg {  // deterministic noise in [-1, 1]
    std::uint32_t s = 12345;
    double next() {
        s = s * 1664525u + 1013904223u;
        return (static_cast<double>(s >> 8) / 8388608.0) - 1.0;
    }
};

static Pcm noise(double sec, double amp, Lcg& r) {
    Pcm p(static_cast<std::size_t>(sec * kSr));
    for (auto& s : p) s = to_pcm(amp * r.next());
    return p;
}

static Pcm tone(double sec, double hz, double amp, Lcg& r, double noise_amp = 0.002) {
    Pcm p(static_cast<std::size_t>(sec * kSr));
    for (std::size_t i = 0; i < p.size(); ++i) {
        const double t = static_cast<double>(i) / kSr;
        p[i] = to_pcm(amp * std::sin(kTwoPi * hz * t) + noise_amp * r.next());
    }
    return p;
}

static Pcm knock(double amp, Lcg& r) {
    Pcm p(static_cast<std::size_t>(0.2 * kSr));
    for (std::size_t i = 0; i < p.size(); ++i) {
        const double t = static_cast<double>(i) / kSr;
        p[i] = to_pcm(amp * std::exp(-t / 0.015) * r.next());
    }
    return p;
}

static void append(Pcm& a, const Pcm& b) { a.insert(a.end(), b.begin(), b.end()); }

static std::vector<Event> run(const Pcm& pcm, std::size_t chunk = 0) {
    Detector d;
    std::vector<Event> all;
    if (chunk == 0) chunk = pcm.size();
    for (std::size_t i = 0; i < pcm.size(); i += chunk) {
        auto ev = d.process(pcm.data() + i, std::min(chunk, pcm.size() - i));
        all.insert(all.end(), ev.begin(), ev.end());
    }
    return all;
}

static int count(const std::vector<Event>& ev, EventType t) {
    return static_cast<int>(std::count_if(ev.begin(), ev.end(),
                                          [t](const Event& e) { return e.type == t; }));
}

static void test_background_noise_is_silent() {
    std::puts("background noise -> no events");
    Lcg r;
    CHECK(run(noise(4.0, 0.003, r)).empty());
}

static void test_alarm_tone() {
    std::puts("2 kHz tone -> one alarm");
    Lcg r;
    Pcm p = noise(1.0, 0.002, r);
    append(p, tone(1.0, 2000.0, 0.3, r));
    append(p, noise(1.0, 0.002, r));
    const auto ev = run(p);
    CHECK(count(ev, EventType::Alarm) == 1);
    CHECK(count(ev, EventType::LoudSound) == 0);
    for (const auto& e : ev) {
        if (e.type == EventType::Alarm) {
            CHECK(std::fabs(e.freq_hz - 2000.f) < 40.f);
            CHECK(e.time_s > 1.3 && e.time_s < 2.1);
            CHECK(e.confidence > 0.5f);
        }
    }
}

static void test_two_alarms() {
    std::puts("two separated tones -> two alarms");
    Lcg r;
    Pcm p = noise(1.0, 0.002, r);
    append(p, tone(1.0, 1500.0, 0.3, r));
    append(p, noise(1.5, 0.002, r));
    append(p, tone(1.0, 3000.0, 0.3, r));
    append(p, noise(1.0, 0.002, r));
    CHECK(count(run(p), EventType::Alarm) == 2);
}

static void test_knock() {
    std::puts("short decaying burst -> knock");
    Lcg r;
    Pcm p = noise(1.0, 0.002, r);
    append(p, knock(0.6, r));
    append(p, noise(1.0, 0.002, r));
    const auto ev = run(p);
    CHECK(count(ev, EventType::Knock) == 1);
    CHECK(count(ev, EventType::LoudSound) == 0);
    CHECK(count(ev, EventType::Alarm) == 0);
}

static void test_loud_sound() {
    std::puts("sustained broadband noise -> loud sound");
    Lcg r;
    Pcm p = noise(1.0, 0.002, r);
    append(p, noise(1.5, 0.5, r));
    append(p, noise(1.0, 0.002, r));
    const auto ev = run(p);
    CHECK(count(ev, EventType::LoudSound) == 1);
    CHECK(count(ev, EventType::Alarm) == 0);
    CHECK(count(ev, EventType::Knock) == 0);
}

static void test_chunking_invariance() {
    std::puts("chunked input == one-shot input");
    Lcg r;
    Pcm p = noise(1.0, 0.002, r);
    append(p, tone(1.0, 2500.0, 0.3, r));
    append(p, noise(1.0, 0.002, r));
    append(p, knock(0.6, r));
    append(p, noise(1.0, 0.002, r));
    const auto a = run(p);
    for (std::size_t chunk : {1u, 7u, 480u, 1000u, 4096u}) {
        const auto b = run(p, chunk);
        CHECK(a.size() == b.size());
        for (std::size_t i = 0; i < std::min(a.size(), b.size()); ++i) {
            CHECK(a[i].type == b[i].type);
            CHECK(std::fabs(a[i].time_s - b[i].time_s) < 1e-9);
        }
    }
}

static void test_reset() {
    std::puts("reset() restores a clean state");
    Lcg r;
    Pcm p = tone(1.0, 2000.0, 0.3, r);
    Detector d;
    const auto a = d.process(p.data(), p.size());
    d.reset();
    const auto b = d.process(p.data(), p.size());
    CHECK(a.size() == b.size());
}

static void test_bad_input() {
    std::puts("invalid input / config handled");
    Detector d;
    CHECK(d.process(nullptr, 100).empty());
    std::int16_t x = 0;
    CHECK(d.process(&x, 0).empty());

    bool threw = false;
    try { Config c; c.frame_size = 1000; Detector bad(c); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
    threw = false;
    try { Config c; c.hop_size = 0; Detector bad(c); } catch (const std::invalid_argument&) { threw = true; }
    CHECK(threw);
}

int main() {
    test_background_noise_is_silent();
    test_alarm_tone();
    test_two_alarms();
    test_knock();
    test_loud_sound();
    test_chunking_invariance();
    test_reset();
    test_bad_input();
    if (g_failed == 0) std::puts("ALL TESTS PASSED");
    else std::printf("%d CHECK(S) FAILED\n", g_failed);
    return g_failed == 0 ? 0 : 1;
}
