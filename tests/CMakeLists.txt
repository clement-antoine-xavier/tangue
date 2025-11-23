include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT for googletest" FORCE)

FetchContent_MakeAvailable(googletest)

set(TESTS_HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers)
set(TESTS_SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sources)

include_directories(${TESTS_HEADERS_DIRECTORY})

file(GLOB_RECURSE SOURCES ${TESTS_SOURCES_DIRECTORY}/*.cpp)

add_executable(tangue_tests ${SOURCES})

target_link_libraries(tangue_tests
    PRIVATE
    tangue
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(tangue_tests)
