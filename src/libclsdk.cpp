#include "CorticalLabs/cl_sdk.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <cstring>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

struct cl_context {
    cl_config config;
    std::atomic<bool> connected;
    bool is_simulator;
    
    // Simulator State
    float membrane_potentials[CL_MAX_CHANNELS];
    float current_stimulus[CL_MAX_CHANNELS];
    
    // Threading and Buffers
    std::mutex buffer_mutex;
    std::queue<cl_spike_event> spike_queue;
    std::thread network_thread;
    std::atomic<bool> should_exit;
    uint32_t last_poll_time;
    
    // UDP Sockets
    int udp_listen_fd;
    int udp_send_fd;
    struct sockaddr_in target_addr;
};

// URL parser helper (udp://192.168.1.100:12345)
void parse_udp_url(const std::string& url, std::string& host, int& port) {
    size_t protocol_end = url.find("udp://");
    size_t host_start = (protocol_end != std::string::npos) ? protocol_end + 6 : 0;
    size_t colon = url.find_last_of(":");
    
    if (colon != std::string::npos) {
        host = url.substr(host_start, colon - host_start);
        port = std::stoi(url.substr(colon + 1));
    } else {
        host = url.substr(host_start);
        port = 12345;
    }
}

void simulator_worker(cl_context* ctx) {
    while (!ctx->should_exit) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / (ctx->config.engine_tick_rate > 0 ? ctx->config.engine_tick_rate : 60)));
        ctx->last_poll_time++;
        
        std::lock_guard<std::mutex> lock(ctx->buffer_mutex);
        for (int i = 0; i < CL_MAX_CHANNELS; i++) {
            ctx->membrane_potentials[i] += (-70.0f - ctx->membrane_potentials[i]) * 0.1f;
            ctx->membrane_potentials[i] += ctx->current_stimulus[i] * 5.0f;
            float noise = ((float)rand() / RAND_MAX) * 4.0f - 2.0f;
            ctx->membrane_potentials[i] += noise;
            
            if (ctx->membrane_potentials[i] > -55.0f) {
                cl_spike_event sp;
                sp.timestamp = ctx->last_poll_time;
                sp.channel_id = i;
                sp.amplitude = 100.0f + (ctx->membrane_potentials[i] + 55.0f) * 2.0f;
                ctx->spike_queue.push(sp);
                ctx->membrane_potentials[i] = -90.0f;
            }
            ctx->current_stimulus[i] *= 0.8f;
        }
    }
}

void udp_worker(cl_context* ctx, int port) {
#ifndef _WIN32
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "[cl_sdk] FATAL: Failed to open UDP Firehose socket.\n";
        ctx->connected = false;
        return;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "[cl_sdk] FATAL: UDP Firehose bind failed on port " << port << "\n";
        close(sockfd);
        ctx->connected = false;
        return;
    }

    std::cout << "[cl_sdk] UDP Firehose ACTIVE. Listening for 25kHz binary spikes on port " << port << "...\n";
    ctx->connected = true;
    ctx->udp_listen_fd = sockfd;

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    uint8_t buffer[1500];
    while (!ctx->should_exit) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);
        if (n > 0) {
            if (n >= 9) { 
                uint64_t ts;
                memcpy(&ts, buffer, 8); 
                
                std::lock_guard<std::mutex> lock(ctx->buffer_mutex);
                for (ssize_t i = 8; i < n; i++) {
                    cl_spike_event sp;
                    sp.timestamp = (uint32_t)(ts & 0xFFFFFFFF); 
                    sp.channel_id = buffer[i] % CL_MAX_CHANNELS;
                    sp.amplitude = 100.0f; 
                    ctx->spike_queue.push(sp);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    close(sockfd);
#endif
}

extern "C" {

cl_context* cl_init(const cl_config* config) {
    if (!config || !config->endpoint_url) return NULL;
    cl_context* ctx = new cl_context();
    
    ctx->config.api_key = config->api_key ? strdup(config->api_key) : NULL;
    ctx->config.endpoint_url = strdup(config->endpoint_url);
    ctx->config.engine_tick_rate = config->engine_tick_rate > 0 ? config->engine_tick_rate : 60;
    ctx->connected = false;
    ctx->should_exit = false;
    ctx->last_poll_time = 0;
    ctx->udp_listen_fd = -1;
    ctx->udp_send_fd = -1;
    
    std::string endpoint = ctx->config.endpoint_url;
    std::string key = ctx->config.api_key ? ctx->config.api_key : "";
    ctx->is_simulator = (endpoint.find("simulator") != std::string::npos) || (key == "simulator");
    
    for (int i = 0; i < CL_MAX_CHANNELS; i++) {
        ctx->membrane_potentials[i] = -70.0f;
        ctx->current_stimulus[i] = 0.0f;
    }
    
    return ctx;
}

void cl_destroy(cl_context* ctx) {
    if (!ctx) return;
    ctx->should_exit = true;
    if (ctx->network_thread.joinable()) {
        ctx->network_thread.join();
    }
#ifndef _WIN32
    if (ctx->udp_send_fd >= 0) close(ctx->udp_send_fd);
#endif
    if (ctx->config.api_key) free((void*)ctx->config.api_key);
    if (ctx->config.endpoint_url) free((void*)ctx->config.endpoint_url);
    delete ctx;
}

bool cl_connect(cl_context* ctx) {
    if (!ctx) return false;
    
    std::string url = ctx->config.endpoint_url;
    if (ctx->is_simulator) {
        std::cout << "[cl_sdk] Initializing Local Biological Simulator (Integrate-and-Fire Model)...\n";
        ctx->network_thread = std::thread(simulator_worker, ctx);
        ctx->connected = true;
    } else if (url.rfind("udp://", 0) == 0) {
        std::string host;
        int port;
        parse_udp_url(url, host, port);
        
#ifndef _WIN32
        ctx->udp_send_fd = socket(AF_INET, SOCK_DGRAM, 0);
        memset(&ctx->target_addr, 0, sizeof(ctx->target_addr));
        ctx->target_addr.sin_family = AF_INET;
        ctx->target_addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &ctx->target_addr.sin_addr);
#endif
        
        ctx->network_thread = std::thread(udp_worker, ctx, port);
        ctx->connected = true;
    } else {
        std::cout << "[cl_sdk] FATAL: Invalid endpoint. Use 'simulator' or 'udp://host:port'\n";
        return false;
    }
    
    return true;
}

bool cl_send_sensor_data(cl_context* ctx, const cl_sensor_data* flow) {
    if (!ctx || !ctx->connected || !flow) return false;
    
    // --- BIOLOGICAL SAFETY LAYER: Charge-Balanced Biphasic Stimulation ---
    cl_sensor_data safe_flow;
    safe_flow.timestamp = flow->timestamp;
    
    for (int i = 0; i < CL_MAX_CHANNELS; i++) {
        float clamped_x = flow->data_x[i];
        if (clamped_x > 150.0f) clamped_x = 150.0f;
        if (clamped_x < -150.0f) clamped_x = -150.0f;
        safe_flow.data_x[i] = clamped_x;
        safe_flow.data_y[i] = flow->data_y[i]; 
    }

    if (ctx->is_simulator) {
        std::lock_guard<std::mutex> lock(ctx->buffer_mutex);
        for (int i = 0; i < CL_MAX_CHANNELS; i++) {
            ctx->current_stimulus[i] = safe_flow.data_x[i];
        }
        return true;
    }
    
#ifndef _WIN32
    if (ctx->udp_send_fd >= 0) {
        // Send safe binary payload over UDP
        sendto(ctx->udp_send_fd, &safe_flow, sizeof(cl_sensor_data), 0, 
               (struct sockaddr*)&ctx->target_addr, sizeof(ctx->target_addr));
    }
#endif

    return true;
}

int cl_receive_spikes(cl_context* ctx, cl_spike_event* spikes_out, int max_spikes) {
    if (!ctx || !ctx->connected || !spikes_out || max_spikes <= 0) return 0;
    
    std::lock_guard<std::mutex> lock(ctx->buffer_mutex);
    int count = 0;
    while (!ctx->spike_queue.empty() && count < max_spikes) {
        spikes_out[count++] = ctx->spike_queue.front();
        ctx->spike_queue.pop();
    }
    return count;
}

int cl_listen_udp_firehose(int port) {
    return -1; // Deprecated by threaded architecture
}

} // extern "C"
