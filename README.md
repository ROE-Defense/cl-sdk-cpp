# 🧠 cl-sdk-cpp (Prototype)

[![Build Status](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/build.yml)
[![Security](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/codeql.yml/badge.svg)](https://github.com/ROE-Defense/cl-sdk-cpp/actions/workflows/codeql.yml)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-C%2B%2B17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)

**⚡️ High-Performance C/C++ SDK Architecture Prototype for Cortical Link ⚡️**

This repository is a **systems architecture prototype** designed to demonstrate how a high-performance C/C++ network bridge can eliminate Python GIL bottlenecks when interfacing with Cortical Labs' 25kHz HD-MEA arrays. 

> **NOTICE:** This SDK is currently an architectural mock-up featuring a biologically accurate Leaky Integrate-and-Fire (LIF) local simulator. It does *not* yet contain the proprietary TCP/UDP transport schemas required to interface with the physical CL1 hardware, as those protocols are currently closed-source. 

---

## 🎯 The Problem: The Python GIL Bottleneck

The official Cortical Labs `cl` Python SDK relies on `asyncio` to ingest massive 25kHz spike arrays. Due to Python's Global Interpreter Lock (GIL), the CPU spends substantial cycles unpacking JSON/Protobuf streams on the main thread. 

When researchers attempt to bind the biological tissue to high-refresh physics engines (like Unreal Engine 5 or headless reinforcement learning environments like ViZDoom), the Python network overhead starves the physics engine. This causes dropped frames, latency spikes, and temporal desynchronization—ultimately destroying the biological operant conditioning loop.

## 🛠️ The Solution: A Native C++ Proxy

`cl-sdk-cpp` solves this by entirely bypassing the Python GIL. 

1. **Detached Network Thread:** The SDK spawns an independent POSIX thread using `Boost.Beast` and `OpenSSL`.
2. **Native Telemetry Downsampling:** It ingests the 25kHz raw data stream in raw C++ memory, aggregating and downsampling it to the user's requested engine tick rate (e.g., 60Hz or 144Hz).
3. **Zero-Copy Handoff:** Through `pybind11` or Unreal Engine Blueprints, the SDK hands a clean, pre-calculated memory pointer back to the physics engine precisely when the render frame ticks.

By offloading the network storm to a C++ background thread, the primary engine can maintain a locked 60/144 FPS simulation with zero temporal drift.

---

## 🔬 Built-in Biological Simulator

To facilitate local pipeline testing without burning physical API credits, the SDK currently defaults to a highly-optimized **Leaky Integrate-and-Fire (LIF)** mathematical simulator running natively in the C-Core.

- Simulates resting membrane potentials (-70mV).
- Calculates temporal voltage decay and absolute refractory periods (-90mV).
- Injects stochastic Gaussian noise to replicate biological basal firing rates.
- Automatically clamps inbound stimulation arrays to safety thresholds (+/- 150mV) and enforces strict biphasic, zero-net-charge injection protocols to prevent software from theoretically "cooking" the tissue.

**Benchmark:** The local LIF simulator processes 10,000 frames of 59-channel stimulation and emits 100,000 biological spikes in **0.05 seconds** (191,000 Ticks/sec on a single thread).

## 🏗️ Architecture

```text
    +-----------------------+                                     +-------------------------+
    | Arbitrary Simulation  |     Raw Float Arrays (Stimulus)     |  Detached C++ SDK Core  |
    |      Environment      | ----------------------------------> | (Boost.Beast + OpenSSL) |
    |  (UE5 / Python RL)    |                                     |                         |
    |                       | <---------------------------------- |  (Telemetry Aggregator) |
    +-----------------------+   Zero-Copy Downsampled Array       +-------------------------+
                                                                             ^ |
                                                       25kHz Spike Avalanche | | Proprietary Binary Stream
                                                                             | v
                                                                  +-------------------------+
                                                                  |     Cortical Labs       |
                                                                  |   Physical CL1 Array    |
                                                                  +-------------------------+
```

## 🗺️ Roadmap & Integration Goals

- **Proprietary Schema Mapping:** Awaiting access to the official `cl` transport layer schemas to replace the LIF simulator with the live Melbourne network bindings.
- **Unreal Engine 5 Plugin:** Wrapping the `pybind11` architecture into an `.uplugin` for direct Blueprint visual scripting access.
- **Lock-Free Ring Buffers (SPSC):** Replacing mutexes with single-producer/single-consumer ring buffers for microsecond memory pipelines.

---
*Maintained under Roe Defense.*
