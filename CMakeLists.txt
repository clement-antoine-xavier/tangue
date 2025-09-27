cmake_minimum_required(VERSION 3.10)

project(tangue
	VERSION 1.0.0
	DESCRIPTION "Tangue"
	HOMEPAGE_URL "https://clement-antoine-xavier.github.io/tangue"
	LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

set(HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers)
set(SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sources)

include_directories(${HEADERS_DIRECTORY})

file(GLOB_RECURSE SOURCES ${SOURCES_DIRECTORY}/*.c)

add_executable(${PROJECT_NAME} ${SOURCES})
