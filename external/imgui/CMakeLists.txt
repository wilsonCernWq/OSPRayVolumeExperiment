#
# build ImGui
#
if (NOT TARGET imgui)
  add_library(imgui
    upstream/imgui.cpp
    upstream/imgui_draw.cpp
    upstream/imgui_demo.cpp
    opengl3_example/imgui_impl_glfw_gl3.cpp)
  target_link_libraries(imgui glad glfw OpenGL)
  target_include_directories(imgui PUBLIC
    "$<BUILD_INTERFACE:"
    "${CMAKE_CURRENT_LIST_DIR}/upstream;"
    "${CMAKE_CURRENT_LIST_DIR}/opengl3_example;"
    ">")
  set_target_properties(imgui PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS USE_IMGUI=1)
endif ()