#pragma once
#ifndef _GHOST_ZOOM_H_
#define _GHOST_ZOOM_H_

#include "../common.h"
#include "../helper.h"
#include "../global.h"

inline void test_ghost_zoom(int argc, const char **argv) 
{
    int useGridAccelerator = argc > 2 ? atoi(argv[2]) : 0;
    int gradRendering = argc > 3 ? atoi(argv[3]) : 0;
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
    	for (int y = 0; y < dims.y; ++y) {
    	    for (int z = 0; z < dims.z; ++z) {		
		int iB = (x - xB);
		int iA = (x - xA);
    		if (x <= 2) { // x = -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2
		    int i = z * dims.y * dims.x + y * dims.x + iB;
    		    volumeDataB[i] = (x < 1 ? iB/10.0*60 : iA/10.0*60+195);
    		}
    		if (x >= -1) { // x = -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
		    int i = z * dims.y * dims.x + y * dims.x + iA;
    		    volumeDataA[i] = (x > 0 ? iA/10.0*60+195 : iB/10.0*60);
    		}
    	    }
    	}
    }
    auto t1 = std::chrono::system_clock::now();
    {
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

#endif// _GHOST_ZOOM_H_
