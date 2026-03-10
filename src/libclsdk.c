#include "cl_sdk.h"
#include "../third_party/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct cl_context {
    cl_config config;
    bool connected;
    // Internal socket handles and buffers would go here.
};

cl_context* cl_init(const cl_config* config) {
    if (!config || !config->endpoint_url) return NULL;
    cl_context* ctx = (cl_context*)malloc(sizeof(cl_context));
    if (!ctx) return NULL;
    
    ctx->config.api_key = config->api_key ? strdup(config->api_key) : NULL;
    ctx->config.endpoint_url = strdup(config->endpoint_url);
    ctx->config.use_websockets = config->use_websockets;
    ctx->connected = false;
    
    return ctx;
}

void cl_destroy(cl_context* ctx) {
    if (!ctx) return;
    if (ctx->config.api_key) free((void*)ctx->config.api_key);
    if (ctx->config.endpoint_url) free((void*)ctx->config.endpoint_url);
    free(ctx);
}

bool cl_connect(cl_context* ctx) {
    if (!ctx) return false;
    // Mock connection layer
    printf("[cl_sdk] 🔌 Initializing hardware bridge to %s (WebSockets: %s)...\n", 
            ctx->config.endpoint_url, ctx->config.use_websockets ? "YES" : "NO");
    ctx->connected = true;
    return true;
}

bool cl_send_optical_flow(cl_context* ctx, const cl_optical_flow* flow) {
    if (!ctx || !ctx->connected || !flow) return false;
    
    // Construct a generic JSON payload using cJSON to mock network layer transmission
    cJSON* root = cJSON_CreateObject();
    if (ctx->config.api_key) {
        cJSON_AddStringToObject(root, "api_key", ctx->config.api_key);
    }
    cJSON_AddNumberToObject(root, "timestamp", flow->timestamp);
    cJSON* flow_x_arr = cJSON_CreateFloatArray(flow->flow_x, CL_MAX_CHANNELS);
    cJSON* flow_y_arr = cJSON_CreateFloatArray(flow->flow_y, CL_MAX_CHANNELS);
    
    cJSON_AddItemToObject(root, "flow_x", flow_x_arr);
    cJSON_AddItemToObject(root, "flow_y", flow_y_arr);
    
    char* json_str = cJSON_PrintUnformatted(root);
    printf("[cl_sdk] 📡 TX Optical Flow Map: %s\n", json_str);
    
    free(json_str);
    cJSON_Delete(root);
    
    return true;
}

int cl_receive_spikes(cl_context* ctx, cl_spike_event* spikes_out, int max_spikes) {
    if (!ctx || !ctx->connected || !spikes_out || max_spikes <= 0) return 0;
    
    // Mock incoming JSON from WebSockets representing raw neural spikes
    const char* mock_json_rx = "{\"spikes\": [{\"ts\": 1005, \"ch\": 42, \"amp\": 1.2}, {\"ts\": 1008, \"ch\": 12, \"amp\": -0.8}, {\"ts\": 1012, \"ch\": 58, \"amp\": 2.4}]}";
    
    cJSON* root = cJSON_Parse(mock_json_rx);
    if (!root) return 0;
    
    cJSON* spikes_arr = cJSON_GetObjectItem(root, "spikes");
    int count = 0;
    
    if (cJSON_IsArray(spikes_arr)) {
        cJSON* spike = NULL;
        cJSON_ArrayForEach(spike, spikes_arr) {
            if (count >= max_spikes) break;
            
            cJSON* ts = cJSON_GetObjectItem(spike, "ts");
            cJSON* ch = cJSON_GetObjectItem(spike, "ch");
            cJSON* amp = cJSON_GetObjectItem(spike, "amp");
            
            if (cJSON_IsNumber(ts) && cJSON_IsNumber(ch) && cJSON_IsNumber(amp)) {
                spikes_out[count].timestamp = ts->valueint;
                spikes_out[count].channel_id = ch->valueint % CL_MAX_CHANNELS;
                spikes_out[count].amplitude = amp->valuedouble;
                count++;
            }
        }
    }
    
    cJSON_Delete(root);
    return count;
}