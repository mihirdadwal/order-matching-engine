# ------------------------------------------------------------
# CMakeLists.txt  – build instructions for the project
# ------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project(OrderMatchingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENGINE_SOURCES
    src/OrderBook.cpp
    src/CommandParser.cpp
    src/MarketStats.cpp
    src/Display.cpp
    src/Exchange.cpp
    src/CsvLoader.cpp
)

# 1. Main interactive engine executable
add_executable(engine
    src/main.cpp
    ${ENGINE_SOURCES}
)
target_include_directories(engine PRIVATE include)
target_compile_options(engine PRIVATE -Wall -Wextra)

# 2. Performance Benchmark executable (compiled with -O3 optimization)
add_executable(benchmark
    bench/Benchmark.cpp
    ${ENGINE_SOURCES}
)
target_include_directories(benchmark PRIVATE include)
target_compile_options(benchmark PRIVATE -Wall -Wextra -O3)

# ------------------------------------------------------------
# 3. GoogleTest Integration via FetchContent
# ------------------------------------------------------------
set(CMAKE_TLS_VERIFY OFF)
cmake_policy(SET CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(order_tests
    tests/OrderBookTests.cpp
    ${ENGINE_SOURCES}
)
target_include_directories(order_tests PRIVATE include)
target_link_libraries(order_tests PRIVATE gtest_main)
target_compile_options(order_tests PRIVATE -Wall -Wextra)

include(GoogleTest)
gtest_discover_tests(order_tests)
