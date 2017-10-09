#pragma once
#ifndef _MARCO_CELL_H_
#define _MARCO_CELL_H_

#include "../common.h"
#include "../helper.h"
#include "../global.h"

inline void test_marco_cell(int argc, const char **argv)
{
    //! check arguments
    if (argc != 4) { 
	std::cerr << "need 3 arguments" << std::endl;
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

    // testing marco cell
#ifdef USE_VISITOSPRAY      
    std::string volumetype = std::stoi(argv[3]) == 1 ? 
	"visit_shared_structured_volume" : 
	"shared_structured_volume";
#else
    std::string volumetype = "shared_structured_volume";
#endif
    const int dim = std::stoi(argv[2]); camZoom *= dim / 5;    
    auto vt1 = std::chrono::system_clock::now();
    // false sharing problem causes openMP performs badly on heap array
    // https://stackoverflow.com/questions/6605677/openmp-poor-performance-of-heap-arrays-stack-arrays-work-fine
    unsigned char* volumeDataA = new unsigned char[dim * dim * dim];
    unsigned char* volumeDataB = new unsigned char[dim * dim * dim];
    cleanlist.push_back([=](){ // cleaning function
	    delete[] volumeDataA;
	    delete[] volumeDataB;
	});
    //#pragma omp parallel for
    for (size_t i = 0; i < dim * dim * dim; ++i) {
       volumeDataA[i] = 
	 (unsigned char) std::floor
	   (((int)(((float)i/dim) * 256) % 256) / 2.0f); 
       volumeDataB[i] = 
	 (unsigned char) std::ceil 
	   (((int)(((float)i/dim) * 256) % 256) / 2.0f) + 128; 
    }
    auto vt2 = std::chrono::system_clock::now();
    std::chrono::duration<double> vdur = vt2 - vt1;
    std::cout << "allocation time " << vdur.count() / 2.0 
	      << " seconds" << std::endl;

    // assign to ospray
    const vec3i volumeDims(dim, dim, dim);
    auto t1 = std::chrono::system_clock::now();
    {
    	OSPVolume volume = ospNewVolume(volumetype.c_str());
    	OSPData voxelData = ospNewData(volumeDims.x * 
				       volumeDims.y * 
				       volumeDims.z, 
				       OSP_UCHAR, volumeDataA, 
				       OSP_DATA_SHARED_BUFFER);
	cleanlist.push_back([=](){ // cleaning function
		ospRelease(volume);
		ospRelease(voxelData);
	    });
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", osp::vec3f{0.0f, 0.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 0.25f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 1);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
	ospSetData(volume, "voxelData", voxelData);
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }
    {
	OSPVolume volume = ospNewVolume(volumetype.c_str());
    	OSPData voxelData = ospNewData(volumeDims.x * 
				       volumeDims.y *
				       volumeDims.z, 
				       OSP_UCHAR, volumeDataA, 
				       OSP_DATA_SHARED_BUFFER);
	cleanlist.push_back([=](){ // cleaning function
		ospRelease(volume);
		ospRelease(voxelData);
	    });
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", 
		    osp::vec3f{0.0f, (float)-dim + 1.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 0.25f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 1);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
    	ospSetData(volume, "voxelData", voxelData);
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }
    {
    	OSPVolume volume = ospNewVolume(volumetype.c_str());
    	OSPData voxelData = ospNewData(volumeDims.x * 
				       volumeDims.y * 
				       volumeDims.z, 
				       OSP_UCHAR, volumeDataB, 
				       OSP_DATA_SHARED_BUFFER);
	cleanlist.push_back([=](){ // cleaning function
		ospRelease(volume);
		ospRelease(voxelData);
	    });
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", 
		    osp::vec3f{(float)-dim + 1.0f, 0.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 3.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 0);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
	ospSetData(volume, "voxelData", voxelData);
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }
    {
	OSPVolume volume = ospNewVolume(volumetype.c_str());
    	OSPData voxelData = ospNewData(volumeDims.x * 
				       volumeDims.y * 
				       volumeDims.z, 
				       OSP_UCHAR, volumeDataB, 
				       OSP_DATA_SHARED_BUFFER);
	cleanlist.push_back([=](){ // cleaning function
		ospRelease(volume);
		ospRelease(voxelData);
	    });
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
    	ospSetVec3f(volume, "gridOrigin", 
		    osp::vec3f{(float)-dim + 1.0f, (float)-dim + 1.0f, 0.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSet1f(volume, "samplingRate", 3.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 0);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
	ospSetData(volume, "voxelData", voxelData);
    	ospCommit(volume);
    	ospAddVolume(world, volume);
    }
    auto t2 = std::chrono::system_clock::now();
    std::chrono::duration<double> dur = t2 - t1;
    std::cout << "finish commits " << dur.count() / 4.0 
	      << " seconds" << std::endl;

}

#endif//_MARCO_CELL_H_
