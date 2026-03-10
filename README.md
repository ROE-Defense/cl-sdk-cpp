# 🧠 cl-sdk-cpp

**⚡️ High-Performance C/C++ SDK for Cortical Labs HD-MEA ⚡️**

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-C%2B%2B17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)

`cl-sdk-cpp` is a zero-overhead 🏎️, bare-metal C/C++ SDK designed specifically for interfacing with Cortical Labs' 59-channel High-Density Microelectrode Arrays (HD-MEA) 🧫. Engineered for AAA game developers 🎮, engine programmers ⚙️, and robotics researchers 🤖, this SDK provides an ultra-low latency bridge between synthetic neural environments and modern game engines 🌉.

## 🔗 Official Documentation & Integration

For the authoritative source on the API and hardware, visit the official **[Cortical Labs Documentation](https://docs.corticallabs.com/)** and the **[official Cortical Labs GitHub](https://github.com/Cortical-Labs)**.

`cl-sdk-cpp` is designed to be fully interoperable with the official `Cortical-Labs/cl-api-doc` schemas and the CL1 Simulator.

## 🤔 Why C/C++?

The Cortical Labs Python simulator and API are fantastic for data science 📊, but they introduce unacceptable latency in real-time simulations ⏱️. `cl-sdk-cpp` solves this by bypassing the Python Global Interpreter Lock (GIL) entirely 🚀:

- **Zero GIL Overhead:** Bypasses Python runtime bottlenecks to deliver true deterministic execution 🎯.
- **CL1 Native UDP Spike Firehose:** Explicitly supports a high-performance UDP listener specifically designed to ingest raw UDP spike streams directly from the CL1. This solves the complex plumbing code issues for real-time Unreal Engine and Vulkan UDP streaming 🌊.
- **Detached Threading Model:** Implements an asynchronous, detached threading architecture for the WebSocket and REST network layers, ensuring that simulation ticks in your game engine are never blocked by network I/O 🕸️.
- **Asynchronous Telemetry Downsampling for High-Refresh Engines:** Configurable 25kHz aggregation buffer designed for the CL1 Simulator and physical CL1 hardware, capturing an ultra-high frequency biological sampling rate and delivering it seamlessly to high-refresh game loops 🏎️ (e.g., 60fps DOOM 👹, 90fps VR 🥽, 144fps Unreal 🕹️) without dropping critical high-frequency spike potentials ⚡️.
- **59-Channel HD-MEA Optimization:** Native C-struct serialization directly mapped to the 59-channel architecture of Cortical Labs' hardware, drastically reducing JSON parsing overhead 📦.
- **Engine-Ready bindings:** Designed to be directly dropped into Unreal Engine, custom C++ engines, or bound seamlessly to other compiled languages via FFI 🔌.

## 👑 Nim Engine Compatibility

For developers using the Nim programming language (highly favored in high-performance simulation), `cl-sdk-cpp` provides first-class FFI bindings 🤝. Nim's deterministic memory management pairs perfectly with the C-core, offering Python-like syntax with C-like speed 🐍⚡. See `examples/cl_sdk.nim` for a complete example of connecting to the dish via Nim 🍽️.

## 🏛️ Architecture Highlights

1. **C Core (`libclsdk`):** A strictly bounded, `malloc`-minimal C99 library managing raw socket connections, threading, and JSON serialization 🧱.
2. **C++ OOP Layer (`CorticalLabs.hpp`):** A modern C++17 wrapper offering RAII semantics, exception handling, and `std::vector` abstractions for developers who prefer modern C++ 🏗️.
3. **Nim FFI (`cl_sdk.nim`):** Zero-cost bindings for Nim integrations 🚀.

## 🏁 Getting Started

### 📋 Prerequisites
- CMake 3.10+ 🛠️
- A C++17 compatible compiler 🖥️
- Cortical Labs API Key 🔑

### 🔨 Build Instructions

```bash
mkdir build && cd build
cmake ..
make -j4
```

### ⚡ Quick Start (C++ Wrapper)

```cpp
#include "CorticalLabs.hpp"
#include <iostream>

using namespace cortical_labs;

int main() {
    try {
        // Initialize detached WebSocket connection
        DishConnection dish("wss://api.corticallabs.com/v1/dish", "YOUR_API_KEY");
        dish.connect();

        // Send Optical Flow Data (59 Channels)
        std::vector<float> flow_x(59, 0.5f);
        std::vector<float> flow_y(59, -0.2f);
        dish.sendOpticalFlow(1005, flow_x, flow_y);

        // Receive Spikes
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

## 🐍 Cortical Labs Python Simulator Interoperability

The `cl-sdk-cpp` JSON serialization logic has been extensively tested against the Cortical Labs Python simulator 🧪. The C-core seamlessly injects generic API keys into the REST/WebSocket payload to ensure drop-in compatibility for researchers migrating from the Python-based workflow to this high-performance C/C++ stack 🔄.

---
*Created and maintained under Roe Defense. 🛡️*