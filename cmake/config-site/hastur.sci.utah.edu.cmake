#
# configuration for hastur
#
# OSPRay development directory
if (USE_VISITOSPRAY)
  set(ospray_DIR /home/sci/qwu/OSPRay/OSPRay-git/install/lib64/cmake/ospray-1.3.0)
  set(ospray_SRC /home/sci/qwu/OSPRay/OSPRay-git)
  set(embree_DIR /home/sci/qwu/software/embree-2.15.0.x86_64.linux)
  add_definitions(-DUSE_VISITOSPRAY)
else (USE_VISITOSPRAY)
  set(ospray_DIR /home/sci/qwu/software/ospray-1.3.0.x86_64.linux/lib/cmake/ospray-1.3.0)
  set(embree_DIR /home/sci/qwu/software/embree-2.16.4.x86_64.linux)
  remove_definitions(-DUSE_VISITOSPRAY)
endif (USE_VISITOSPRAY)
# 
# other library
set(TBB_ROOT /home/sci/qwu/software/tbb2017_20160916oss)
set(ISPC_EXECUTABLE /home/sci/qwu/software/ispc-v1.9.1-linux)
set(OSPRAY_USE_EXTERNAL_EMBREE ON) 

