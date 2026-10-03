cmake_minimum_required(VERSION 3.16)
project(ambient_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Threads REQUIRED)  # Detector and SpeechSession use std::mutex

# ---------------------------------------------------------------- library
add_library(ambient_core STATIC
        src/fft.cpp
        src/detector.cpp
        src/custom.cpp
        src/trainer.cpp
        src/speech.cpp
)
target_include_directories(ambient_core PUBLIC include)
target_link_libraries(ambient_core PUBLIC Threads::Threads)
target_compile_features(ambient_core PUBLIC cxx_std_17)

option(AMBIENT_WERROR "Treat warnings as errors (use in CI)" OFF)
option(AMBIENT_SANITIZE "Build with AddressSanitizer + UBSan (use in CI)" OFF)

if(MSVC)
  target_compile_options(ambient_core PRIVATE /W4)
  if(AMBIENT_WERROR)
    target_compile_options(ambient_core PRIVATE /WX)
  endif()
else()
  target_compile_options(ambient_core PRIVATE -Wall -Wextra -Wpedantic -fstack-protector-strong)
  target_compile_definitions(ambient_core PRIVATE _GLIBCXX_ASSERTIONS)
  if(AMBIENT_WERROR)
    target_compile_options(ambient_core PRIVATE -Werror)
  endif()
  if(AMBIENT_SANITIZE)
    # PUBLIC: tests and tools linking ambient_core inherit the sanitizers
    target_compile_options(ambient_core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(ambient_core PUBLIC -fsanitize=address,undefined)
  endif()
endif()

# ------------------------------------------------------------------ tests
option(AMBIENT_BUILD_TESTS "Build unit tests" ON)
if(AMBIENT_BUILD_TESTS)
  enable_testing()

  foreach(t test_core test_custom test_safety)
    add_executable(${t} tests/${t}.cpp)
    target_link_libraries(${t} PRIVATE ambient_core)
    add_test(NAME ${t} COMMAND ${t})
  endforeach()

  add_executable(speech_test tests/speech_test.cpp)
  target_link_libraries(speech_test PRIVATE ambient_core)
  add_test(NAME speech_test COMMAND speech_test)
endif()

# ------------------------------------------------------------------ tools
option(AMBIENT_BUILD_TOOLS "Build wav_cli" ON)
if(AMBIENT_BUILD_TOOLS)
  add_executable(wav_cli tools/wav_cli.cpp)
  target_link_libraries(wav_cli PRIVATE ambient_core)
endif()

# End-to-end checks of wav_cli on generated samples (tests/data, see tools/gen_test_wavs.py)
if(AMBIENT_BUILD_TESTS AND AMBIENT_BUILD_TOOLS)
  set(S ${CMAKE_CURRENT_SOURCE_DIR}/tests/data)
  add_test(NAME wav_alarm_knock
          COMMAND wav_cli --expect alarm=1,knock=1,loud_sound=0 ${S}/sample_alarm_knock.wav)
  add_test(NAME wav_loud
          COMMAND wav_cli --expect alarm=0,knock=0,loud_sound=1 ${S}/sample_loud.wav)
  add_test(NAME wav_custom_fridge
          COMMAND wav_cli --learn fridge=${S}/sample_fridge_learn.wav
          --expect custom:fridge=2,knock=0,loud_sound=0 ${S}/sample_fridge_stream.wav)
  add_test(NAME wav_quiet_noise
          COMMAND wav_cli --expect alarm=0,knock=0,loud_sound=0 ${S}/sample_quiet_noise.wav)
endif()