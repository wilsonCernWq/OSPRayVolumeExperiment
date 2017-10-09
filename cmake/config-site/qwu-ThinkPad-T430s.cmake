#
# configuration for hastur
#
# OSPRay development directory
set(ospray_DIR /home/qwu/work/ospray/install/lib/cmake/ospray-1.4.1)
set(embree_DIR /home/qwu/software/embree-2.15.0.x86_64.linux)
add_definitions(-DUSE_VISITOSPRAY)
# 
# other library
set(TBB_ROOT /home/qwu/software/tbb2017_20170604oss)
set(ISPC_EXECUTABLE /home/qwu/software/ispc-v1.9.1-linux)
set(OSPRAY_USE_EXTERNAL_EMBREE ON) 
