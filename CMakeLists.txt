cmake_minimum_required(VERSION 3.20)

# Project name and language. Everything below hangs off this.
project(ParticleEngine VERSION 1.0 LANGUAGES C CXX)

# Default to a Release build if the user didn't pick one. Without this, a
# single-config generator (Ninja, Makefiles) builds with no optimisation, which
# makes a particle sim look far slower than it is. Multi-config generators
# (Visual Studio) ignore this and choose with `--config` instead.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Don't fall back to compiler-specific dialects (e.g. -std=gnu++20); keeps the
# code portable between GCC, Clang and MSVC. The C++ standard itself is set per
# target below with target_compile_features.
set(CMAKE_CXX_EXTENSIONS OFF)

# Write build/compile_commands.json, which lists every file's include paths and
# flags. VS Code's IntelliSense reads it so it understands the folder layout.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options. Tests are off by default so a normal build doesn't download GoogleTest.
option(BUILD_TESTS "Build the unit tests (downloads GoogleTest)" OFF)

# Warnings helper. Applied only to OUR targets, never to raylib/GoogleTest, so
# their warnings don't drown out ours. No -Werror: warnings inform, they don't fail.
function(enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra)
    endif()
endfunction()

# raylib, downloaded and built from source at configure time so nobody needs it
# installed. Pinned to release tag 5.5 so builds are reproducible. The options
# switch off raylib's examples so we only build the library itself.
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# physics: the simulation as a static library. It deliberately links nothing, so
# it cannot see raylib and cannot accidentally depend on it. The include
# directory is PUBLIC so anything linking `physics` can write #include "physics.h".
add_library(physics STATIC
    src/physics/config.cpp
    src/physics/physics.cpp
    src/physics/spatialHasher.cpp
    src/physics/vec2.cpp
)
target_include_directories(physics PUBLIC src/physics)
target_compile_features(physics PUBLIC cxx_std_20)  # PUBLIC: consumers also build as C++20
enable_warnings(physics)

# app: the game window. It's the only target that links raylib. Linking `raylib`
# also pulls in its include path and the system libs it needs (OpenGL, winmm...).
add_executable(app
    src/app/main.cpp
    src/app/render.cpp
    src/app/playerInteraction.cpp
)
target_link_libraries(app PRIVATE physics raylib)
enable_warnings(app)

# bench: placeholder for benchmarks. Links physics only (no raylib), so
# benchmarks measure the simulation alone, not rendering.
add_executable(bench bench/bench_main.cpp)
target_link_libraries(bench PRIVATE physics)
enable_warnings(bench)

# tests: placeholder for unit tests, built only with -DBUILD_TESTS=ON. GoogleTest
# is pinned to release v1.18.0. Links physics only, so tests never need a window.
if(BUILD_TESTS)
    enable_testing()

    # On Windows/MSVC, make GoogleTest use the same runtime library as our code.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.18.0
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(tests tests/tests_placeholder.cpp)
    target_link_libraries(tests PRIVATE physics GTest::gtest_main)
    enable_warnings(tests)

    add_test(NAME tests COMMAND tests)  # lets `ctest` run it
endif()
