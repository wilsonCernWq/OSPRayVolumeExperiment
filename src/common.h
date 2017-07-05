//!
//! This file contains headers of all external libraries
//!
#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#define NOMINMAX

//
// include ospray
//
#include "ospray/ospray.h"
#include "ospray/ospcommon/vec.h"
//#include "ospray/VisItModuleCommon.h"

//
// include cpp standard library
//
#include <iostream>
#include <string>
#include <cmath>
#include <ratio>
#include <limits>
#include <chrono> 
#include <vector>     // c++11
#include <algorithm>  // c++11
#include <functional> // c++11
#include <thread>
#include <omp.h>

#include <GL/glew.h>
#ifdef APPLE // apple specific header
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# if defined(WIN32) || defined(_WIN32) || defined(WIN32) && !defined(CYGWIN)
#  include <Windows.h>
# else
#  if unix // Linux needs extensions for framebuffers
#   define GL_GLEXT_PROTOTYPES 1
#   include <GL/glext.h>
#  endif
# endif
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
# include <GL/freeglut.h>
#endif

//
// include cyCodeBase here
//
#include <cyCore.h>
#include <cyTimer.h>
#include <cyPoint.h>
#include <cyTriMesh.h>
#include <cyMatrix.h>
#include <cyGL.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#endif//_COMMON_H_
