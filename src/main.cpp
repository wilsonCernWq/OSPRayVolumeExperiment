#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "callback.h"
#include "examples/ghost_zoom.h"
#include "examples/marco_cell.h"
#include "examples/isosurface.h"
#include "examples/instance_volume.h"

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

int main(int argc, const char **argv)
{
    ospInit(&argc, argv);    

#ifdef USE_VISITOSPRAY
    OSPError err = ospLoadModule("visit");
    if (err != OSP_NO_ERROR) {
      std::cerr << "[Error] can't load visit module" << std::endl;
    }
#endif

    //! create world and renderer
    world = ospNewModel();
    renderer = ospNewRenderer("scivis"); 

    //! setup volume/geometry
    if (argc < 2) {
	std::cerr << "need example name" << std::endl;
	exit(-1);
    }
    std::string teststr(argv[1]);
    if (teststr.compare("ghost_zoom") == 0) {
	test_ghost_zoom(argc, argv);
    }
    else if (teststr.compare("marco_cell") == 0) {
	test_marco_cell(argc, argv);
    }
    else if (teststr.compare("isosurface") == 0) {
	test_isosurface(argc, argv);
    }
    else if (teststr.compare("instance_volume") == 0) {
	test_instance_volume(argc, argv);
    }
    ospCommit(world);

    //! camera
    camera = ospNewCamera("perspective");
    ospSetf(camera, "aspect", 
	    static_cast<float>(WINSIZE.x) / 
	    static_cast<float>(WINSIZE.y));
    UpdateCamera(false);

    //! lighting
    OSPLight ambient_light = ospNewLight(renderer, "AmbientLight");
    ospSet1f(ambient_light, "intensity", 0.0f);
    ospCommit(ambient_light);
    OSPLight directional_light = ospNewLight(renderer, "DirectionalLight");
    ospSet1f(directional_light, "intensity", 2.0f);
    ospSetVec3f(directional_light, "direction", 
		osp::vec3f{20.0f, 20.0f, 20.0f});
    ospCommit(directional_light);
    std::vector<OSPLight> light_list { ambient_light, directional_light };
    OSPData lights = 
	ospNewData(light_list.size(), OSP_OBJECT, light_list.data());
    ospCommit(lights);

    //! renderer
    ospSetVec3f(renderer, "bgColor", osp::vec3f{0.5f, 0.5f, 0.5f});
    ospSetData(renderer, "lights", lights);
    ospSetObject(renderer, "model", world);
    ospSetObject(renderer, "camera", camera);
    ospSet1i(renderer, "shadowEnabled", 0);
    ospSet1i(renderer, "oneSidedLighting", 0);
    ospCommit(renderer);

    //! render to buffer
    framebuffer = ospNewFrameBuffer((osp::vec2i&)WINSIZE, 
				    OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
    ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
    ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
    ofb = (uint32_t*)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);

    //! initialize openGL
    {
	glutInit(&argc, const_cast<char**>(argv));
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WINX, WINY);
	glutInitWindowSize(WINSIZE.x, WINSIZE.y);
	glutCreateWindow(argv[0]);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
	    std::cerr << "Error: Cannot Initialize GLEW " 
		      << glewGetErrorString(err) << std::endl;
	    return EXIT_FAILURE;
	}
	gfb.Initialize(true, 4, WINSIZE.x, WINSIZE.y);
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
	glutSpecialFunc(GetSpecialKeys);
	glutInitContextFlags(GLUT_DEBUG);
	glutMainLoop();
    }

    // exit
    Clean();
    return EXIT_SUCCESS;
}

