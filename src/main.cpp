#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "callback.h"

void SetupTF(const std::vector<vec3f>& colors, 
	     const std::vector<float>& opacities)
{
  //! setup trasnfer function
  transferFcn = ospNewTransferFunction("piecewise_linear_2d");
  OSPData colorsData = ospNewData(colors.size(), 
				  OSP_FLOAT3, 
				  colors.data());
  ospCommit(colorsData);
  OSPData opacityData = ospNewData(opacities.size(), 
				   OSP_FLOAT, 
				   opacities.data());
  ospCommit(opacityData);
  const vec2f valueRange(static_cast<float>(0), static_cast<float>(255));
  ospSetData(transferFcn, "colors", colorsData);
  ospSetData(transferFcn, "opacities", opacityData);
  ospSetVec2f(transferFcn, "valueRange", (osp::vec2f&)valueRange);
  ospCommit(transferFcn);
}

void volume(int argc, const char **argv) 
{
  int useGridAccelerator = 0; //argc > 2 ? atoi(argv[2]) : 0;
  int gradRendering      = 0; //argc > 3 ? atoi(argv[3]) : 0;
  //! transfer function
  const std::vector<vec3f> colors = {
    vec3f(0, 0, 0.563),
    vec3f(0, 0, 1),
    vec3f(0, 1, 1),
    vec3f(0.5, 1, 0.5),
    vec3f(1, 1, 0),
    vec3f(1, 0, 0),
    vec3f(0.5, 0, 0)
  };
  const std::vector<float> opacities = 
    { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
  SetupTF(colors, opacities);

  // ! create volume
  const vec3i dims(12, 10, 10);
  auto volumeDataA = new unsigned char[dims.x * dims.y * dims.z];
  auto volumeDataB = new unsigned char[dims.x * dims.y * dims.z];
  cleanlist.push_back([=](){ 
      delete[] volumeDataA;
      delete[] volumeDataB;
    });
  const int xA = -1;
  const int xB = -9;
  for (int x = -9; x < 11; ++x) {
    int c;
    c = ::sin((x + 9)/19.0 * M_PI / 2.0) * 255.0;
    std::cout << "x = " << x << " v = " << c << std::endl;
    for (int y = 0; y < dims.y; ++y) {
      for (int z = 0; z < dims.z; ++z) {		
	if (x <= 2) { // x = -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2
	  int i = z * dims.y * dims.x + y * dims.x + x + 9;
	  volumeDataB[i] = c;
	}
	if (x >=-1) { // x = -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	  int i = z * dims.y * dims.x + y * dims.x + x + 1;
	  volumeDataA[i] = c;
	}
      }
    }
  }
  auto t1 = std::chrono::system_clock::now();
  {
    //OSPVolume volume = ospNewVolume("visit_shared_structured_volume");
    //OSPVolume volume = ospNewVolume("shared_structured_volume");
    OSPVolume volume = ospNewVolume("shared_structured_volume");
    OSPData voxelData = ospNewData(dims.x * dims.y * dims.z, 
				   OSP_UCHAR, volumeDataA, 
				   OSP_DATA_SHARED_BUFFER);
    cleanlist.push_back([=](){ // cleaning function
	ospRelease(volume);
	ospRelease(voxelData);
      });
    ospSet1i(volume, "useGridAccelerator", useGridAccelerator);
    ospSetString(volume, "voxelType", "uchar");
    ospSetVec3i(volume, "dimensions", (osp::vec3i&)dims);
    ospSetVec3f(volume, "gridOrigin", 
		osp::vec3f
		{(float)xA,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    ospSetVec3f(volume, "volumeClippingBoxLower",
		osp::vec3f{0.5f,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    ospSetVec3f(volume, "volumeClippingBoxUpper",
		osp::vec3f{10.0f,(float) dims.y/2.0f,(float) dims.z/2.0f});
    ospSet1f(volume, "samplingRate", 8.0f);
    ospSet1i(volume, "preIntegration", 0);
    ospSet1i(volume, "adaptiveSampling", 0);
    ospSet1i(volume, "singleShade", 0);
    ospSetObject(volume, "transferFunction", transferFcn);
    ospSetData(volume, "voxelData", voxelData);
    ospSet1i(volume, "gradientShadingEnabled", gradRendering);
    ospCommit(volume);
    ospAddVolume(world, volume);
  }
  {
    //OSPVolume volume = ospNewVolume("visit_shared_structured_volume");
    //OSPVolume volume = ospNewVolume("shared_structured_volume");
    OSPVolume volume = ospNewVolume("shared_structured_volume");
    OSPData voxelData = ospNewData(dims.x * dims.y * dims.z,
				   OSP_UCHAR, volumeDataB,
				   OSP_DATA_SHARED_BUFFER);
    cleanlist.push_back([=](){ // cleaning function
	ospRelease(volume);
	ospRelease(voxelData);
      });
    ospSet1i(volume, "useGridAccelerator", useGridAccelerator);
    ospSetString(volume, "voxelType", "uchar");
    ospSetVec3i(volume, "dimensions", (osp::vec3i&)dims);
    ospSetVec3f(volume, "gridOrigin",
		osp::vec3f
		{(float)xB,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    ospSetVec3f(volume, "volumeClippingBoxLower",
		osp::vec3f{-9.0f,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    ospSetVec3f(volume, "volumeClippingBoxUpper",
		osp::vec3f{0.5f,(float)dims.y/2.0f,(float) dims.z/2.0f});
    ospSet1f(volume, "samplingRate", 8.0f);
    ospSet1i(volume, "preIntegration", 0);
    ospSet1i(volume, "adaptiveSampling", 0);
    ospSet1i(volume, "singleShade", 0);
    ospSetObject(volume, "transferFunction", transferFcn);
    ospSetData(volume, "voxelData", voxelData);
    ospSet1i(volume, "gradientShadingEnabled", gradRendering);
    ospCommit(volume);
    ospAddVolume(world, volume);
  }
  auto t2 = std::chrono::system_clock::now();
  std::chrono::duration<double> dur = t2 - t1;
  std::cout << "finish commits " << dur.count() / 2.0 << " seconds" << std::endl;
}

int main(int argc, const char **argv)
{
  ospInit(&argc, argv);    
  ospLoadModule("visit");
  ospLoadModule("tfn");
    
  //! create world and renderer
  world = ospNewModel();
  renderer = ospNewRenderer("scivis"); 

  //! setup volume/geometry
  volume(argc, argv);    
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

