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

inline void SetupTF(const std::vector<vec3f>& colors, const std::vector<float>& opacities)
{
    //! setup trasnfer function
    transferFcn = ospNewTransferFunction("piecewise_linear");
    OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
    ospCommit(colorsData);
    OSPData opacityData = ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
    ospCommit(opacityData);
    const vec2f valueRange(static_cast<float>(0), static_cast<float>(255));
    ospSetData(transferFcn, "colors", colorsData);
    ospSetData(transferFcn, "opacities", opacityData);
    ospSetVec2f(transferFcn, "valueRange", (osp::vec2f&)valueRange);
    ospCommit(transferFcn);
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

#endif//_CALLBACK_H_
