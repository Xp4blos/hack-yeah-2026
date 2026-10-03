cmake_minimum_required(VERSION 3.4.1)

project(ambient_native)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions("-DOHOS_PLATFORM")

# Include the core ambient headers and source
include_directories(${NATIVERENDER_ROOT_PATH}/../../include)
include_directories(${NATIVERENDER_ROOT_PATH}/../../src)

add_library(ambient_bridge SHARED 
    hello.cpp
    ../../src/detector.cpp
)

target_link_libraries(ambient_bridge PUBLIC libace_napi.z.so hibase.z.so)
