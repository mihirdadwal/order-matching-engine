# ------------------------------------------------------------
# CMakeLists.txt  – build instructions for the project
# ------------------------------------------------------------

cmake_minimum_required(VERSION 3.16)

project(OrderMatchingEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compile engine from our source files
add_executable(engine
    src/main.cpp
    src/OrderBook.cpp
)

# Look for headers in include/
target_include_directories(engine PRIVATE include)

# Compile warnings
target_compile_options(engine PRIVATE -Wall -Wextra)
