### IMGUI

ADD_LIBRARY(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_stdlib.cpp
)

TARGET_INCLUDE_DIRECTORIES(imgui PUBLIC imgui)
