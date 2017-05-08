/**
 * This is a common header for every file. It includes all external libraries and defines all basic types
 */
#pragma once
#ifndef _OTV_COMMON_H_
#define _OTV_COMMON_H_

#define NOMINMAX

//
// include ospray
//
#include "ospray/ospray.h"
#include "ospray/ospcommon/vec.h"

//
// include cpp standard library
//
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>

#include <GL/glew.h>
#ifdef APPLE // apple specific header
#   include <OpenGL/gl.h>
#	include <OpenGL/glu.h>
#	include <GLUT/glut.h>
#else
#	if defined(WIN32) || defined(_WIN32) || defined(WIN32) && !defined(CYGWIN) //	Windows need special header
#		include <Windows.h>
#	else
#		if unix	// Linux needs extensions for framebuffers
#			define GL_GLEXT_PROTOTYPES 1
#			include <GL/glext.h>
#		endif
#	endif
#	include <GL/gl.h>
#	include <GL/glu.h>
#	include <GL/glut.h>
#	include <GL/freeglut.h>
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

//
// trying this obj loader https://github.com/syoyo/tinyobjloader
//
// #include "tiny_obj_loader.h"

//
// include image loader
//
// #include "lodepng.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#endif//_OTV_COMMON_H_
