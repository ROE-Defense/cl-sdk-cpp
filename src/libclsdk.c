#include "CorticalLabs/cl_sdk.h"
#include "../third_party/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct cl_context {
    cl_config config;
    bool connected;
    // Telemetry Downsampling Buffer
    cl_spike_event* cached_spikes;
    int cached_spike_count;
    uint32_t last_poll_time;
    // Internal socket handles and buffers would go here.
};

cl_context* cl_init(const cl_config* config) {
    if (!config || !config->endpoint_url) return NULL;
    cl_context* ctx = (cl_context*)malloc(sizeof(cl_context));
    if (!ctx) return NULL;
    
    ctx->config.api_key = config->api_key ? strdup(config->api_key) : NULL;
    ctx->config.endpoint_url = strdup(config->endpoint_url);
    ctx->config.use_websockets = config->use_websockets;
    ctx->config.engine_tick_rate = config->engine_tick_rate > 0 ? config->engine_tick_rate : 60;
    ctx->config.enable_downsampling = config->enable_downsampling;
    ctx->connected = false;
    ctx->cached_spikes = NULL;
    ctx->cached_spike_count = 0;
    ctx->last_poll_time = 0;
    
    return ctx;
}

void cl_destroy(cl_context* ctx) {
    if (!ctx) return;
    if (ctx->config.api_key) free((void*)ctx->config.api_key);
    if (ctx->config.endpoint_url) free((void*)ctx->config.endpoint_url);
    if (ctx->cached_spikes) free(ctx->cached_spikes);
    free(ctx);
}

bool cl_connect(cl_context* ctx) {
    if (!ctx) return false;
    // Mock connection layer
    printf("[cl_sdk] Initializing hardware bridge to %s (WebSockets: %s)...\n", 
            ctx->config.endpoint_url, ctx->config.use_websockets ? "YES" : "NO");
    ctx->connected = true;
    return true;
}

bool cl_send_sensor_data(cl_context* ctx, const cl_sensor_data* flow) {
    if (!ctx || !ctx->connected || !flow) return false;
    
    // Construct a generic JSON payload using cJSON to mock network layer transmission
    cJSON* root = cJSON_CreateObject();
    if (ctx->config.api_key) {
        cJSON_AddStringToObject(root, "api_key", ctx->config.api_key);
    }
    cJSON_AddNumberToObject(root, "timestamp", flow->timestamp);
    cJSON* data_x_arr = cJSON_CreateFloatArray(flow->data_x, CL_MAX_CHANNELS);
    cJSON* data_y_arr = cJSON_CreateFloatArray(flow->data_y, CL_MAX_CHANNELS);
    
    cJSON_AddItemToObject(root, "data_x", data_x_arr);
    cJSON_AddItemToObject(root, "data_y", data_y_arr);
    
    char* json_str = cJSON_PrintUnformatted(root);
    printf("[cl_sdk] TX Sensor Data Map: %s\n", json_str);
    
    free(json_str);
    cJSON_Delete(root);
    
    return true;
}

int cl_receive_spikes(cl_context* ctx, cl_spike_event* spikes_out, int max_spikes) {
    if (!ctx || !ctx->connected || !spikes_out || max_spikes <= 0) return 0;
    
    // Downsample telemetry buffer to match tick rate
    // Hardware samples at 25kHz, engine polls at configurable rate
    ctx->last_poll_time++;
    
    if (ctx->config.enable_downsampling) {
        double poll_interval_ms = 1000.0 / ctx->config.engine_tick_rate;
        int hw_samples_per_tick = (int)(25.0 * poll_interval_ms); // 25000 samples/sec = 25 samples/ms
        if (hw_samples_per_tick < 1) hw_samples_per_tick = 1;
        
        // Serve aggregated/buffered downsampled spikes without dropping critical potentials
        if (ctx->last_poll_time % hw_samples_per_tick != 0 && ctx->cached_spikes != NULL) {
            int serve_count = (ctx->cached_spike_count < max_spikes) ? ctx->cached_spike_count : max_spikes;
            for (int i = 0; i < serve_count; i++) {
                spikes_out[i] = ctx->cached_spikes[i];
            }
            return serve_count;
        }
    }
    
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
                spikes_out[count].timestamp = ts->valueint + ctx->last_poll_time;
                spikes_out[count].channel_id = ch->valueint % CL_MAX_CHANNELS;
                spikes_out[count].amplitude = amp->valuedouble;
                count++;
            }
        }
    }
    
    cJSON_Delete(root);
    
    // Update Telemetry Downsampling Buffer
    if (ctx->config.enable_downsampling) {
        if (ctx->cached_spikes) {
            free(ctx->cached_spikes);
        }
        ctx->cached_spikes = (cl_spike_event*)malloc(sizeof(cl_spike_event) * count);
        if (ctx->cached_spikes) {
            memcpy(ctx->cached_spikes, spikes_out, sizeof(cl_spike_event) * count);
            ctx->cached_spike_count = count;
        }
    }
    
    return count;
}

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int cl_listen_udp_firehose(int port) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}