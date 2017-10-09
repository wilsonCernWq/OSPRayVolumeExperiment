#pragma once
#ifndef _INSTANCE_VOLUME_H_
#define _INSTANCE_VOLUME_H_

#include "../common.h"
#include "../helper.h"
#include "../global.h"

inline void test_instance_volume(int argc, const char **argv)
{
    //! check arguments
    if (argc != 3) { 
	std::cerr << "need 2 arguments:" << std::endl
		  << argv[0] << " instance_volume <dimension>" << std::endl;
	exit(-1); 
    }
    
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
	{ 0.01f, 0.01f, 0.01f, 1.f, 1.f, 1.f };
    SetupTF(colors, opacities);

    // testing instance volume
    const int dim = std::stoi(argv[2]); camZoom *= dim / 5;    
    auto vt1 = std::chrono::system_clock::now();        
    unsigned char* volumeData = new unsigned char[dim * dim * dim];
    cleanlist.push_back([=](){ // cleaning function
	    delete[] volumeData;
	});
    for (size_t i = 0; i < dim * dim * dim; ++i) {
      volumeData[i] = (unsigned char)std::floor(((float)i/dim) * 256); 
    }
    auto vt2 = std::chrono::system_clock::now();
    std::chrono::duration<double> vdur = vt2 - vt1;
    std::cout << "allocation time " << vdur.count() << " seconds" << std::endl;

    // assign to ospray
    const vec3i volumeDims(dim, dim, dim);
    auto t1 = std::chrono::system_clock::now();
    {
#ifdef USE_VISITOSPRAY
    	OSPVolume volume = ospNewVolume("visit_shared_structured_volume");
#else
	OSPVolume volume = ospNewVolume("shared_structured_volume");
#endif
    	OSPData voxelData = ospNewData(volumeDims.x * 
				       volumeDims.y * 
				       volumeDims.z, 
				       OSP_UCHAR, volumeData, 
				       OSP_DATA_SHARED_BUFFER);
	cleanlist.push_back([=](){ // cleaning function
		ospRelease(volume);
		ospRelease(voxelData);
	    });
    	ospSetString(volume, "voxelType", "uchar");
	ospSetData(volume, "voxelData", voxelData);
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", 
		    osp::vec3f{-(float)volumeDims.x/2.f, 
			       -(float)volumeDims.y/2.f, 
			       -(float)volumeDims.z/2.f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 0.25f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 1);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
	ospSetVec3f(volume, "xfm.l.vx", osp::vec3f{4.0f, 1.0f, 0.0f});
	ospSetVec3f(volume, "xfm.l.vy", osp::vec3f{-1.0f, 2.0f, 0.0f});
	ospSetVec3f(volume, "xfm.l.vz", osp::vec3f{0.0f, 0.0f, 1.0f});
	ospSetVec3f(volume, "xfm.p",    osp::vec3f{0.0f, 0.0f, 0.0f});
    	ospCommit(volume);
	
    	ospAddVolume(world, volume);
    }
    auto t2 = std::chrono::system_clock::now();
    std::chrono::duration<double> dur = t2 - t1;
    std::cout << "finish commits " << dur.count() << " seconds" << std::endl;

}

#endif//_INSTANCE_VOLUME_H_
