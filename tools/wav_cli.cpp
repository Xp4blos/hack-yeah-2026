// wav_cli: run WAV files through the ambient detector and print an event timeline.
// Accepts PCM 16-bit WAV, any channel count (downmixed to mono), sample rate >= 8000.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ambient/detector.hpp"

using namespace ambient;

struct Wav {
    int sample_rate = 0;
    std::vector<std::int16_t> mono;
};

static std::uint32_t rd32(const std::uint8_t* p) {
    return p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}
static std::uint16_t rd16(const std::uint8_t* p) { return static_cast<std::uint16_t>(p[0] | (p[1] << 8)); }

static Wav load_wav(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("cannot open file");
    std::vector<std::uint8_t> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (b.size() < 12 || std::memcmp(b.data(), "RIFF", 4) != 0 || std::memcmp(b.data() + 8, "WAVE", 4) != 0)
        throw std::runtime_error("not a RIFF/WAVE file");

    int channels = 0, bits = 0, rate = 0, tag = 0;
    const std::uint8_t* data = nullptr;
    std::size_t data_len = 0;

    std::size_t pos = 12;
    while (pos + 8 <= b.size()) {
        const std::uint8_t* h = b.data() + pos;
        std::size_t len = rd32(h + 4);
        const std::size_t body = pos + 8;
        const std::size_t avail = b.size() - body;
        if (len > avail) len = avail;  // truncated / streaming header
        if (std::memcmp(h, "fmt ", 4) == 0 && len >= 16) {
            tag = rd16(b.data() + body);
            channels = rd16(b.data() + body + 2);
            rate = static_cast<int>(rd32(b.data() + body + 4));
            bits = rd16(b.data() + body + 14);
        } else if (std::memcmp(h, "data", 4) == 0) {
            data = b.data() + body;
            data_len = len;
            break;
        }
        pos = body + len + (len & 1);  // chunks are word-aligned
    }
    if (channels == 0) throw std::runtime_error("missing fmt chunk");
    if (!data) throw std::runtime_error("missing data chunk");
    if ((tag != 1 && tag != 0xFFFE) || bits != 16)
        throw std::runtime_error("only 16-bit PCM WAV is supported (convert with ffmpeg)");
    if (rate < 8000) throw std::runtime_error("sample rate below 8000 Hz");

    Wav w;
    w.sample_rate = rate;
    const std::size_t frames = data_len / (2u * static_cast<std::size_t>(channels));
    w.mono.resize(frames);
    for (std::size_t i = 0; i < frames; ++i) {
        int sum = 0;
        for (int c = 0; c < channels; ++c)
            sum += static_cast<std::int16_t>(rd16(data + (i * channels + c) * 2));
        w.mono[i] = static_cast<std::int16_t>(sum / channels);
    }
    return w;
}

static void usage() {
    std::puts(
        "usage: wav_cli [options] file.wav [file2.wav ...]\n"
        "  --chunk N        feed N samples at a time (default 480)\n"
        "  --csv            machine-readable output: file,time_s,type,confidence,freq_hz,level_db\n"
        "  --expect LIST    e.g. alarm=1,knock=0 ; exit code 2 if any file differs\n"
        "  --tonal X        tonal_ratio_min       --onset X     onset_db\n"
        "  --min-level X    min_level_db (dBFS)   --decay X     decay_db\n"
        "  --alarm-min-s X  alarm_min_s           --knock-max-s X\n"
        "event types: alarm, knock, loud_sound");
}

static bool parse_expect(const std::string& s, std::map<std::string, int>& out) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
        const auto eq = item.find('=');
        if (eq == std::string::npos) return false;
        out[item.substr(0, eq)] = std::atoi(item.c_str() + eq + 1);
    }
    return !out.empty();
}

int main(int argc, char** argv) {
    Config cfg;
    std::size_t chunk = 480;
    bool csv = false;
    std::map<std::string, int> expect;
    std::vector<std::string> files;

    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        auto next = [&](const char* name) -> const char* {
            if (i + 1 >= argc) {
                std::fprintf(stderr, "missing value for %s\n", name);
                std::exit(1);
            }
            return argv[++i];
        };
        if (a == "--help" || a == "-h") { usage(); return 0; }
        else if (a == "--csv") csv = true;
        else if (a == "--chunk") chunk = static_cast<std::size_t>(std::atoll(next("--chunk")));
        else if (a == "--expect") {
            if (!parse_expect(next("--expect"), expect)) { std::fprintf(stderr, "bad --expect\n"); return 1; }
        }
        else if (a == "--tonal") cfg.tonal_ratio_min = static_cast<float>(std::atof(next("--tonal")));
        else if (a == "--onset") cfg.onset_db = static_cast<float>(std::atof(next("--onset")));
        else if (a == "--min-level") cfg.min_level_db = static_cast<float>(std::atof(next("--min-level")));
        else if (a == "--decay") cfg.decay_db = static_cast<float>(std::atof(next("--decay")));
        else if (a == "--alarm-min-s") cfg.alarm_min_s = static_cast<float>(std::atof(next("--alarm-min-s")));
        else if (a == "--knock-max-s") cfg.knock_max_s = static_cast<float>(std::atof(next("--knock-max-s")));
        else if (!a.empty() && a[0] == '-') { std::fprintf(stderr, "unknown option %s\n", a.c_str()); usage(); return 1; }
        else files.push_back(a);
    }
    if (files.empty() || chunk == 0) { usage(); return 1; }

    if (csv) std::puts("file,time_s,type,confidence,freq_hz,level_db");

    int exit_code = 0;
    for (const auto& path : files) {
        try {
            const Wav w = load_wav(path);
            Config c = cfg;
            c.sample_rate = w.sample_rate;
            Detector det(c);  // may throw invalid_argument on bad thresholds

            std::vector<Event> events;
            for (std::size_t i = 0; i < w.mono.size(); i += chunk) {
                auto ev = det.process(w.mono.data() + i, std::min(chunk, w.mono.size() - i));
                events.insert(events.end(), ev.begin(), ev.end());
            }

            std::map<std::string, int> counts;
            for (const auto& e : events) ++counts[to_string(e.type)];

            if (csv) {
                for (const auto& e : events)
                    std::printf("%s,%.3f,%s,%.2f,%.0f,%.1f\n", path.c_str(), e.time_s,
                                to_string(e.type), e.confidence, e.freq_hz, e.level_db);
            } else {
                std::printf("== %s  (%.2f s, %d Hz)\n", path.c_str(),
                            static_cast<double>(w.mono.size()) / w.sample_rate, w.sample_rate);
                for (const auto& e : events)
                    std::printf("  %7.2fs  %-10s conf=%.2f  freq=%4.0f Hz  level=%6.1f dBFS\n",
                                e.time_s, to_string(e.type), e.confidence, e.freq_hz, e.level_db);
                if (events.empty()) std::puts("  (no events)");
            }

            for (const auto& kv : expect) {
                const int got = counts.count(kv.first) ? counts[kv.first] : 0;
                if (got != kv.second) {
                    std::fprintf(stderr, "EXPECT FAIL %s: %s expected %d, got %d\n", path.c_str(),
                                 kv.first.c_str(), kv.second, got);
                    exit_code = 2;
                }
            }
        } catch (const std::exception& e) {
            std::fprintf(stderr, "%s: %s\n", path.c_str(), e.what());
            if (exit_code == 0) exit_code = 1;
        }
    }
    return exit_code;
}
