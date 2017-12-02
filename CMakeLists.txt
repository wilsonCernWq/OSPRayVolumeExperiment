cmake_minimum_required(VERSION 2.8)
project(OSPRayVolumeExperiment)
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
#
# load OSPRay
FIND_PACKAGE(ospray REQUIRED)
INCLUDE_DIRECTORIES(${OSPRAY_INCLUDE_DIRS})
LIST(APPEND COMMON_LIBS ${OSPRAY_LIBRARIES})
MESSAGE(STATUS "OSPRAY_LIBRARIES ${OSPRAY_LIBRARIES}")
#
# include sources
file(GLOB_RECURSE CXX "src/*.cpp")
file(GLOB_RECURSE HXX "src/*.h")
add_executable(VolumeDisplay ${CXX} ${HXX})
target_link_libraries(VolumeDisplay ${COMMON_LIBS})
set_target_properties(VolumeDisplay
  PROPERTIES
  CXX_STANDARD 11)
