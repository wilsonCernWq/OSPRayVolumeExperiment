#
#--- Libraries Setups
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
#--- Interface
#   This module wil define two global variables
#--- This is how you show a status message in the build system
MESSAGE(STATUS "Interactive Computer Graphics - Loading Common Configuration")
#
# define macro
MACRO(DeployRepo SRC DEST)
  message(STATUS " -- Deploying: ${SRC} to ${DEST}")
  FOREACH(f ${SRC})
    FILE(COPY ${f} DESTINATION ${DEST})
  ENDFOREACH()
ENDMACRO(DeployRepo)
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
#
# Setting up compiler
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/configcxx.cmake)
#
# Initialize library pathes and dll path
SET(COMMON_LIBS "") # those will be link for each project
SET(COMMON_DLLS "") # those files will be copyed to the executable folder
SET(COMMON_DEFINITIONS "")
#
# check win arch
SET(WIN_ARCH "")
IF(WIN32)
  IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(WIN_ARCH /x64)
  ENDIF()
ENDIF()
#
#--- OPENGL
FIND_PACKAGE(OpenGL REQUIRED)
IF(OPENGL_FOUND)
  MESSAGE(STATUS " OPENGL found!  ${OPENGL_LIBRARIES}")
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  LIST(APPEND COMMON_LIBS ${OPENGL_LIBRARIES})
  IF(OPENGL_GLU_FOUND)
    MESSAGE(STATUS " GLU found!")
  ELSE()
    MESSAGE(ERROR " GLU not found!")
  ENDIF()
ELSE()
  MESSAGE(ERROR " OPENGL not found!")
ENDIF()
#
#--- CMake extension to load GLUT
SET(GLUT_ROOT_PATH "${PROJECT_SOURCE_DIR}/external/freeglut")
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glut.cmake)
IF(GLUT_FOUND)
    INCLUDE_DIRECTORIES(library ${GLUT_INCLUDE_DIR})
    LIST(APPEND COMMON_LIBS ${GLUT_LIBRARIES})
    LIST(APPEND COMMON_DLLS ${GLUT_DLL})
ELSE()
    MESSAGE(FATAL_ERROR " GLUT not found!")
ENDIF()
#
#--- GLEW
SET(GLEW_ROOT_PATH "${PROJECT_SOURCE_DIR}/external/glew")
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/glew.cmake)
IF(GLUT_FOUND)
    INCLUDE_DIRECTORIES(library ${GLEW_INCLUDE_DIR})
    LIST(APPEND COMMON_LIBS ${GLEW_LIBRARIES})
    LIST(APPEND COMMON_DLLS ${GLEW_DLL})
ELSE()
    MESSAGE(FATAL_ERROR " GLEW not found!")
ENDIF()
#
#--- add cyCodeBase (submodule)
#   https://github.com/cemyuksel/cyCodeBase.git
INCLUDE_DIRECTORIES(library ${PROJECT_SOURCE_DIR}/external/cyCodeBase)
#
#--- add OSPRay
FIND_PACKAGE(ospray REQUIRED)
INCLUDE_DIRECTORIES(${OSPRAY_INCLUDE_DIRS})
LIST(APPEND COMMON_LIBS ${OSPRAY_LIBRARIES})
MESSAGE(STATUS "OSPRAY_LIBRARIES ${OSPRAY_LIBRARIES}")
