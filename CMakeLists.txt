cmake_minimum_required(VERSION 2.8)
project(OSPRayTinyLoader)
#
# include host specification file
cmake_host_system_information(RESULT SYSNAME QUERY HOSTNAME)
message(STATUS "trying to include ${SYSNAME}.cmake configuration file")
include(cmake/config-site/${SYSNAME}.cmake OPTIONAL RESULT_VARIABLE CONFIGFOUND)
if (NOT CONFIGFOUND)
  message(STATUS "failed to load the configuration file")
  message(STATUS "helper message:
    To build the program sucessfully, you might need to provide following paths to your cmake
    *) path to your osprayConfig.cmake included in the OSPRay releases to CMake as '-Dospray_DIR=<..>'
    *) path to the embreeConfig.cmake from the Embree releases similarly as '-Dembree_DIR=<..>'
    *) path to the root directory of your standalone TBB library as '-DTBB_ROOT=<..>'
    *) path to the root directory of your standalone ISPC executable as '-DISPC_EXECUTABLE=<..>'
  ")
endif()
#
# load required libraries
include(cmake/common.cmake)
find_package(ospray REQUIRED)
#
# install dlls or shader
file(GLOB_RECURSE SHADERS "src/*.glsl")
set(COPYLIST ${COMMON_DLLS} ${SHADERS})
if(EXECUTABLE_OUTPUT_PATH)
  DeployRepo(${COPYLIST} ${EXECUTABLE_OUTPUT_PATH})
else()
  DeployRepo("${COPYLIST}" "${CMAKE_BINARY_DIR}")
endif()
#
# include sources
file(GLOB_RECURSE CXX "src/*.cpp")
file(GLOB_RECURSE HXX "src/*.h")
include_directories(${OSPRAY_INCLUDE_DIRS})
add_executable(osp_tiny_loader ${CXX} ${HXX} ${SHADERS})
target_link_libraries(osp_tiny_loader ${COMMON_LIBS} ${OSPRAY_LIBRARIES})
