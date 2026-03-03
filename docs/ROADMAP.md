# cl-sdk-cpp Roadmap

The development of `cl-sdk-cpp` is an active and ongoing effort to provide a robust C++ client for Cortical Labs HD-MEA environments. Below is our current roadmap.

## Near-Term Goals

1. **Multi-Dish Orchestrator (In Progress)**
   - Architect a thread-pooling system to connect to multiple Dish Brains simultaneously without blocking the main event loop.
   - Expand `cl_sdk.h` and `CorticalLabs.hpp` to handle `MultiDishConfig`.

2. **Python Bindings**
   - Provide a set of Python bindings using `pybind11` to allow researchers to interface with `cl-sdk-cpp` from Python scripts.
   - Support seamless NumPy arrays for spike data and optical flow.

3. **Unreal Engine Plugin**
   - Wrap the C++ SDK into an Unreal Engine 5 plugin.
   - Allow Blueprint visual scripting access to the HD-MEA, mapping neural responses directly to in-engine actors.

## Long-Term Vision

- **High-Performance Streaming**: Leverage ZeroMQ or WebRTC for sub-millisecond data delivery.
- **Auto-Calibration**: Self-tuning algorithms to optimize stimulation parameters dynamically.

*Note: This roadmap is subject to change based on community feedback and core engine updates.*
