//!
//! This file defines some global variables and all callback functions
//!
#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "common.h"
#include "camera.h"
#include "framebuffer.h"

//! camera
Camera camera;

//! framebuffers
Framebuffer framebuffer;

//! ospray objects
OSPModel world = nullptr;
OSPRenderer renderer = nullptr;
OSPTransferFunction transferFcn = nullptr;

//! cleaning
std::vector<std::function<void()>> cleanlist;

#endif//_GLOBAL_H_
