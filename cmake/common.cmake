#
#--- Libraries Setups
#
# Author: Qi WU, University of Utah
#
# Modified based on the course project from EPFL ICG course that I have taken
# This is used for configure the environment with CMAKE
#
# Build configuration file for "Intro to Graphics"
# Copyright (C) 2014 - LGG EPFL
#
#--- To understand its content:
#   http://www.cmake.org/cmake/help/syntax.html
#   http://www.cmake.org/Wiki/CMake_Useful_Variables
#
MESSAGE(STATUS "OSPRay Volume Renderer")
#
# define macro
#
MACRO(DeployRepo SRC DEST)
  MESSAGE(STATUS "-- Deploying: ${SRC} to ${DEST}")
  FOREACH(f ${SRC})
    FILE(COPY ${f} DESTINATION ${DEST})
  ENDFOREACH()
ENDMACRO(DeployRepo)
#
# General Settings
#
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  SET(CMAKE_MACOSX_RPATH 1)
ENDIF()
#
# Initialize library pathes and dll path
#
SET(OPENGL_LIBS "") # those will be link for each project
SET(COMMON_LIBS "") # those will be link for each project
SET(COMMON_DLLS "") # those files will be copyed to the executable folder
#
#--- OPENGL
#
FIND_PACKAGE(OpenGL REQUIRED)
IF(OPENGL_FOUND)
  MESSAGE(STATUS " OPENGL found!  ${OPENGL_LIBRARIES}")
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  LIST(APPEND COMMON_LIBS ${OPENGL_LIBRARIES})
  IF(OPENGL_GLU_FOUND)
    MESSAGE(STATUS " GLU found!")
  ELSE()
    MESSAGE(FATAL_ERROR " GLU not found!")
  ENDIF()
ELSE()
  MESSAGE(FATAL_ERROR " OPENGL not found!")
ENDIF()
#
#--- GLFW
#
IF(EXISTS ${PROJECT_SOURCE_DIR}/external/glfw)
  SET(GLFW_BUILD_DOCS     OFF)
  SET(GLFW_BUILD_EXAMPLES OFF)
  SET(GLFW_BUILD_TESTS    OFF)
  SET(GLFW_BUILD_INSTALL  OFF)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/external/glfw/include)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/external/glfw/deps)
  ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/external/glfw)
  ADD_LIBRARY(glfw_glad
    ${GLFW_SOURCE_DIR}/deps/glad/glad.h
    ${GLFW_SOURCE_DIR}/deps/glad.c)
  LIST(APPEND COMMON_LIBS glfw glfw_glad)
  LIST(APPEND OPENGL_LIBS glfw glfw_glad)
  ADD_DEFINITIONS(-DUSE_GLFW)
ELSE()
  MESSATE(FATAL_ERROR "cannot find glfw")
ENDIF()
#
#----------------------------------------------------------------------------
#
#--- ImGUI
#
IF(EXISTS ${PROJECT_SOURCE_DIR}/external/imgui)
  ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/external/imgui)
  INCLUDE_DIRECTORIES(${ImGUI_INCLUDE_DIR})
  LIST(APPEND COMMON_LIBS ${ImGUI_LIBRARIES})
  ADD_DEFINITIONS(-DUSE_IMGUI)
ENDIF()
#
#--- glm
#
IF(EXISTS ${PROJECT_SOURCE_DIR}/external/glm)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/external/glm)
  ADD_DEFINITIONS(-DUSE_GLM)
ENDIF()
#
#----------------------------------------------------------------------------
#
#--- OpenMP
#
FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  ADD_DEFINITIONS(-DUSE_OMP)
ENDIF()
