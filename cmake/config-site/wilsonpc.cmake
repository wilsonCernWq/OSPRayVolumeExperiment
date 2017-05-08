#
# configuration for wilsonpc
#   written by Qi, WU 24 Feb 2017
#
set(embree_DIR D:/Software/OSPRay/embree-2.14.0.x64.windows/lib/cmake/embree-2.14.0)
set(ospray_DIR D:/Software/OSPRay/ospray-1.2.0.windows/lib/cmake/ospray-1.2.0)
set(TBB_ROOT D:/Software/OSPRay/tbb2017_20161128oss)
set(ISPC_EXECUTABLE D:/Software/OSPRay/ispc-v1.9.1-windows-vs2015)
#
# don't want it to appear in the cmake main window
mark_as_advanced(embree_DIR ospray_DIR)
