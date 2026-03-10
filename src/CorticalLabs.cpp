#include "CorticalLabs/CorticalLabs.hpp"
#include <iostream>

namespace cortical_labs {

DishConnection::DishConnection(const std::string& endpoint, const std::string& api_key, bool use_websockets) {
    cl_config config;
    config.endpoint_url = endpoint.c_str();
    config.api_key = api_key.empty() ? nullptr : api_key.c_str();
    config.use_websockets = use_websockets;

    m_ctx = cl_init(&config);
    if (!m_ctx) {
        throw HDMEAException("Failed to initialize C-core context.");
    }
}

DishConnection::~DishConnection() {
    if (m_ctx) {
        cl_destroy(m_ctx);
    }
}

void DishConnection::connect() {
    if (!cl_connect(m_ctx)) {
        throw HDMEAException("Failed to connect to HD-MEA.");
    }
}

void DishConnection::sendOpticalFlow(uint32_t timestamp, const std::vector<float>& flow_x, const std::vector<float>& flow_y) {
    if (flow_x.size() > CL_MAX_CHANNELS || flow_y.size() > CL_MAX_CHANNELS) {
        throw HDMEAException("Channel count exceeds 59-channel architecture limit.");
    }

    cl_optical_flow flow = {};
    flow.timestamp = timestamp;
    
    // Copy data to the strict C-array bounds
    for (size_t i = 0; i < flow_x.size(); ++i) flow.flow_x[i] = flow_x[i];
    for (size_t i = 0; i < flow_y.size(); ++i) flow.flow_y[i] = flow_y[i];

    if (!cl_send_optical_flow(m_ctx, &flow)) {
        throw HDMEAException("Failed to send optical flow data.");
    }
}

std::vector<cl_spike_event> DishConnection::receiveSpikes(int max_spikes) {
    std::vector<cl_spike_event> spikes(max_spikes);
    int count = cl_receive_spikes(m_ctx, spikes.data(), max_spikes);
    spikes.resize(count);
    return spikes;
}

int DishConnection::listenUdpFirehose(int port) {
    int fd = cl_listen_udp_firehose(port);
    if (fd < 0) {
        throw HDMEAException("Failed to start UDP Spike Firehose listener.");
    }
    return fd;
}

} // namespace cortical_labs