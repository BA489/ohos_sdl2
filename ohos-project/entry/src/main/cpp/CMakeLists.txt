# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(entry)

set(env ${CMAKE_CURRENT_SOURCE_DIR})
set(SDL_HOME_PATH ohos_sdl2)
set(NATIVE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${SDL_HOME_PATH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/${SDL_HOME_PATH}/src/video/khronos/GLES2)
set(NATIVE_ENTRY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/application)


add_subdirectory(${SDL_HOME_PATH})
include_directories(${NATIVE_INCLUDE_PATH})

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file(GLOB my_files "${NATIVE_ENTRY_PATH}/*.c" "${NATIVE_ENTRY_PATH}/*.h")

add_library(entry SHARED ${my_files})

add_dependencies(entry SDL2)

target_link_libraries(entry PUBLIC libSDL2d.so)