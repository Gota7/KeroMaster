cmake_minimum_required(VERSION 3.20)
project(KeroMaster)

if (MINGW)
  message(STATUS "MinGW detected")
  set(WIN32 TRUE)
  set(UNIX FALSE)
  set(CMAKE_SYSTEM_NAME "Windows")
endif()

set(BUILD_EXAMPLES OFF)
ADD_SUBDIRECTORY(deps)

find_package(raylib 5.5 QUIET)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if (NOT raylib_FOUND)
  include(FetchContent)

  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip
  )

  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  endif()
endif()

include_directories("src")

set(SOURCES
src/bgm/bgm.cpp
  src/bgm/pxtone/pxtnDelay.cpp
  src/bgm/pxtone/pxtnDescriptor.cpp
  src/bgm/pxtone/pxtnError.cpp
  src/bgm/pxtone/pxtnEvelist.cpp
  src/bgm/pxtone/pxtnMaster.cpp
  src/bgm/pxtone/pxtnMem.cpp
  src/bgm/pxtone/pxtnOverDrive.cpp
  src/bgm/pxtone/pxtnPulse_Frequency.cpp
  src/bgm/pxtone/pxtnPulse_Noise.cpp
  src/bgm/pxtone/pxtnPulse_NoiseBuilder.cpp
  src/bgm/pxtone/pxtnPulse_Oggv.cpp
  src/bgm/pxtone/pxtnPulse_Oscillator.cpp
  src/bgm/pxtone/pxtnPulse_PCM.cpp
  src/bgm/pxtone/pxtnService_moo.cpp
  src/bgm/pxtone/pxtnService.cpp
  src/bgm/pxtone/pxtnText.cpp
  src/bgm/pxtone/pxtnUnit.cpp
  src/bgm/pxtone/pxtnWoice_io.cpp
  src/bgm/pxtone/pxtnWoice.cpp
  src/bgm/pxtone/pxtnWoicePTV.cpp
  src/bgm/pxtone/pxtoneNoise.cpp
  src/conv/conv.cpp
  src/ed/attributeEditor.cpp
  src/ed/editor.cpp
  src/ed/entityDisplay.cpp
  src/ed/entityEditor.cpp
  src/ed/help.cpp
  src/ed/imageViewer.cpp
  src/ed/levelEditor.cpp
  src/ed/musicPlayer.cpp
  src/ed/palette.cpp
  src/ed/profileEditor.cpp
  src/ed/scriptEditor.cpp
  src/ed/selection.cpp
  src/ed/settings.cpp
  src/ed/style.cpp
  src/ed/styleEditor.cpp
  src/ed/tilesetEditor.cpp
  src/ed/toolbar.cpp
  src/ed/tools/entityHand.cpp
  src/ed/tools/eraser.cpp
  src/ed/tools/hand.cpp
  src/ed/tools/tileBrush.cpp
  src/ed/tools/tools.cpp
  src/ed/tools/zoom.cpp
  src/ed/undoStack.cpp
  src/gbin/gfile.cpp
  src/gbin/gtypes.cpp
  src/main.cpp
  src/px/profile.cpp
  src/px/pxmap.cpp
  src/px/str.cpp
  src/px/tileset.cpp
  src/rlImGui/dateChooser.cpp
  src/rlImGui/fileBrowser.cpp
  src/rlImGui/focusData.cpp
  src/rlImGui/rlImGui.cpp
  src/rlImGui/utils.cpp
  src/tinyxml2/tinyxml2.cpp
)

if(APPLE)
  list(APPEND SOURCES
    src/platform.mm
  )
else()
  list(APPEND SOURCES
    src/platform.cpp
  )
endif()

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
  ${SOURCES}
)
target_link_libraries(${PROJECT_NAME} raylib imgui)

if(CMAKE_VERSION VERSION_GREATER 4.0)
  set_property(TARGET ${PROJECT_NAME} PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}") # CMake 4.0+
else()
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
  set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

if(MINGW)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -s")
endif()

if (WIN32)
  target_link_libraries(${PROJECT_NAME} imm32 opengl32 gdi32)
endif()