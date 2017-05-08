#
# configuration for hastur
#   written by Qi, WU 28 Feb 2017
#
# set(ospray_DIR ~/OSPRay/OSPRay-git/install_embree/lib64/cmake/ospray-1.3.0/)
set(ospray_DIR ~/OSPRay/OSPRay-git/install_embree_qwu_devel/lib64/cmake/ospray-1.3.0/)
set(embree_DIR ~/OSPRay/embree-2.14.0.x86_64.linux/lib/cmake/embree-2.14.0)
set(TBB_ROOT ~/OSPRay/tbb2017_20160916oss)
set(ISPC_EXECUTABLE ~/OSPRay/ispc-v1.9.1-linux)
#
# don't want it to appear in the cmake main window
mark_as_advanced(embree_DIR ospray_DIR)
