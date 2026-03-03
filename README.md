# 🧠 cl-sdk-cpp

[![Build Status](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/build.yml)
[![Security](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/codeql.yml/badge.svg)](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/codeql.yml)
[![Latest Release](https://img.shields.io/github/v/release/ROE-Defense/cl-sdk-cpp)](https://github.com/ROE-Defense/cl-sdk-cpp/releases/latest)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-C%2B%2B17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)

**⚡️ High-Performance C/C++ SDK for Cortical Labs HD-MEA ⚡️**

`cl-sdk-cpp` is a C/C++ SDK designed for interfacing with Cortical Labs' 59-channel High-Density Microelectrode Arrays (HD-MEA). It provides a low-latency, hardware-agnostic network bridge for connecting simulations or engines to the CL1 array. **Note that this SDK is an agnostic network pipe, not a Spiking Neural Network itself.**

---

### 🔥 Killer Feature: Asynchronous Telemetry Downsampling Buffer (25kHz -> 90Hz)

Bypass runtime bottlenecks! The SDK utilizes a fully detached threading model and an **Asynchronous Telemetry Downsampling Buffer** that seamlessly scales the raw 25kHz biological sampling rate down to engine-friendly update loops (e.g., 90Hz for VR or 144Hz for high-refresh rendering), without dropping critical high-frequency spike potentials.

---

## 🏗️ Dual-Engine Architecture

Developers love diagrams. Here is how the high-performance pipeline flows from synthetic environments, to the biological substrate, and back:

```text
    +-----------------------+ Hardware-Agnostic Sensor Data +-------------------------+
    |  Synthetic Workspace  |  (Pixels, Raycasts, Audio)  |     CL1 / Simulator     |
    |  (UE5 / Nim Engine)   | --------------------------> |    (59-Channel Array)   |
    +-----------------------+                             +-------------------------+
               ^                                                       |
               |                                                       |
               |           Biphasic Voltage Signals / Biological Spikes|
               |                  (UDP Firehose / REST)                |
               |                                                       |
               |                 +-------------------------+           |
               +-----------------|   cl-sdk-cpp C Core     |<----------+
                                 | (Downsampling Buffer)   |
                                 +-------------------------+
```

## 🔗 Official Documentation & Integration

For authoritative API references, visit the [Cortical Labs Documentation](https://docs.corticallabs.com/) and [Cortical Labs GitHub](https://github.com/Cortical-Labs).

## 🧠 Architecture Highlights

1. **C Core (`libclsdk`):** A C99 library managing socket connections, threading, and JSON serialization.
2. **C++ OOP Layer (`CorticalLabs.hpp`):** A C++17 wrapper offering RAII semantics and STL abstractions.
3. **Unreal Engine 5 Plugin (`CorticalLabs.uplugin`):** Native Blueprint Plugin support mapping the SDK into Blueprint nodes (`GetLatestSpikes`, `SendSensorData`) and C++ modules.
4. **Nim FFI (`cl_sdk.nim`):** Bindings for Nim integration.

## 🔌 Supported Integration Layers

- [x] Native C/C++ ABI
- [x] Nim FFI
- [x] Unreal Engine 5 (.uplugin)
- [ ] Python 3.12 (ctypes / pybind11) [Coming Soon]

## 🗺️ Roadmap

- **Multi-Dish Orchestrator for distributed biology:** Manage and cluster multiple HD-MEA dishes efficiently.
- **Hardware-Agnostic Encoder Templates:** Out-of-the-box sensor encoding for (LiDAR, Optical Flow, Spectrogram).

## 🏁 Getting Started

### 📋 Prerequisites
- CMake 3.10+
- A C++17 compatible compiler
- Cortical Labs API Key

### 🛠️ Build Instructions

```bash
mkdir build && cd build
cmake ..
make -j4
```

### 🚀 Quick Start (C++ Wrapper)

```cpp
#include "CorticalLabs.hpp"
#include <iostream>

using namespace cortical_labs;

int main() {
    try {
        DishConnection dish("wss://api.corticallabs.com/v1/dish", "YOUR_API_KEY");
        dish.connect();

        std::vector<float> sensor_data_x(59, 0.5f);
        std::vector<float> sensor_data_y(59, -0.2f);
        dish.sendSensorData(1005, sensor_data_x, sensor_data_y);

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