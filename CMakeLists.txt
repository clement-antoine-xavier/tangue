# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(tangue VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the sources
set(SOURCES_DIRECTORY sources)
file(GLOB SOURCES ${SOURCES_DIRECTORY}/*.cpp)

# Set the headers
set(HEADERS_DIRECTORY headers)

# Set the include directories
include_directories(${HEADERS_DIRECTORY})

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})
