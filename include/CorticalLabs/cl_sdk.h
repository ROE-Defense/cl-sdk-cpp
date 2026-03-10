#ifndef CL_SDK_H
#define CL_SDK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @brief 59-channel architecture defined by Cortical Labs
#define CL_MAX_CHANNELS 59

/// @brief Opaque context structure for the HD-MEA connection
typedef struct cl_context cl_context;

/// @brief Configuration for connection
typedef struct {
    const char* api_key;
    const char* endpoint_url;
    bool use_websockets;
    int engine_tick_rate;      // e.g. 90 for VR, 144 for Unreal
    bool enable_downsampling; // Telemetry downsampling (25kHz -> Engine Tick Rate)
} cl_config;

/// @brief Represents a spike event on the HD-MEA
typedef struct {
    uint32_t timestamp;
    uint8_t channel_id; // 0 to 58
    float amplitude;
} cl_spike_event;

/// @brief Represents optical flow array for stimulation
typedef struct {
    uint32_t timestamp;
    float flow_x[CL_MAX_CHANNELS];
    float flow_y[CL_MAX_CHANNELS];
} cl_optical_flow;

/// @brief Initialize context (zero-overhead C-core)
/// @param config Configuration parameters
/// @return Pointer to the initialized cl_context
cl_context* cl_init(const cl_config* config);

/// @brief Free context
/// @param ctx Pointer to the context to be freed
void cl_destroy(cl_context* ctx);

/// @brief Connect to HD-MEA
/// @param ctx Pointer to the context
/// @return true if connection is successful, false otherwise
bool cl_connect(cl_context* ctx);

/// @brief Send optical flow array to stimulate dish
/// @param ctx Pointer to the context
/// @param flow Pointer to the optical flow structure
/// @return true if successfully sent, false otherwise
bool cl_send_optical_flow(cl_context* ctx, const cl_optical_flow* flow);

/// @brief Receive spikes (generic JSON mock parsing)
/// @param ctx Pointer to the context
/// @param spikes_out Array to hold the received spikes
/// @param max_spikes Maximum number of spikes to receive
/// @return Number of spikes received, up to max_spikes
int cl_receive_spikes(cl_context* ctx, cl_spike_event* spikes_out, int max_spikes);

#ifdef __cplusplus
}
#endif

#endif // CL_SDK_H
