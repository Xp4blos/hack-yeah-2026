#!/usr/bin/env python3
"""Generate synthetic WAV samples (16 kHz, mono, 16-bit) into tests/data/."""
import math, os, random, struct, wave

SR = 16000
OUT = os.path.join(os.path.dirname(__file__), "..", "tests", "data")
rnd = random.Random(1)

def noise(sec, amp):
    return [amp * rnd.uniform(-1, 1) for _ in range(int(sec * SR))]

def tone(sec, hz, amp):
    return [amp * math.sin(2 * math.pi * hz * i / SR) + 0.002 * rnd.uniform(-1, 1)
            for i in range(int(sec * SR))]

def knock(amp):
    return [amp * math.exp(-(i / SR) / 0.015) * rnd.uniform(-1, 1) for i in range(int(0.2 * SR))]

def write(name, samples):
    os.makedirs(OUT, exist_ok=True)
    with wave.open(os.path.join(OUT, name), "wb") as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
        w.writeframes(b"".join(struct.pack("<h", max(-32768, min(32767, int(s * 32767)))) for s in samples))

write("sample_alarm_knock.wav",
      noise(1, .002) + tone(1, 2000, .3) + noise(1, .002) + knock(.6) + noise(1, .002))
write("sample_loud.wav", noise(1, .002) + noise(1.5, .5) + noise(1, .002))
write("sample_quiet_noise.wav", noise(4, .003))
print("written to", os.path.abspath(OUT))
