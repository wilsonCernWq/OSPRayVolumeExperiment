# OSPRay Volume Experiment Viewer

This is an simple OSPRay example for testing volume renderer. It requires the [TransferFunctionModule](https://github.com/wilsonCernWq/TransferFunctionModule)

## Build
```
mkdir build
cd build
cmake .. -Dospray_DIR=<...> -Dembree_DIR=<...> -DTBB_ROOT=<...> -DISPC_EXECUTABLE=<...> -DTFN_MODULE_ROOT=<...>
make -j8
```
