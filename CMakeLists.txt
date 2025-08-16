cmake_minimum_required(VERSION 3.20)

set(PROJECT_VERSION 0.3.0)

project(KeroMaster VERSION ${PROJECT_VERSION} LANGUAGES CXX)

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
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.5)

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

add_executable(${PROJECT_NAME} WIN32
  ${SOURCES}
)
target_include_directories(${PROJECT_NAME} PRIVATE
  src
)
target_link_libraries(${PROJECT_NAME} raylib imgui)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  -DKM_VERSION="${PROJECT_VERSION}"
  -DKM_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  -DKM_VERSION_MINOR=${PROJECT_VERSION_MINOR}
  -DKM_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

if(CMAKE_VERSION VERSION_GREATER 4.0)
  set_property(TARGET ${PROJECT_NAME} PROPERTY DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}") # CMake 4.0+
else()
  set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
  set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

if(APPLE)
  set(RESOURCE_ASSETS
    "${CMAKE_CURRENT_SOURCE_DIR}/res/macos/Assets.xcassets"
    # "${CMAKE_CURRENT_SOURCE_DIR}/res/macos/mac-icon.icns"
    "${CMAKE_CURRENT_SOURCE_DIR}/art/mac-icon.icon"
  )

  if(CMAKE_GENERATOR STREQUAL "Xcode")
    set(RESOURCE_ASSETS_OUTPUT ${RESOURCE_ASSETS})
  else()
    message(WARNING "Using non-Xcode generator is not recommended, expect problems with asset catalog generation...")
    set(RESOURCE_ASSETS_OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/assets-thinned/Assets.car
      # ${CMAKE_CURRENT_BINARY_DIR}/assets-thinned/mac-icon.icns
    )
    add_custom_target(${PROJECT_NAME}_AssetCatalog ALL
      COMMAND actool ${RESOURCE_ASSETS} 
        --compile ${CMAKE_CURRENT_BINARY_DIR}/assets-thinned 
        --export-dependency-info ${CMAKE_CURRENT_BINARY_DIR}/assetcatalog_dependencies_thinned
        --output-partial-info-plist ${CMAKE_CURRENT_BINARY_DIR}/assetcatalog_generated_info.plist_thinned
        --output-format human-readable-text 
        --notices 
        --warnings 
        --app-icon mac-icon 
        --enable-on-demand-resources NO 
        --development-region en 
        --target-device mac 
        --minimum-deployment-target ${CMAKE_OSX_DEPLOYMENT_TARGET} 
        --platform macosx
      BYPRODUCTS ${RESOURCE_ASSETS_OUTPUT}
    )
    add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_AssetCatalog)
  endif()
  target_sources(${PROJECT_NAME} PRIVATE ${RESOURCE_ASSETS_OUTPUT})
  set_source_files_properties(${RESOURCE_ASSETS_OUTPUT}
    TARGET_DIRECTORY ${PROJECT_NAME}
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources
  )
  set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/res/macos/Info.plist"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER   "io.github.gota7.keromaster"
    XCODE_ATTRIBUTE_PRODUCT_NAME                "KeroMaster"
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION     ${PROJECT_VERSION}
    XCODE_ATTRIBUTE_MARKETING_VERSION           ${PROJECT_VERSION}
  )
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