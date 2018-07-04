#include "common.h"
#include "viewer/viewer.h"

using namespace ospcommon;

#define TYPE_CXX unsigned char
#define TYPE_OSP OSP_UCHAR
#define TYPE_STR "uchar"

OSPVolume make_volume(int argc, const char **argv, OSPTransferFunction transferFcn) 
{
  //! create volume
  const ospcommon::vec3i dims(20, 10, 10);
  auto volumeData = new TYPE_CXX[dims.x * dims.y * dims.z];
  for (int x = 0; x < dims.x; ++x) {
    for (int y = 0; y < dims.y; ++y) {
      for (int z = 0; z < dims.z; ++z) {
	int c = std::sin(x/(float)(dims.x-1) * M_PI / 2.0) * 255.0;
	int i = z * dims.y * dims.x + y * dims.x + x;
	volumeData[i] = (TYPE_CXX)c;
      }
    }
  }
  
  //! create ospray volume
  auto t1 = std::chrono::system_clock::now();
  OSPVolume volume = ospNewVolume("shared_structured_volume");
  OSPData voxelData = ospNewData(dims.x * dims.y * dims.z, 
                                 TYPE_OSP, volumeData, 
                                 OSP_DATA_SHARED_BUFFER);
  ospSetString(volume, "voxelType", TYPE_STR);
  ospSetVec3i(volume, "dimensions", (osp::vec3i&)dims);
  ospSet3f(volume, "gridOrigin", -dims.x/2.0f, -dims.y/2.0f, -dims.z/2.0f);
  ospSet3f(volume, "gridSpacing", 1.0f, 1.0f, 1.0f);
  ospSet1f(volume, "samplingRate", 1.0f);
  ospSet1i(volume, "gradientShadingEnabled", 0);
  ospSet1i(volume, "useGridAccelerator", 0);
  ospSet1i(volume, "adaptiveSampling", 0);
  ospSet1i(volume, "preIntegration", 0);
  ospSet1i(volume, "singleShade", 0);
  ospSetData(volume, "voxelData", voxelData);
  ospSetObject(volume, "transferFunction", transferFcn);
  ospCommit(volume);  
  auto t2 = std::chrono::system_clock::now();
  std::chrono::duration<double> dur = t2 - t1;
  std::cout << "finish commits " << dur.count() << " seconds" << std::endl;
  return volume;
}

int main(int ac, const char **av)
{
  int window = viewer::Init(ac, av, 800, 800);

  ospInit(&ac, av);
  ospLoadModule("ispc");
  
  OSPCamera camera = ospNewCamera("perspective");
  OSPModel world = ospNewModel();
  OSPRenderer renderer = ospNewRenderer("scivis"); 
  OSPTransferFunction transferFcn = ospNewTransferFunction("piecewise_linear");
  OSPVolume volume = make_volume(ac, av, transferFcn);    
  ospAddVolume(world, volume);
  ospCommit(world);

  ospSetVec3f(renderer, "bgColor", osp::vec3f{0.5f, 0.5f, 0.5f});
  ospSetObject(renderer, "model", world);
  ospSetObject(renderer, "camera", camera);
  ospSet1i(renderer, "shadowEnabled", 0);
  ospSet1i(renderer, "oneSidedLighting", 0);
  ospCommit(renderer);

  viewer::Handler(camera, "perspective",
                  osp::vec3f {0.f, 0.f, -20.f},
                  osp::vec3f {0.f, 1.f, 0.f},
                  osp::vec3f {0.f, 0.f, 0.f});
  viewer::Handler(transferFcn, 0, 256);
  viewer::Handler(world, renderer);
  viewer::Render(window);

  return EXIT_SUCCESS;
}

