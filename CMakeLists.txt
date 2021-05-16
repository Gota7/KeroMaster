cmake_minimum_required(VERSION 3.11)
project(KeroMaster)

if (MINGW)
  message(STATUS "MinGW detected")
  set(WIN32 TRUE)
  set(UNIX FALSE)
  set(CMAKE_SYSTEM_NAME "Windows")
endif()

set(BUILD_EXAMPLES OFF)
ADD_SUBDIRECTORY(include)
find_package(raylib 3.7 QUIET)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if (NOT raylib_FOUND)
  include(FetchContent)

  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/3.7.0.zip
  )

  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

set(BINARY_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIR})
include_directories("src")
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} raylib imgui)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC include)

if (MINGW)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -s")
endif()

if (WIN32)
  target_link_libraries(${PROJECT_NAME} imm32 opengl32 gdi32)
endif()