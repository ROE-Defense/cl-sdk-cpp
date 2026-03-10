#pragma once

#include "cl_sdk.h"
#include <string>
#include <vector>
#include <stdexcept>

namespace cortical_labs {

class HDMEAException : public std::runtime_error {
public:
    explicit HDMEAException(const std::string& msg) : std::runtime_error(msg) {}
};

class DishConnection {
private:
    cl_context* m_ctx;

public:
    DishConnection(const std::string& endpoint, const std::string& api_key = "", bool use_websockets = true);
    ~DishConnection();

    // Prevent copies to safely manage the C-core pointer lifecycle
    DishConnection(const DishConnection&) = delete;
    DishConnection& operator=(const DishConnection&) = delete;

    void connect();
    void sendOpticalFlow(uint32_t timestamp, const std::vector<float>& flow_x, const std::vector<float>& flow_y);
    std::vector<cl_spike_event> receiveSpikes(int max_spikes = 100);
};

} // namespace cortical_labs