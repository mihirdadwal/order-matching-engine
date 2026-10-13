# ------------------------------------------------------------
# CMakeLists.txt  – build instructions for the project
# Think of this like package.json (npm) or pom.xml (Maven).
# ------------------------------------------------------------

# Minimum CMake version we need
cmake_minimum_required(VERSION 3.16)

# Project name and language (CXX = C++)
project(OrderMatchingEngine LANGUAGES CXX)

# Use C++17 and don't fall back to an older version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build an executable called "engine" from these source files.
# We'll add more .cpp files here as the project grows.
add_executable(engine
    src/main.cpp
)

# Let the compiler find our header files in the include/ folder
target_include_directories(engine PRIVATE include)

# Turn on helpful compiler warnings (catches common mistakes)
target_compile_options(engine PRIVATE -Wall -Wextra)
