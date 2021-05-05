### RAYLIB

#ADD_LIBRARY(raylib STATIC
#    raylib/src/core.c
#    raylib/src/shapes.c
#    raylib/src/textures.c
#    raylib/src/text.c
#    raylib/src/utils.c
#    raylib/src/rglfw.c
#    raylib/src/models.c
#    raylib/src/raudio.c
#)

#TARGET_INCLUDE_DIRECTORIES(raylib
#    PUBLIC
#        raylib/src
#    PRIVATE
#        raylib/src/external/glfw/include
#        raylib/src/external/glfw/deps/mingw
#)

find_package(raylib 3.5.0 QUIET)

if (NOT raylib_FOUND)
  include(FetchContent)

  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/master.tar.gz
  )

  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SUPPORT_FILEFORMAT_GLTF ON CACHE BOOL "" FORCE)

    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})

  endif()

endif()

#TARGET_COMPILE_DEFINITIONS(raylib PRIVATE PLATFORM_DESKTOP GRAPHICS_API_OPENGL_33)

#TARGET_COMPILE_OPTIONS(raylib PRIVATE -std=c99 -fno-strict-aliasing)

# Use winmm instead of X11, pthread, and dl if Windows!
#TARGET_LINK_LIBRARIES(raylib PUBLIC X11 pthread dl)

### GLAD
find_package(glad 0.1.34 QUIET)
if (NOT glad_FOUND)
  include(FetchContent)

  FetchContent_Declare(
    glad
    URL https://github.com/Dav1dde/glad/archive/master.tar.gz
  )

  FetchContent_GetProperties(glad)
  if (NOT glad_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(glad)
    add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})

  endif()

endif()

### IMGUI

ADD_LIBRARY(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    #imgui/examples/imgui_impl_opengl3.cpp
    #imgui_impl_raylib.cpp
)

TARGET_INCLUDE_DIRECTORIES(imgui
    PUBLIC imgui imgui/examples
    PRIVATE raylib/src/external raylib/src)

TARGET_COMPILE_DEFINITIONS(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
TARGET_LINK_LIBRARIES(imgui glad)
