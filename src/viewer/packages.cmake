#
#--- OPENGL
#
if (NOT TARGET OpenGL)
  find_package(OpenGL REQUIRED)
  add_library(OpenGL INTERFACE IMPORTED)
  set_target_properties(OpenGL PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${OPENGL_LIBRARIES}")
endif ()
#
#--- GLFW
#
if (NOT TARGET glfw)
  messate(FATAL_ERROR "cannot find glfw")
endif ()
#
#--- ImGUI
#
if (NOT TARGET imgui)
  messate(FATAL_ERROR "cannot find imgui")
endif ()
