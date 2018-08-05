#
# General Settings
#
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_MACOSX_RPATH 1)
endif ()
include(cmake/OpenGLTargets.cmake)
#
#--- GLFW
#
if (NOT TARGET glfw)
  if (EXISTS ${PROJECT_SOURCE_DIR}/external/glfw/CMakeLists.txt)
    set(GLFW_BUILD_DOCS     OFF)
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS    OFF)
    set(GLFW_INSTALL        OFF)
    add_subdirectory(${PROJECT_SOURCE_DIR}/external/glfw)
    set_target_properties(glfw PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS USE_GLFW=1)
    target_include_directories(glfw INTERFACE
      "$<BUILD_INTERFACE:"
      "${PROJECT_SOURCE_DIR}/external/deps;"
      "${PROJECT_SOURCE_DIR}/external/glfw;"
      "${PROJECT_SOURCE_DIR}/external/glfw/include;"
      ">")
    if (NOT TARGET glad)
      add_library(glad
        ${PROJECT_SOURCE_DIR}/external/deps/glad.c)
      target_include_directories(glad PUBLIC
        "$<BUILD_INTERFACE:"
        "${PROJECT_SOURCE_DIR}/external/deps;"
        "${PROJECT_SOURCE_DIR}/external/glfw;"
        "${PROJECT_SOURCE_DIR}/external/glfw/include;"
        ">")
    endif ()
  else ()
    messate(FATAL_ERROR "cannot find glfw")
  endif ()
endif ()
#
#--- ImGUI
#
if (NOT TARGET imgui)
  if(EXISTS ${PROJECT_SOURCE_DIR}/external/imgui/CMakeLists.txt)
    add_subdirectory(${PROJECT_SOURCE_DIR}/external/imgui)
  else ()
    messate(FATAL_ERROR "cannot find imgui")
  endif ()
endif ()
#
#--- glm
#
if (NOT TARGET glm)
  if (EXISTS ${PROJECT_SOURCE_DIR}/external/glm/CMakeLists.txt)
    set(GLM_INSTALL_ENABLE OFF)
    add_subdirectory(${PROJECT_SOURCE_DIR}/external/glm)
  endif ()
endif ()
#
#--- OpenMP
#
if (NOT TARGET OpenMP)
  find_package(OpenMP)
  if (OPENMP_FOUND)    
    add_library(OpenMP INTERFACE IMPORTED)
    set_target_properties(OpenMP PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS USE_OMP=1)
    if (OpenMP_CXX_FLAGS)
      set_target_properties(OpenMP PROPERTIES 
        INTERFACE_COMPILE_FLAGS "${OpenMP_CXX_FLAGS}")
    endif ()
  endif ()
endif ()
