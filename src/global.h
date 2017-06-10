//!
//! This file defines some global variables and all callback functions
//!
#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "common.h"
#include "trackball.h"

using namespace ospcommon;

//! window size
unsigned int WINX = 0, WINY = 0;
const vec2i WINSIZE = vec2i(512, 512);

//! framebuffers
uint32_t*           ofb;
cyGLRenderTexture2D gfb;

//! ospray objects
OSPModel world = nullptr;
OSPCamera camera = nullptr;
OSPRenderer renderer = nullptr;
OSPFrameBuffer framebuffer = nullptr;
OSPTransferFunction transferFcn = nullptr;

//! camera objects
float camZoom = 1.0f;
ospcommon::vec3f camFocus = vec3f(0, 0, 0);
ospcommon::vec3f camPos = vec3f(0.00175993, -2.58563, 25.0471);
ospcommon::vec3f camUp = vec3f(0.41229, 0.100841, 1.67924);
ospcommon::vec3f camDir = camFocus - camPos;
Trackball camRotate(true);

//! cleaning
std::vector<std::function<void()>> cleanlist;

#endif//_GLOBAL_H_
