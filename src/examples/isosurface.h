#pragma once
#ifndef _ISOSURFACE_H_
#define _ISOSURFACE_H_

inline void test_isosurface(int argc, const char **argv)
{
    //! setup everything as ghost zoom test
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
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f };
    SetupTF(colors, opacities);

    // ! create volume
    camZoom *= 10.0;
    const vec3i dims(101, 100, 100);
    auto volumeDataA = new unsigned char[dims.x * dims.y * dims.z];
    auto volumeDataB = new unsigned char[dims.x * dims.y * dims.z];
    float iso[10];
    cleanlist.push_back([=](){ 
	    delete[] volumeDataA;
	    delete[] volumeDataB;
	});
    auto vt1 = std::chrono::system_clock::now();
    for (int x = -99; x < 101; ++x) {
	int v = round((x + 99) / 200.0 * 256.0);
	if ((x+99) % 20 == 10) {
	    iso[(x+99)/20] = (float)v;
	}
    	for (int y = 0; y < dims.y; ++y) {
    	    for (int z = 0; z < dims.z; ++z) {
    		if (x <=1) {
    		    int i = z * dims.y * dims.x + y * dims.x + (x + 99);
    		    volumeDataB[i] = v;
    		}
    		if (x >= 0) {
    		    int i = z * dims.y * dims.x + y * dims.x + x;
    		    volumeDataA[i] = v;
    		}
    	    }
    	}
    }
    auto vt2 = std::chrono::system_clock::now();
    std::chrono::duration<double> vdur = vt2 - vt1;
    std::cout << "finish initialization " << vdur.count() 
	      << " seconds" << std::endl;
    auto t1 = std::chrono::system_clock::now();
    {
#ifdef USE_VISITOSPRAY      
    	OSPVolume volume = ospNewVolume("visit_shared_structured_volume");
#else
	OSPVolume volume = ospNewVolume("shared_structured_volume");
#endif       
    	OSPData voxelData = ospNewData(dims.x * dims.y * dims.z, 
				       OSP_UCHAR, volumeDataA, 
				       OSP_DATA_SHARED_BUFFER);
	cleanlist.push_back([=](){ // cleaning function
		ospRelease(volume);
		ospRelease(voxelData);
	    });
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)dims);
    	ospSetVec3f(volume, "gridOrigin", 
		    osp::vec3f{0.0f,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSetVec3f(volume, "volumeClippingBoxLower", 
		    osp::vec3f{0.5f,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    	ospSetVec3f(volume, "volumeClippingBoxUpper", 
		    osp::vec3f{100.0f,(float)dims.y/2.0f,(float) dims.z/2.0f});
    	ospSet1f(volume, "samplingRate", 1.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 1);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
    	ospSetData(volume, "voxelData", voxelData);
    	ospCommit(volume);
	// create isosurface	
	OSPGeometry surf = ospNewGeometry("isosurfaces");
    	OSPData values = ospNewData(10, OSP_FLOAT, iso);
	ospCommit(values);
	ospSetData(surf, "isovalues", values);
	ospSetObject(surf, "volume", volume); 
	ospCommit(surf);
	ospAddGeometry(world, surf);	
    }
    {
#ifdef USE_VISITOSPRAY      
    	OSPVolume volume = ospNewVolume("visit_shared_structured_volume");
#else
	OSPVolume volume = ospNewVolume("shared_structured_volume");
#endif       
    	OSPData voxelData = ospNewData(dims.x * dims.y * dims.z, 
				       OSP_UCHAR, volumeDataB, 
				       OSP_DATA_SHARED_BUFFER);
	cleanlist.push_back([=](){ // cleaning function
		ospRelease(volume);
		ospRelease(voxelData);
	    });
    	ospSetString(volume, "voxelType", "uchar");
    	ospSetVec3i(volume, "dimensions", (osp::vec3i&)dims);
    	ospSetVec3f(volume, "gridOrigin", 
		    osp::vec3f{-99.0f,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    	ospSetVec3f(volume, "gridSpacing", osp::vec3f{1.0f, 1.0f, 1.0f});
    	ospSetVec3f(volume, "volumeClippingBoxLower", 
		    osp::vec3f{-99.0f,(float)-dims.y/2.0f,(float)-dims.z/2.0f});
    	ospSetVec3f(volume, "volumeClippingBoxUpper", 
		    osp::vec3f{0.5f,(float) dims.y/2.0f,(float) dims.z/2.0f});
    	ospSet1f(volume, "samplingRate", 1.0f);
    	ospSet1i(volume, "preIntegration", 0);
    	ospSet1i(volume, "adaptiveSampling", 1);
    	ospSet1i(volume, "singleShade", 0);
    	ospSetObject(volume, "transferFunction", transferFcn);
    	ospSetData(volume, "voxelData", voxelData);
    	ospCommit(volume);
	// create isosurface	
	OSPGeometry surf = ospNewGeometry("isosurfaces");
    	OSPData values = ospNewData(10, OSP_FLOAT, iso);
	ospCommit(values);
	ospSetData(surf, "isovalues", values);
	ospSetObject(surf, "volume", volume); 
	ospCommit(surf);
	ospAddGeometry(world, surf);	
    }
    auto t2 = std::chrono::system_clock::now();
    std::chrono::duration<double> dur = t2 - t1;
    std::cout << "finish commits " << dur.count() / 2.0 
	      << " seconds" << std::endl;
}

#endif//_ISOSURFACE_H_
