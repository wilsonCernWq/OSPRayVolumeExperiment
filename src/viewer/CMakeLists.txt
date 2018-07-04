# find packages
include(packages.cmake)

# transfer function widget
set(TFN_MODULE_DEFAULT_ROOT 
  ${PROJECT_SOURCE_DIR}/apps/exampleViewer/widgets/TransferFunctionModule) 
if(TFN_MODULE_ROOT)
  set(TFN_MODULE 1)
elseif (EXISTS ${TFN_MODULE_DEFAULT_ROOT})
  set(TFN_MODULE_ROOT ${TFN_MODULE_DEFAULT_ROOT})
  set(TFN_MODULE 1)
else ()
  set(TFN_MODULE 0)
endif ()
if (TFN_MODULE)
  set(TFN_MODULE_INCLUDE_DIR ${TFN_MODULE_ROOT}/src)
  set(TFN_MODULE_SRC 
    ${TFN_MODULE_ROOT}/src/TransferFunctionModule.cpp
    ${TFN_MODULE_ROOT}/src/common/jsoncpp.cpp
    ${TFN_MODULE_ROOT}/src/widgets/TransferFunctionWidget.cpp)
endif (TFN_MODULE)

# build everything as a library
add_library(ospray${OSPRAY_APP_VIEWER_PREFIX}_app_viewer
  viewer.cpp
  src/engine.cpp
  src/camera.cpp
  src/scene/properties.cpp
  src/common/trackball.cpp
  ${TFN_MODULE_SRC}
  external/glad.c
  external/imgui_glfw_impi.cpp)
target_include_directories(glad PUBLIC
  "$<BUILD_INTERFACE:"
  "${TFN_MODULE_INCLUDE_DIR};"
  "${CMAKE_CURRENT_LIST_DIR}/src;"
  "${CMAKE_CURRENT_LIST_DIR}/external;"
  ">")
target_link_libraries(ospray${OSPRAY_APP_VIEWER_PREFIX}_app_viewer
  ospray imgui glfw OpenGL)
set_target_properties(ospray${OSPRAY_APP_VIEWER_PREFIX}_app_viewer
  PROPERTIES CXX_STANDARD 14)
