# ------------------------------------------------------------
# CMakeLists.txt  – build instructions for the project
# ------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project(OrderMatchingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compile engine executable from our source files
add_executable(engine
    src/main.cpp
    src/OrderBook.cpp
    src/CommandParser.cpp
)
target_include_directories(engine PRIVATE include)
target_compile_options(engine PRIVATE -Wall -Wextra)

# ------------------------------------------------------------
# GoogleTest Integration via FetchContent
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
    src/OrderBook.cpp
    src/CommandParser.cpp
)
target_include_directories(order_tests PRIVATE include)
target_link_libraries(order_tests PRIVATE gtest_main)
target_compile_options(order_tests PRIVATE -Wall -Wextra)

include(GoogleTest)
gtest_discover_tests(order_tests)
