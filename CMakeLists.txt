cmake_minimum_required(VERSION 3.14)

project(tangue
	VERSION 1.0.0
	DESCRIPTION "Tangue"
	HOMEPAGE_URL "https://clement-antoine-xavier.github.io/tangue"
	LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

set(SDL_TEST_LIBRARY OFF)

FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.22
)

FetchContent_MakeAvailable(SDL)

FetchContent_Declare(
  SDL_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG release-3.2.4
)

FetchContent_MakeAvailable(SDL_image)

FetchContent_Declare(
  SDL_ttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
  GIT_TAG release-3.2.2
)

FetchContent_MakeAvailable(SDL_ttf)

set(HEADERS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/headers)
set(SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sources)

include_directories(${HEADERS_DIRECTORY})

file(GLOB_RECURSE SOURCES ${SOURCES_DIRECTORY}/*.c)
list(REMOVE_ITEM SOURCES ${SOURCES_DIRECTORY}/main_old.c)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)
