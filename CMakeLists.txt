cmake_minimum_required(VERSION 3.10)

project(tangue
    VERSION 1.0.0
    DESCRIPTION "Tangue"
    HOMEPAGE_URL "https://clement-antoine-xavier.github.io/tangue"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers)
set(SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sources)

include_directories(${HEADERS_DIRECTORY})

file(GLOB_RECURSE SOURCES ${SOURCES_DIRECTORY}/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADERS_DIRECTORY})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${HEADERS_DIRECTORY}/application.hpp"
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/tangue
)

option(BUILD_TESTS "Build the tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(BUILD_EXAMPLES "Build the examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()