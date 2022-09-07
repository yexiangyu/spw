# spw

Sapera Wrapper

## how to build

### Step 1 build dll 
```shell
cd cpp
md build
cd build
cmake ..
cmake --build build --config Release
cmake --build build --config Debug
```

### Step 2 build spw-rs

```shell
set SPW_DLL_DIR=where_dll_is_located
cargo build
```