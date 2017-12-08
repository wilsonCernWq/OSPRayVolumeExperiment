# OSPRay Volume Experiment Viewer

This is an simple OSPRay example for testing volume renderer

## Build
```
mkdir build
cd build
cmake .. -Dospray_DIR=<...> -Dembree_DIR=<...> -DTBB_ROOT=<...> -DISPC_EXECUTABLE=<...> -DOSPRAY_USE_EXTERNAL_EMBREE=ON
make -j8
```
