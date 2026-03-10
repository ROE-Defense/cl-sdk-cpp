cmake_minimum_required(VERSION 3.14)
project(CorticalLabsSDK VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(include third_party)

# C-Core Library
add_library(clsdk_c_core STATIC 
    src/libclsdk.c
    third_party/cJSON.c
)
target_include_directories(clsdk_c_core PUBLIC include third_party)

# C++ OOP Wrapper Library
add_library(CorticalLabsCpp STATIC
    src/CorticalLabs.cpp
)
target_include_directories(CorticalLabsCpp PUBLIC include)
target_link_libraries(CorticalLabsCpp PUBLIC clsdk_c_core)

# C++ Example Binary
add_executable(cl_sdk_example src/example.cpp)
target_link_libraries(cl_sdk_example PRIVATE CorticalLabsCpp)

# Tests & Benchmarks
enable_testing()

add_executable(cl_sdk_tests tests/test_runner.cpp)
target_link_libraries(cl_sdk_tests PRIVATE CorticalLabsCpp)
add_test(NAME CLSDKTests COMMAND cl_sdk_tests)

add_executable(cl_benchmark benchmarks/benchmark.cpp)
target_link_libraries(cl_benchmark PRIVATE CorticalLabsCpp)
