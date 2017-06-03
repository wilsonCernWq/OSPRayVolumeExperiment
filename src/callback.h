//!
//! This file defines some global variables and all callback functions
//!
#pragma once
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

using namespace ospcommon;

//! window size
unsigned int WINX = 0, WINY = 0;
const vec2i WINSIZE = vec2i(512, 512);

//! texture maps
uint32_t*           ofb;
cyGLRenderTexture2D gfb;

//! OSPRay objects
OSPModel world;
OSPCamera camera;
OSPRenderer renderer;
OSPFrameBuffer framebuffer;

//! camera objects
float camZoom = 1.0f;
vec3f camFocus = vec3f(0, 0, 0);
vec3f camPos = vec3f(0.00175993, -2.58563, 25.0471);
vec3f camUp = vec3f(0.41229, 0.100841, 1.67924);
vec3f camDir = camFocus - camPos;
Trackball camRotate(true);

//! functions
inline void UpdateCamera(bool cleanbuffer = true)
{
    camDir = camFocus - camPos;
    auto currCamUp = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camUp, 0.0f));
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
    if (key == 27) { glutLeaveMainLoop(); }
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

inline void clean()
{
    gfb.Delete();
    ospUnmapFrameBuffer(ofb, framebuffer);
    ospRelease(world);
    ospRelease(camera);
    ospRelease(renderer);
    ospRelease(framebuffer);
}

#endif//_CALLBACK_H_
