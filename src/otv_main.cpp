#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc

#include "otv_common.h"
#include "otv_helper.h"
#include "otv_trackball.h"
#include <chrono>
#include <ratio>
#include <thread>

// - turn color of the bounding box
// - turn color of the of the first hit

using namespace ospcommon;

unsigned int WINX = 0, WINY = 0;
const vec2i WINSIZE(512, 512);

//! texture maps
uint32_t*           fb_osp;
cyGLRenderTexture2D fb_gl;

//! OSPRay objects
OSPModel world;
OSPCamera camera;
OSPRenderer renderer;
OSPFrameBuffer framebuffer;

//! camera objects // check point 260 275 
vec3f camFocus = vec3f(0, 0, 128);
vec3f camPos(598.8784, 537.4496, 962.184);
vec3f camUp(-0.263984, 0.889771, -0.372316);
vec3f camDir = camFocus - camPos;
otv::Trackball camRotate(true);

void UpdateCamera(bool cleanbuffer = true)
{
    camDir = camFocus - camPos;
    auto currCamUp = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camUp, 0.0f));
    auto currCamDir = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camDir, 0.0f));
    auto currCamPos = (cyPoint3f)camFocus - currCamDir;
    ospSetVec3f(camera, "pos", (osp::vec3f&)currCamPos);
    ospSetVec3f(camera, "dir", (osp::vec3f&)currCamDir);
    ospSetVec3f(camera, "up", (osp::vec3f&)currCamUp);
    ospCommit(camera);
    if (cleanbuffer) {
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
}

void GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
    static cy::Point2f p;
    otv::helper::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    camRotate.BeginDrag(p[0], p[1]);
}

void GetMousePosition(GLint x, GLint y) {
    static cy::Point2f p;
    otv::helper::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    camRotate.Drag(p[0], p[1]);
    UpdateCamera();
}

void GetNormalKeys(unsigned char key, GLint x, GLint y) {
    if (key == 27) { glutLeaveMainLoop(); }
    if (key == 'p') {
	// print camera
	camDir = camFocus - camPos;
	auto currCamUp  = cyPoint3f(camRotate.Matrix() 
				    * cyPoint4f((cyPoint3f)camUp, 0.0f));
	auto currCamDir = cyPoint3f(camRotate.Matrix()
				    * cyPoint4f((cyPoint3f)camDir, 0.0f));
	auto currCamPos = (cyPoint3f)camFocus - currCamDir;
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
    if (key == 's') {
	// save screen shot
	otv::helper::writePPM("screenshot.ppm", WINSIZE, fb_osp);
    }
}

void Idle() { glutPostRedisplay(); }

void clean()
{
    fb_gl.Delete();
    ospUnmapFrameBuffer(fb_osp, framebuffer);
    ospRelease(world);
    ospRelease(camera);
    ospRelease(renderer);
    ospRelease(framebuffer);
}

void render()
{
    ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
    fb_gl.BindTexture();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINSIZE.x, WINSIZE.y, GL_RGBA, GL_UNSIGNED_BYTE, fb_osp);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_gl.GetID());
    glBlitFramebuffer(0, 0, WINSIZE.x, WINSIZE.y, 0, 0, WINSIZE.x, WINSIZE.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glutSwapBuffers();
}

int main(int argc, const char **argv)
{
    ospInit(&argc, argv);    
    
    //! create world and renderer
    world = ospNewModel();
    renderer = ospNewRenderer("scivis"); // possible options: "pathtracer" "raytracer"

    OSPTransferFunction transferFcn = ospNewTransferFunction("piecewise_linear");
    const std::vector<vec3f> colors = {
	vec3f(0, 0, 0.563),vec3f(0, 0, 1),vec3f(0, 1, 1),vec3f(0.5, 1, 0.5),vec3f(1, 1, 0),vec3f(1, 0, 0),vec3f(0.5, 0, 0)
    };
    const std::vector<float> opacities = { 0.01f, 0.01f, 0.01f, 1.f, 1.f, 1.f };
    OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
    ospCommit(colorsData);
    OSPData opacityData = ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
    ospCommit(opacityData);
    const vec2f valueRange(static_cast<float>(0), static_cast<float>(255));
    ospSetData(transferFcn, "colors", colorsData);
    ospSetData(transferFcn, "opacities", opacityData);
    ospSetVec2f(transferFcn, "valueRange", (osp::vec2f&)valueRange);
    ospCommit(transferFcn);

    // now I am going to create two volumes and see if there are boundary defects
    // int volumeScale = 256;
    // std::vector<unsigned char> volumeData(volumeScale * volumeScale * volumeScale, 0);
    // for (size_t i = 0; i < volumeData.size(); ++i) { volumeData[i] = i % volumeScale; }
    // // volume #1
    // {
    // 	vec3i volumeDims(256);
    // 	vec3f volumeStart(-2.0f, -4.0f, -2.0f);
    // 	vec3f volumeStop(2.0f, 0.0f, 2.0f);
    // 	vec3f volumeSpac = (volumeStop - volumeStart) / (vec3f)(volumeDims - 1);
    // 	OSPVolume volume = ospNewVolume("block_bricked_volume");
    // 	ospSetString(volume, "voxelType", "uchar");
    // 	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    // 	ospSetVec3f(volume, "gridOrigin", (osp::vec3f&)volumeStart);
    // 	ospSetVec3f(volume, "gridSpacing", (osp::vec3f&)volumeSpac);
    // 	ospSet1i(volume, "singleShade", 0);
    // 	ospSetObject(volume, "transferFunction", transferFcn);
    // 	ospSetRegion(volume, volumeData.data(), osp::vec3i{ 0, 0, 0 }, (osp::vec3i&)volumeDims);
    // 	ospCommit(volume);
    // 	ospAddVolume(world, volume);
    // }
    // // volume #2
    // {
    // 	vec3i volumeDims(256);
    // 	vec3f volumeStart(-2.0f, 0.0f, -2.0f);
    // 	vec3f volumeStop(2.0f, 4.0f, 2.0f);
    // 	vec3f volumeSpac = (volumeStop - volumeStart) / (vec3f)(volumeDims - 1);
    // 	OSPVolume volume = ospNewVolume("block_bricked_volume");
    // 	ospSetString(volume, "voxelType", "uchar");
    // 	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    // 	ospSetVec3f(volume, "gridOrigin", (osp::vec3f&)volumeStart);
    // 	ospSetVec3f(volume, "gridSpacing", (osp::vec3f&)volumeSpac);
    // 	ospSet1i(volume, "singleShade", 0);
    // 	ospSetObject(volume, "transferFunction", transferFcn);
    // 	ospSetRegion(volume, volumeData.data(), osp::vec3i{ 0, 0, 0 }, (osp::vec3i&)volumeDims);
    // 	ospCommit(volume);
    // 	ospAddVolume(world, volume);
    // }
    // // volume #3
    // std::vector<unsigned char> volumeData(256 * 512 * 256, 0);
    // for (size_t i = 0; i < volumeData.size(); ++i) { volumeData[i] = i % 256; }
    // {
    // 	vec3i volumeDims(256, 512, 256);
    // 	vec3f volumeStart(-2.0f, -4.0f, -2.0f);
    // 	vec3f volumeStop(2.0f, 4.0f, 2.0f);
    // 	vec3f volumeSpac = (volumeStop - volumeStart) / (vec3f)(volumeDims - 1);
    // 	OSPVolume volume = ospNewVolume("block_bricked_volume");
    // 	ospSetString(volume, "voxelType", "uchar");
    // 	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    // 	//ospSetVec3f(volume, "gridOrigin", (osp::vec3f&)volumeStart);
    // 	//ospSetVec3f(volume, "gridSpacing", (osp::vec3f&)volumeSpac);
    // 	ospSet1i(volume, "singleShade", 0);
    // 	ospSetObject(volume, "transferFunction", transferFcn);
    // 	ospSetRegion(volume, volumeData.data(), osp::vec3i{ 0, 0, 0 }, osp::vec3i{ 256, 256, 256 });
    // 	// ospSetRegion(volume, volumeData.data(), osp::vec3i{ 0, 256, 0 }, osp::vec3i{ 256, 512, 256 });
    // 	ospCommit(volume);
    // 	ospAddVolume(world, volume);
    // }
    // volume #4 testing ghost shared structured volume
    std::vector<unsigned char> volumeDataA(256 * 256 * 256, 0);
    for (size_t i = 0; i < volumeDataA.size(); ++i) { volumeDataA[i] = (i % 256)/2; }
    std::vector<unsigned char> volumeDataB(256 * 256 * 256, 0);
    for (size_t i = 0; i < volumeDataB.size(); ++i) { volumeDataB[i] = (i % 256)/2 + 128; }

    auto t1 = std::chrono::system_clock::now();

    {
    	vec3i volumeDims(256, 256, 256);
    	// OSPVolume volume = ospNewVolume("shared_structured_volume");
    	OSPVolume volume = ospNewVolume("block_bricked_volume");
    	OSPData voxelData = ospNewData(volumeDims.x * volumeDims.y * volumeDims.z, OSP_UCHAR, volumeDataA.data(), OSP_DATA_SHARED_BUFFER);
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", osp::vec3f{0.0f, 0.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 8.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 0);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
    	// ospSetData(volume, "voxelData", voxelData);
    	ospSetRegion(volume, volumeDataA.data(), osp::vec3i{ 0, 0, 0 }, osp::vec3i{ 256, 256, 256 });
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }
    {
    	vec3i volumeDims(256, 256, 256);
    	// OSPVolume volume = ospNewVolume("shared_structured_volume");
    	OSPVolume volume = ospNewVolume("block_bricked_volume");
    	OSPData voxelData = ospNewData(volumeDims.x * volumeDims.y * volumeDims.z, OSP_UCHAR, volumeDataA.data(), OSP_DATA_SHARED_BUFFER);
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", osp::vec3f{0.0f, -255.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 8.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 0);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
    	// ospSetData(volume, "voxelData", voxelData);
    	ospSetRegion(volume, volumeDataA.data(), osp::vec3i{ 0, 0, 0 }, osp::vec3i{ 256, 256, 256 });
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }
    {
    	vec3i volumeDims(256, 256, 256);
    	// OSPVolume volume = ospNewVolume("shared_structured_volume");
    	OSPVolume volume = ospNewVolume("block_bricked_volume");
    	OSPData voxelData = ospNewData(volumeDims.x * volumeDims.y * volumeDims.z, OSP_UCHAR, volumeDataB.data(), OSP_DATA_SHARED_BUFFER);
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", osp::vec3f{-255.0f, 0.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 8.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 0);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
    	// ospSetData(volume, "voxelData", voxelData);
    	ospSetRegion(volume, volumeDataB.data(), osp::vec3i{ 0, 0, 0 }, osp::vec3i{ 256, 256, 256 });
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }
    {
    	vec3i volumeDims(256, 256, 256);
    	// OSPVolume volume = ospNewVolume("shared_structured_volume");
    	OSPVolume volume = ospNewVolume("block_bricked_volume");
    	OSPData voxelData = ospNewData(volumeDims.x * volumeDims.y * volumeDims.z, OSP_UCHAR, volumeDataB.data(), OSP_DATA_SHARED_BUFFER);
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", osp::vec3f{-255.0f, -255.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 8.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 0);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
    	// ospSetData(volume, "voxelData", voxelData);
    	ospSetRegion(volume, volumeDataB.data(), osp::vec3i{ 0, 0, 0 }, osp::vec3i{ 256, 256, 256 });
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }

    auto t2 = std::chrono::system_clock::now();

    std::chrono::duration<double> dur = t2 - t1;
    std::cout << "finish commits " << dur.count() / 4.0 << "seconds" << std::endl;

    ospCommit(world);

    //! camera
    camera = ospNewCamera("perspective");
    ospSetf(camera, "aspect", static_cast<float>(WINSIZE.x) / static_cast<float>(WINSIZE.y));
    UpdateCamera(false);

    //! lighting
    OSPLight ambient_light = ospNewLight(renderer, "AmbientLight");
    ospCommit(ambient_light);
    OSPLight directional_light = ospNewLight(renderer, "DirectionalLight");
    ospSetVec3f(directional_light, "direction", osp::vec3f{ 0.0f, 11.0f, 0.0f});
    ospCommit(directional_light);
    std::vector<OSPLight> light_list { ambient_light, directional_light };
    OSPData lights = ospNewData(light_list.size(), OSP_OBJECT, light_list.data());
    ospCommit(lights);

    //! renderer
    ospSetVec3f(renderer, "bgColor", osp::vec3f{0.5f, 0.5f, 0.5f});
    ospSetData(renderer, "lights", lights);
    ospSetObject(renderer, "model", world);
    ospSetObject(renderer, "camera", camera);
    ospCommit(renderer);

    //! render to buffer
    framebuffer = ospNewFrameBuffer((osp::vec2i&)WINSIZE, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
    ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
    ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
    fb_osp = (uint32_t*)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);

    //! initialize openGL
    {
	glutInit(&argc, const_cast<char**>(argv));
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WINX, WINY);
	glutInitWindowSize(WINSIZE.x, WINSIZE.y);
	glutCreateWindow(argv[0]);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
	    std::cerr << "Error: Cannot Initialize GLEW " << glewGetErrorString(err) << std::endl;
	    return EXIT_FAILURE;
	}
	fb_gl.Initialize(true, 4, WINSIZE.x, WINSIZE.y);
    }

    // execute the program
    {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glDisable(GL_DEPTH_TEST);
	glutDisplayFunc(render);
	glutIdleFunc(Idle);
	glutMouseFunc(GetMouseButton);
	glutMotionFunc(GetMousePosition);
	glutKeyboardFunc(GetNormalKeys);
	glutInitContextFlags(GLUT_DEBUG);
	glutMainLoop();
    }

    // exit
    clean();
    return EXIT_SUCCESS;
}

