cmake_minimum_required(VERSION 3.15)

add_library(imgui)

target_sources(imgui
PRIVATE
     "../../modules/imgui/imgui.cpp"
     "../../modules/imgui/imgui_demo.cpp"
     "../../modules/imgui/imgui_draw.cpp"
     "../../modules/imgui/imgui_tables.cpp"
     "../../modules/imgui/imgui_widgets.cpp"
     "../../modules/imgui/backends/imgui_impl_opengl3.cpp"
     "../../modules/imgui/backends/imgui_impl_glfw.cpp"
)

target_include_directories(imgui 
PUBLIC
    "../../modules/imgui/"
    "../../modules/imgui/backends"
)

target_link_libraries(imgui
PRIVATE
    glfw
)