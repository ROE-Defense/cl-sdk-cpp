#ifndef CL_SDK_H
#define CL_SDK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 59-channel architecture defined by Cortical Labs
#define CL_MAX_CHANNELS 59

typedef struct cl_context cl_context;

// Configuration for connection
typedef struct {
    const char* api_key;
    const char* endpoint_url;
    bool use_websockets;
} cl_config;

// Represents a spike event on the HD-MEA
typedef struct {
    uint32_t timestamp;
    uint8_t channel_id; // 0 to 58
    float amplitude;
} cl_spike_event;

// Represents optical flow array for stimulation
typedef struct {
    uint32_t timestamp;
    float flow_x[CL_MAX_CHANNELS];
    float flow_y[CL_MAX_CHANNELS];
} cl_optical_flow;

// Initialize context (zero-overhead C-core)
cl_context* cl_init(const cl_config* config);

// Free context
void cl_destroy(cl_context* ctx);

// Connect to HD-MEA
bool cl_connect(cl_context* ctx);

// Send optical flow array to stimulate dish
bool cl_send_optical_flow(cl_context* ctx, const cl_optical_flow* flow);

// Receive spikes (generic JSON mock parsing)
// Returns number of spikes received, up to max_spikes
int cl_receive_spikes(cl_context* ctx, cl_spike_event* spikes_out, int max_spikes);

#ifdef __cplusplus
}
#endif

#endif // CL_SDK_H