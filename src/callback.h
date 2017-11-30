//!
//! This file defines some global variables and all callback functions
//!
#pragma once
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include "common.h"
#include "helper.h"
#include "global.h"

//! functions
inline void Clean()
{
    std::cout << "cleaning" << std::endl;
    if (framebuffer != nullptr) {
	gfb.Delete();
	ospUnmapFrameBuffer(ofb, framebuffer);       
	ospFreeFrameBuffer(framebuffer);
	framebuffer = nullptr;
    }
    if (world != nullptr) { 
	ospRelease(world); 
	world = nullptr;
    }
    if (camera != nullptr) { 
	ospRelease(camera); 
	camera = nullptr;
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

inline void UpdateCamera(bool cleanbuffer = true)
{
    camDir = camFocus - camPos;
    auto currCamUp  = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camUp,  0.0f));
    auto currCamDir = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camDir, 0.0f));
    auto currCamPos = (cyPoint3f)camFocus - currCamDir * camZoom;
    ospSetVec3f(camera, "pos", (osp::vec3f&)currCamPos);
    ospSetVec3f(camera, "dir", (osp::vec3f&)currCamDir);
    ospSetVec3f(camera, "up",  (osp::vec3f&)currCamUp);
    ospCommit(camera);
    if (cleanbuffer) {
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
}

inline void GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
    static cy::Point2f p;
    mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    camRotate.BeginDrag(p[0], p[1]);
}

inline void GetMousePosition(GLint x, GLint y) {
    static cy::Point2f p;
    mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    camRotate.Drag(p[0], p[1]);
    UpdateCamera();
}

inline void GetNormalKeys(unsigned char key, GLint x, GLint y) {
    if (key == 27) { 
	Clean();
	glutLeaveMainLoop(); 
    }
    if (key == 'p') { // print camera
	camDir = camFocus - camPos;
	auto currCamUp  = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camUp, 0.0f));
	auto currCamDir = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camDir, 0.0f));
	auto currCamPos = (cyPoint3f)camFocus - currCamDir * camZoom;
	std::cout << "camup  " 
		  << currCamUp.x << ", "
		  << currCamUp.y << ", "
		  << currCamUp.z << std::endl
		  << "camdir " 
		  << currCamDir.x << ", " 
		  << currCamDir.y << ", " 
		  << currCamDir.z << std::endl
		  << "campos " 
		  << currCamPos.x << ", "
		  << currCamPos.y << ", "
		  << currCamPos.z << std::endl;
    }
    if (key == 's') { // save screen shot	
	writePPM("screenshot.ppm", WINSIZE, ofb);
    }
}

inline void GetSpecialKeys(int key, GLint x, GLint y) {
    if (key == GLUT_KEY_UP) { 
	camZoom-=0.01f;
	UpdateCamera();
    }
    if (key == GLUT_KEY_DOWN) {
	camZoom+=0.01f;
	UpdateCamera();
    }
}

inline void Idle() { glutPostRedisplay(); }

void render()
{
    static int framecount = 0;
    static const int stepcount = 10;
    static std::chrono::system_clock::time_point 
	t1 = std::chrono::system_clock::now(), 
	t2 = std::chrono::system_clock::now();
    if (framecount % stepcount == 0) {
	t2 = std::chrono::system_clock::now();
	std::chrono::duration<double> dur = t2 - t1;
	if (framecount > 0) {
	    glutSetWindowTitle
		(std::to_string((double)stepcount / dur.count()).c_str());
	}
	t1 = std::chrono::system_clock::now();	
    }
    ++framecount;
    if (framebuffer != nullptr) {
	ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
	gfb.BindTexture();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINSIZE.x, WINSIZE.y, 
			GL_RGBA, GL_UNSIGNED_BYTE, ofb);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gfb.GetID());
	glBlitFramebuffer(0, 0, WINSIZE.x, WINSIZE.y, 
			  0, 0, WINSIZE.x, WINSIZE.y,
			  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glutSwapBuffers();
    }
}

#endif//_CALLBACK_H_
