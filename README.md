# cl-sdk-cpp

High-Performance C/C++ SDK for Cortical Labs HD-MEA

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-C%2B%2B17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)

`cl-sdk-cpp` is a C/C++ SDK designed for interfacing with Cortical Labs' 59-channel High-Density Microelectrode Arrays (HD-MEA). It provides a low-latency network bridge for connecting simulations or engines to the CL1 array.

## Official Documentation & Integration

For authoritative API references, visit the [Cortical Labs Documentation](https://docs.corticallabs.com/) and [Cortical Labs GitHub](https://github.com/Cortical-Labs).

## Architecture Highlights

1. **C Core (`libclsdk`):** A C99 library managing socket connections, threading, and JSON serialization.
2. **C++ OOP Layer (`CorticalLabs.hpp`):** A C++17 wrapper offering RAII semantics and STL abstractions.
3. **Nim FFI (`cl_sdk.nim`):** Bindings for Nim integration.

## Getting Started

### Prerequisites
- CMake 3.10+
- A C++17 compatible compiler
- Cortical Labs API Key

### Build Instructions

```bash
mkdir build && cd build
cmake ..
make -j4
```

### Quick Start (C++ Wrapper)

```cpp
#include "CorticalLabs.hpp"
#include <iostream>

using namespace cortical_labs;

int main() {
    try {
        DishConnection dish("wss://api.corticallabs.com/v1/dish", "YOUR_API_KEY");
        dish.connect();

        std::vector<float> flow_x(59, 0.5f);
        std::vector<float> flow_y(59, -0.2f);
        dish.sendOpticalFlow(1005, flow_x, flow_y);

        auto spikes = dish.receiveSpikes(100);
        for (const auto& s : spikes) {
            std::cout << "Spike on Ch " << (int)s.channel_id << " Amp: " << s.amplitude << "\n";
        }
    } catch (const HDMEAException& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
```

---
*Maintained under Roe Defense.*
