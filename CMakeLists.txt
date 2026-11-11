cmake_minimum_required(VERSION 3.20)
project(lsmdb LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)

option(LSM_SANITIZE "Build with ASan+UBSan" ON)
option(LSM_TSAN "Build with ThreadSanitizer (mutually exclusive with LSM_SANITIZE)" OFF)
if(LSM_TSAN)
  add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
  add_link_options(-fsanitize=thread)
elseif(LSM_SANITIZE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

# Library: add your .cc files here as you write them.
add_library(lsm
  src/memtable.cc
)
target_include_directories(lsm PUBLIC include)

# Tests
find_package(GTest QUIET)
if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

enable_testing()
include(GoogleTest)

add_executable(lsm_tests
  tests/memtable_test.cc
)
target_link_libraries(lsm_tests PRIVATE lsm GTest::gtest_main)
gtest_discover_tests(lsm_tests)

# Benchmark driver (milestone 8)
add_executable(lsm_bench bench/db_bench.cc)
target_link_libraries(lsm_bench PRIVATE lsm)

# Fuzzers need clang: cmake -DCMAKE_CXX_COMPILER=clang++ -DLSM_FUZZ=ON
option(LSM_FUZZ "Build libFuzzer targets (clang only)" OFF)
if(LSM_FUZZ)
  add_executable(fuzz_memtable fuzz/fuzz_memtable.cc)
  target_compile_options(fuzz_memtable PRIVATE -fsanitize=fuzzer,address,undefined)
  target_link_options(fuzz_memtable PRIVATE -fsanitize=fuzzer,address,undefined)
  target_link_libraries(fuzz_memtable PRIVATE lsm)
endif()
