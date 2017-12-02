#include "common.h"
#include "trackball.h"
#include "callback.h"

void SetupTF(const std::vector<ospcommon::vec3f>& colors, 
	     const std::vector<float>& opacities, 
	     int colorW, int colorH, int opacityW, int opacityH)
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
  const ospcommon::vec2f valueRange(static_cast<float>(0), static_cast<float>(255));
  ospSetData(transferFcn, "colors", colorsData);
  ospSetData(transferFcn, "opacities", opacityData);
  ospSetVec2f(transferFcn, "valueRange", (osp::vec2f&)valueRange);
  ospSet1i(transferFcn, "colorWidth",  colorW);
  ospSet1i(transferFcn, "colorHeight", colorH);
  ospSet1i(transferFcn, "opacityWidth",  opacityW);
  ospSet1i(transferFcn, "opacityHeight", opacityH);
  ospCommit(transferFcn);
}

void volume(int argc, const char **argv) 
{
  int useGridAccelerator = 0; //argc > 2 ? atoi(argv[2]) : 0;
  int gradRendering      = 0; //argc > 3 ? atoi(argv[3]) : 0;
  //! transfer function
  const std::vector<ospcommon::vec3f> colors = {
    ospcommon::vec3f(0, 0, 0.563),
    ospcommon::vec3f(0, 0, 1),
    ospcommon::vec3f(0, 1, 1),
    ospcommon::vec3f(0.5, 1, 0.5),
    ospcommon::vec3f(1, 1, 0),
    ospcommon::vec3f(1, 0, 0),
    ospcommon::vec3f(0.5, 0, 0),
  };
  const std::vector<float> opacities = 
    { 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f };
  SetupTF(colors, opacities, 7, 1, 6, 1);

  // ! create volume
  const ospcommon::vec3i dims(12, 10, 10);
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
  //------------------------------------------------------------------------------------------//
  // OpenGL Setup
  //------------------------------------------------------------------------------------------//
  // Create Context
  GLFWwindow* window = InitWindow();
  check_error_gl("Initialized OpenGL");

  //------------------------------------------------------------------------------------------//
  // OSPRay Setup
  //------------------------------------------------------------------------------------------//
  ospInit(&argc, argv);
  ospLoadModule("tfn");
  
  //! Init camera and framebuffer
  camera.Init();
  framebuffer.Init(camera.CameraWidth(), camera.CameraHeight());

  //! create world and renderer
  world = ospNewModel();
  renderer = ospNewRenderer("scivis"); 

  //! setup volume/geometry
  volume(argc, argv);    
  ospCommit(world);

  //! lighting
  OSPLight ambient_light = ospNewLight(renderer, "AmbientLight");
  ospSet1f(ambient_light, "intensity", 0.0f);
  ospCommit(ambient_light);
  OSPLight directional_light = ospNewLight(renderer, "DirectionalLight");
  ospSet1f(directional_light, "intensity", 2.0f);
  ospSetVec3f(directional_light, "direction", osp::vec3f{20.0f, 20.0f, 20.0f});
  ospCommit(directional_light);
  std::vector<OSPLight> light_list { ambient_light, directional_light };
  OSPData lights = ospNewData(light_list.size(), OSP_OBJECT, light_list.data());
  ospCommit(lights);

  //! renderer
  ospSetVec3f(renderer, "bgColor", osp::vec3f{0.5f, 0.5f, 0.5f});
  ospSetData(renderer, "lights", lights);
  ospSetObject(renderer, "model", world);
  ospSetObject(renderer, "camera", camera.OSPRayPtr());
  ospSet1i(renderer, "shadowEnabled", 0);
  ospSet1i(renderer, "oneSidedLighting", 0);
  ospCommit(renderer);

  //------------------------------------------------------------------------------------------//
  // Render
  //------------------------------------------------------------------------------------------//
  while (!glfwWindowShouldClose(window))
  {
    // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // render
    render();
    // swap frame
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  ShutdownWindow(window);
  glfwTerminate();

  // exit
  Clean();
  return EXIT_SUCCESS;
}

