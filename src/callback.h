//!
//! This file defines some global variables and all callback functions
//!
#pragma once
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include "common.h"
#include "global.h"

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

//! functions
inline void Clean()
{
  std::cout << "cleaning" << std::endl;
  camera.Clean();
  framebuffer.Clean();
  if (world != nullptr) { 
    ospRelease(world); 
    world = nullptr;
  }
  if (renderer != nullptr) {
    ospRelease(renderer);
    renderer = nullptr;
  }
  if (transferFcn != nullptr) {
    ospRelease(transferFcn);
    transferFcn = nullptr;
  }
  std::cout << "cleaning other stuffs" << std::endl;
  for (auto& c : cleanlist) { c(); }
}

inline void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

inline void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  { glfwSetWindowShouldClose(window, GLFW_TRUE); }
}

inline void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  //if (!CapturedByGUI())
  {
    int left_state  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int right_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (left_state == GLFW_PRESS) { camera.CameraDrag(xpos, ypos); }
    else { camera.CameraBeginDrag(xpos, ypos); framebuffer.CleanBuffer(); }
    if (right_state == GLFW_PRESS) { camera.CameraZoom(xpos, ypos); }
    else { camera.CameraBeginZoom(xpos, ypos); framebuffer.CleanBuffer(); }
  }
}

inline void window_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  camera.CameraUpdateProj(width, height);
  framebuffer.Resize(width, height);
}

inline void render()
{  
  ospRenderFrame(framebuffer.OSPRayPtr(), renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
  framebuffer.Upload();
}

inline GLFWwindow* InitWindow()
{
  // Initialize GLFW
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) { exit(EXIT_FAILURE); }
  // Provide Window Hints
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Create Window
  GLFWwindow* window = glfwCreateWindow(camera.CameraWidth(), 
					camera.CameraHeight(),
					"OSPRay Volume Test Renderer", 
					NULL, NULL);
  if (!window) { glfwTerminate(); exit(EXIT_FAILURE); }
  // Callback
  glfwSetKeyCallback(window, key_callback);
  glfwSetWindowSizeCallback(window, window_size_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  // Ready
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
  check_error_gl("Ready");
  // Setup OpenGL
  glEnable(GL_DEPTH_TEST);
  check_error_gl("Setup OpenGL Options");
  // GUI
  {
    // Initialize GUI
    ImGui_ImplGlfwGL3_Init(window, false);
  }
  return window;
}

inline void ShutdownWindow(GLFWwindow* window)
{
  // GUI
  {
    // Shutup GUI
    ImGui_ImplGlfwGL3_Shutdown();
  }
  // Shutup window
  glfwDestroyWindow(window);
}

#endif//_CALLBACK_H_
