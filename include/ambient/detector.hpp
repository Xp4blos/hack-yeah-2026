#pragma once
#include <complex>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "ambient/fft.hpp"

namespace ambient {

enum class EventType { Alarm, Knock, LoudSound };

const char* to_string(EventType t);

struct Event {
    EventType type;
    float confidence;  // 0..1, heuristic
    double time_s;     // detection time since start / last reset()
    float freq_hz;     // dominant frequency (Alarm), otherwise 0
    float level_db;    // frame level in dBFS at detection
};

struct Config {
    int sample_rate = 16000;
    std::size_t frame_size = 1024;  // power of two
    std::size_t hop_size = 512;     // 0 < hop <= frame

    float min_level_db = -40.f;     // ignore anything quieter (dBFS)

    // Alarm: sustained, narrow-band tone
    float tonal_ratio_min = 0.5f;   // share of power within +-3 bins of the peak
    float alarm_min_hz = 800.f;
    float alarm_max_hz = 4500.f;
    float alarm_min_s = 0.4f;
    int alarm_gap_frames = 4;       // tolerated non-tonal frames inside one alarm

    // Transients: Knock (short, decays fast) / LoudSound (sustained, broadband)
    float onset_db = 12.f;          // rise over background that counts as an onset
    float decay_db = 10.f;          // drop from peak that ends a knock
    float knock_max_s = 0.25f;
    float background_alpha = 0.05f; // background level smoothing per frame

    // Throws std::invalid_argument when inconsistent.
    void validate() const;
};

// Streaming detector. Feed mono 16-bit PCM in chunks of any size.
class Detector {
public:
    explicit Detector(Config cfg = {});
    std::vector<Event> process(const std::int16_t* samples, std::size_t count);
    void reset();
    const Config& config() const { return cfg_; }

private:
    struct Features {
        float level_db;
        float peak_hz;
        float tonal_ratio;
    };
    enum class State { Idle, Onset, Sustained };

    Features analyze(const float* frame);
    void on_frame(const Features& f, double t, std::vector<Event>& out);
    void reset_state();

    Config cfg_;
    Fft fft_;
    std::vector<float> window_;
    std::vector<std::complex<float>> spec_;
    std::vector<float> pending_;
    std::size_t read_ = 0;        // read offset inside pending_
    std::uint64_t consumed_ = 0;  // samples dropped from the front of pending_
    float hop_s_ = 0.f;

    // alarm state
    int tonal_run_ = 0;
    int miss_run_ = 0;
    bool alarm_active_ = false;

    // transient state
    State state_ = State::Idle;
    bool bg_init_ = false;
    float bg_db_ = -90.f;
    int onset_frames_ = 0;
    int tonal_frames_ = 0;
    float peak_db_ = -90.f;
    float jump_db_ = 0.f;
};

}  // namespace ambient
