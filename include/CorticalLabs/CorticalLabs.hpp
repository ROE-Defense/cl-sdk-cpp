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

/// @brief Configuration for connecting to multiple Dish endpoints simultaneously
struct MultiDishConfig {
    std::vector<std::string> endpoints;
    int max_threads_per_dish = 2;
    bool enable_orchestrator = true;
};

/// @brief Class managing connection to the Dish (HD-MEA)
class DishConnection {
private:
    cl_context* m_ctx;

public:
    /// @brief Initialize a DishConnection instance
    /// @param endpoint Endpoint URL
    /// @param api_key API key for authentication
    /// @param use_websockets Whether to use websockets
    DishConnection(const std::string& endpoint, const std::string& api_key = "", bool use_websockets = true);
    
    /// @brief Destructor
    ~DishConnection();

    // Prevent copies to safely manage the C-core pointer lifecycle
    DishConnection(const DishConnection&) = delete;
    DishConnection& operator=(const DishConnection&) = delete;

    /// @brief Establish the connection to the HD-MEA
    void connect();

    /// @brief Send optical flow data to stimulate the dish
    /// @param timestamp The event timestamp
    /// @param flow_x X-coordinates of optical flow
    /// @param flow_y Y-coordinates of optical flow
    void sendOpticalFlow(uint32_t timestamp, const std::vector<float>& flow_x, const std::vector<float>& flow_y);

    /// @brief Receive spike events from the HD-MEA
    /// @param max_spikes Maximum number of spikes to receive in this call
    /// @return A vector of spike events
    std::vector<cl_spike_event> receiveSpikes(int max_spikes = 100);
};

} // namespace cortical_labs
