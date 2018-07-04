cmake_minimum_required(VERSION 2.8)
project(OSPRayVolumeExperiment)
#
# options
#
option(USE_OSPRAY "Use OSPRay Rendering Library to render a volume" ON)
#
# include host specification file
#
cmake_host_system_information(RESULT SYSNAME QUERY HOSTNAME)
message(STATUS "trying to include ${SYSNAME}.cmake configuration file")
include(cmake/config-site/${SYSNAME}.cmake OPTIONAL RESULT_VARIABLE CONFIGFOUND)
if (NOT CONFIGFOUND)
  message(STATUS "failed to load the configuration file")
  message(STATUS "helper message:
    To build the program sucessfully, you might need to 
    provide following paths to your cmake
    *) path to your osprayConfig.cmake included in the 
       OSPRay releases to CMake as '-Dospray_DIR=<..>'
    *) path to the embreeConfig.cmake from the Embree 
       releases similarly as '-Dembree_DIR=<..>'
    *) path to the root directory of your standalone TBB
        library as '-DTBB_ROOT=<..>'
    *) path to the root directory of your standalone ISPC
        executable as '-DISPC_EXECUTABLE=<..>'
  ")
endif ()
#
# load required libraries
#
include(cmake/common.cmake)
#
# load OSPRay
#
if(USE_OSPRAY)
  find_package(ospray REQUIRED)
else (USE_OSPRAY)
  find_package(ospray QUIET)
endif (USE_OSPRAY)
if (ospray_FOUND)
  if (NOT TARGET ospray)
    set(_skip_next_ OFF)
    set(_ospray_libs_ "")
    foreach (l ${OSPRAY_LIBRARIES})
      if ("${l}" STREQUAL "optimized")
        set(_skip_next_ ON)
      else ()
        if (_skip_next_)
          set(_skip_next_ OFF)
        else ()
          if (NOT "${l}" STREQUAL "debug")
            list(APPEND _ospray_libs_ ${l})
            message(STATUS ${l})
          endif ()
        endif ()
      endif ()
    endforeach()
    add_library(ospray INTERFACE IMPORTED)
    set_target_properties(ospray PROPERTIES
      INTERFACE_COMPILE_DEFINITIONS USE_OSP=1
      INTERFACE_INCLUDE_DIRECTORIES "${OSPRAY_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${_ospray_libs_}")
  endif ()
endif ()
#
# TFN module
#
if(TFN_MODULE_ROOT)
  include_directories(${TFN_MODULE_ROOT}/src)
  set(TFN_MODULE_SRC
    ${TFN_MODULE_ROOT}/src/TransferFunctionModule.cpp
    ${TFN_MODULE_ROOT}/src/widgets/TransferFunctionWidget.cpp)
  add_definitions(-DUSE_TFN_MODULE)
  mark_as_advanced(TFN_MODULE_SRC)
endif(TFN_MODULE_ROOT)
#
# make application
#
add_executable(renderer
  src/main.cpp
  ${TFN_MODULE_SRC})
target_link_libraries(renderer
  ospray glm imgui OpenGL glad glfw)
set_target_properties(renderer PROPERTIES
  CXX_STANDARD 11)
