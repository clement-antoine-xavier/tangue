set(EXAMPLE_HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers)
set(EXAMPLE_SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sources)

include_directories(${EXAMPLE_HEADERS_DIRECTORY})

file(GLOB_RECURSE SOURCES ${EXAMPLE_SOURCES_DIRECTORY}/*.cpp)

add_executable(simple_application ${SOURCES})

target_link_libraries(simple_application
    PRIVATE
    tangue
)
