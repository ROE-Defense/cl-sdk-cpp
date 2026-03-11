#pragma once

#include "CorticalLabs/cl_sdk.h"
#include <string>
#include <vector>
#include <stdexcept>

namespace cortical_labs {

/// @brief Exception class for HD-MEA errors
class HDMEAException : public std::runtime_error {
public:
    /// @brief Constructor
    /// @param msg Error message
    explicit HDMEAException(const std::string& msg) : std::runtime_error(msg) {}
};

/// @brief Class managing connection to the Dish (HD-MEA)
class DishConnection {
private:
    cl_context* m_ctx;

public:
    /// @brief Target engine tick rates for telemetry downsampling
    enum class TickRate {
        HZ_30 = 30,
        HZ_60 = 60,
        HZ_90 = 90,
        HZ_120 = 120,
        HZ_144 = 144,
        UNLOCKED = 0
    };

    /// @brief Initialize a DishConnection instance
    /// @param endpoint Endpoint URL
    /// @param api_key API key for authentication
    /// @param use_websockets Whether to use websockets
    /// @param target_hz Target engine tick rate for telemetry downsampling
    DishConnection(const std::string& endpoint, const std::string& api_key = "", bool use_websockets = true, TickRate target_hz = TickRate::HZ_60);
    
    /// @brief Destructor
    ~DishConnection();

    // Prevent copies to safely manage the C-core pointer lifecycle
    DishConnection(const DishConnection&) = delete;
    DishConnection& operator=(const DishConnection&) = delete;

    /// @brief Establish the connection to the HD-MEA
    void connect();

    /// @brief Send sensor data to stimulate the dish
    /// @param timestamp The event timestamp
    /// @param data_x X-coordinates of sensor data
    /// @param data_y Y-coordinates of sensor data
    void sendSensorData(uint32_t timestamp, const std::vector<float>& data_x, const std::vector<float>& data_y);

    /// @brief Receive spike events from the HD-MEA
    /// @param max_spikes Maximum number of spikes to receive in this call
    /// @return A vector of spike events
    std::vector<cl_spike_event> receiveSpikes(int max_spikes = 100);

    /// @brief Start a high-performance UDP Spike Firehose listener for raw CL1 spike streams
    /// @param port UDP port to listen on
    /// @return Socket file descriptor
    static int listenUdpFirehose(int port);
};

} // namespace cortical_labs
