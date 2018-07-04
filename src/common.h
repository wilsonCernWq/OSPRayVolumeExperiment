//!
//! This file contains headers of all external libraries
//!
#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#define NOMINMAX
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

//
// include ospray
//
#ifdef USE_OSP
# include "ospray/ospray.h"
# include "ospray/ospcommon/vec.h"
#else
# error "Hmmm where is ospray?"
#endif

//
// OpenMP
//
#ifdef USE_OMP
# include <omp.h>
#endif

//
// GLFW
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

//
// GLM
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

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

//! @name writePPM Helper function to write the rendered image as PPM file
inline void writePPM
(const char *fileName, const osp::vec2i &size, const uint32_t *pixel) 
{
  using namespace ospcommon;
  FILE *file = fopen(fileName, "wb");
  fprintf(file, "P6\n%i %i\n255\n", size.x, size.y);
  unsigned char *out = (unsigned char *)alloca(3 * size.x);
  for (int y = 0; y < size.y; y++) {
    const unsigned char *in = 
      (const unsigned char *)&pixel[(size.y - 1 - y)*size.x];
    for (int x = 0; x < size.x; x++) {
      out[3 * x + 0] = in[4 * x + 0];
      out[3 * x + 1] = in[4 * x + 1];
      out[3 * x + 2] = in[4 * x + 2];
    }
    fwrite(out, 3 * size.x, sizeof(char), file);
  }
  fprintf(file, "\n");
  fclose(file);
}

#endif//_COMMON_H_
