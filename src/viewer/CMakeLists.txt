# define package name
if (NOT APP_VIEWER_PREFIX)
  set(APP_VIEWER_PREFIX ospray)
endif ()

# find packages
include(packages.cmake)

# transfer function widget
find_path(TFN_MODULE_INCLUDE_DIR TransferFunctionModule.h
  HINTS
  ${PROJECT_SOURCE_DIR}/apps/exampleViewer/widgets/TransferFunctionModule/src
  ${TFN_MODULE_ROOT}/src
  NO_DEFAULT_PATH)
if (TFN_MODULE_INCLUDE_DIR)
  set(TFN_MODULE_SRC 
    ${TFN_MODULE_INCLUDE_DIR}/TransferFunctionModule.h
    ${TFN_MODULE_INCLUDE_DIR}/TransferFunctionModule.cpp
    ${TFN_MODULE_INCLUDE_DIR}/widgets/HelperFunctions.h
    ${TFN_MODULE_INCLUDE_DIR}/widgets/DefaultTransferFunctionMaps.h
    ${TFN_MODULE_INCLUDE_DIR}/widgets/TransferFunctionWidget.h
    ${TFN_MODULE_INCLUDE_DIR}/widgets/TransferFunctionWidget.cpp)
else  (TFN_MODULE_INCLUDE_DIR)
  message(FATAL_ERROR "Cannot find Transfer Function Module.")
endif (TFN_MODULE_INCLUDE_DIR)

# build everything as a library
add_library(${APP_VIEWER_PREFIX}_app_viewer
  ${TFN_MODULE_SRC}
  external/imgui_glfw_impi.h
  external/imgui_glfw_impi.cpp
  src/common/constants.h
  src/common/navsphere.h
  src/common/trackball.h
  src/common/trackball.cpp
  src/scene/properties.h
  src/scene/properties.cpp
  src/camera.h
  src/camera.cpp
  src/engine.h
  src/engine.cpp
  viewer.h
  viewer.cpp)
set_target_properties(${APP_VIEWER_PREFIX}_app_viewer
  PROPERTIES CXX_STANDARD 11)
target_include_directories(${APP_VIEWER_PREFIX}_app_viewer
  PRIVATE
  "$<BUILD_INTERFACE:"
  "${TFN_MODULE_INCLUDE_DIR};"
  "${CMAKE_CURRENT_LIST_DIR}/src;"
  "${CMAKE_CURRENT_LIST_DIR}/external;"
  ">")
target_link_libraries(${APP_VIEWER_PREFIX}_app_viewer glad glfw OpenGL)
if (TARGET ospray::ospray) # if the viewer is built outside ospray source
  target_link_libraries(${APP_VIEWER_PREFIX}_app_viewer imgui glfw glad OpenGL::GLU OpenGL::GL
    ospray::ospray
    ospray::ospray_common)
else () # if the package is built within an ospray module
  target_include_directories(${APP_VIEWER_PREFIX}_app_viewer
    PUBLIC "$<BUILD_INTERFACE:"
    "${PROJECT_SOURCE_DIR}/apps/exampleViewer/common/imgui;>")
  target_link_libraries(${APP_VIEWER_PREFIX}_app_viewer
    ospray
    ospray_common
    ospray_imgui
    ospray_imgui3d_sg)
endif ()
