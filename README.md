# 🧠 cl-sdk-cpp

[![Build Status](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/build.yml)
[![Security](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/codeql.yml/badge.svg)](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/codeql.yml)
[![Latest Release](https://img.shields.io/github/v/release/ROE-Defense/cl-sdk-cpp)](https://github.com/ROE-Defense/cl-sdk-cpp/releases/latest)
[![Live Demo](https://img.shields.io/badge/Live-WASM_Demo-success.svg)](https://ROE-Defense.github.io/cl-sdk-cpp/)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-C%2B%2B17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)

**⚡️ High-Performance C/C++ SDK for Synthetic Biological Intelligence (SBI) ⚡️**

`cl-sdk-cpp` is a systems-level C/C++ SDK engineered for robust interfacing with Cortical Labs' 59-channel High-Density Microelectrode Arrays (HD-MEA). Designed to serve as a low-latency, hardware-agnostic network bridge, it enables seamless coupling of Arbitrary Simulation Environments to the physical CL1 array. 

This SDK adheres strictly to the **Zero-Mock Doctrine**; it provides a transparent, unadulterated telemetry pipe rather than synthesizing data. It is not an artificial neural network mock; it is a conduit for live biological computation.

---

### 🔥 Core Feature: Asynchronous Telemetry Downsampling & Temporal Synchronization

Biological substrates operate on fundamentally different timescales than digital environments. The SDK resolves runtime bottlenecks via a fully detached threading model and an **Asynchronous Telemetry Downsampling Buffer**. This mechanism reliably scales raw 25kHz biological sampling rates down to update loops suitable for arbitrary client rendering (e.g., 90Hz or 144Hz loops) while maintaining rigorous temporal synchronization and preserving critical high-frequency spike potentials.

---

## 🌐 Live Interactive WebAssembly Demo

Experience the core telemetry pipeline natively in-browser. The high-performance downsampling engine has been cross-compiled to WebAssembly for seamless demonstration.

[👉 **Click here to view the live 59-channel telemetry demo.**](https://ROE-Defense.github.io/cl-sdk-cpp/)

---

## 🏗️ Dual-Engine Architecture

To facilitate deterministic SBI integration, the high-performance pipeline routes data from the physical biological substrate to the target environment as follows:

```text
    +-----------------------+ Hardware-Agnostic Sensor Data +-------------------------+
    | Arbitrary Simulation  |  (Pixels, Raycasts, Audio)  |     CL1 / Simulator     |
    |      Environment      | --------------------------> |    (59-Channel Array)   |
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

1. **C Core (`libclsdk`):** A strict C99 library managing socket connections, threading, and JSON serialization.
2. **C++ OOP Layer (`CorticalLabs.hpp`):** A C++17 wrapper offering standard RAII semantics and STL abstractions.
3. **Client Engine Plugin:** A native integration plugin mapping the SDK into visual scripting nodes and C++ modules for arbitrary simulation engines.
4. **Nim FFI (`cl_sdk.nim`):** Bindings for Nim integration.

## 🔌 Supported Integration Layers

- [x] Native C/C++ ABI
- [x] Nim FFI
- [x] Client Engine Plugin (.uplugin architecture compliant)
- [ ] Python 3.12 (ctypes / pybind11) [Coming Soon]

## 🗺️ Roadmap

- **Multi-Dish Orchestrator for distributed biology:** Manage and cluster multiple HD-MEA dishes efficiently.
- **Hardware-Agnostic Encoder Templates:** Out-of-the-box sensor encoding for varied data streams (LiDAR, spatial telemetry, raw spectrum).
- **Lock-Free Ring Buffers (SPSC) for zero-copy, zero-allocation microsecond memory pipelines.**
- **`.cl1_rec` Binary Replay System for deterministic, frame-by-frame biological session debugging.**
- **Native PyTorch / Jupyter Integration via `pybind11` for seamless AI research workflows.**

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