# ROE-Defense C++ SDK API Reference

Welcome to the API Reference for the C/C++ SDK for the Cortical Link platform. This SDK provides a comprehensive, low-latency wrapper around the Cortical Labs DishBrain REST and WebSocket interfaces. It is designed to allow high-performance integration of neuroscience arrays and biological computing substrates into C++ applications.

## Table of Contents
1. [Initialization and Configuration](#initialization)
2. [Session Management](#session-management)
3. [Stimulation & Sensory Input](#stimulation)
4. [Telemetry & Motor Output](#telemetry)
5. [Error Handling & Exceptions](#error-handling)

---

## Initialization and Configuration <a name="initialization"></a>

### `CorticalLabs::Client`
The primary entry point for establishing a connection to the biological compute cluster.

```cpp
#include <CorticalLabs.hpp>

CorticalLabs::Client client(
    const std::string& api_key,
    const std::string& endpoint = "wss://api.corticallabs.com/v1"
);
```

#### Methods:
- `bool Client::connect(uint32_t timeout_ms = 5000);`
  Establishes the WebSocket connection. Returns `true` on success.
- `void Client::disconnect();`
  Gracefully terminates the connection and frees associated hardware resources.
- `ClientStatus Client::getStatus() const;`
  Returns the current cluster operational state.

---

## Session Management <a name="session-management"></a>

Sessions define isolated compute environments within the biological cluster.

### `CorticalLabs::Session`

```cpp
CorticalLabs::Session session = client.createSession(SessionConfig config);
```

#### `SessionConfig` Struct:
- `uint32_t max_neurons`: Maximum allocated neuron count.
- `float learning_rate_multiplier`: Multiplier for long-term potentiation plasticity.
- `std::string topology`: Network topology preference (e.g., `"grid"`, `"sparse"`).

#### Methods:
- `std::string Session::getId() const;`
  Returns the UUID of the allocated session.
- `void Session::pause();`
  Temporarily halts neural simulation.
- `void Session::resume();`
  Resumes execution and plasticity updates.

---

## Stimulation & Sensory Input <a name="stimulation"></a>

Send spatial and temporal data patterns directly into the cultured neural network.

### `CorticalLabs::Stimulator`

```cpp
CorticalLabs::Stimulator stimulator(session);
```

#### Methods:
- `void Stimulator::sendSpikes(const std::vector<uint8_t>& electrodes, float intensity);`
  Sends rapid action potential stimulations to the mapped multi-electrode array (MEA). `intensity` must be between `0.0` and `1.0`.
- `void Stimulator::injectPattern(const std::vector<PatternData>& pattern_sequence);`
  Streams a time-series burst of structural input for complex state initialization.

---

## Telemetry & Motor Output <a name="telemetry"></a>

Subscribe to real-time spiking telemetry and processed outputs from the biological network.

### `CorticalLabs::TelemetryStream`

```cpp
CorticalLabs::TelemetryStream stream(session);
```

#### Methods:
- `void TelemetryStream::onSpike(std::function<void(uint8_t electrode, double timestamp)> callback);`
  Registers a callback for individual spike events.
- `void TelemetryStream::onBatch(std::function<void(const std::vector<SpikeEvent>&)> callback);`
  Registers a callback for batched spike data, optimized for high throughput.
- `std::vector<double> TelemetryStream::pollMotorVectors();`
  Retrieves aggregated vector transformations derived from the culture's current localized activity.

---

## Error Handling & Exceptions <a name="error-handling"></a>

The SDK utilizes standard C++ exceptions inherited from `std::runtime_error`.

- `CorticalLabs::ConnectionError`: Thrown when establishing a socket connection fails.
- `CorticalLabs::AuthenticationError`: Thrown for invalid API keys or expired JWT tokens.
- `CorticalLabs::ClusterBusyError`: Thrown when the biological hardware lacks sufficient resources to provision a new session.

*For further details, generate the full documentation using Doxygen: `doxygen Doxyfile`*
