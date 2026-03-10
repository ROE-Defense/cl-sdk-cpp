#include <iostream>
#include <chrono>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "CorticalLabs/CorticalLabs.hpp"

using namespace cortical_labs;

void benchmark_json_deserialization() {
    std::cout << "Benchmarking JSON Deserialization (Mock API)...\n";
    cl_config cfg;
    cfg.endpoint_url = "wss://mock";
    cfg.api_key = "mock";
    cfg.use_websockets = true;
    cfg.engine_tick_rate = 144;
    cfg.enable_downsampling = false; // Disable to force JSON parsing on every call

    cl_context* ctx = cl_init(&cfg);
    cl_connect(ctx);

    std::vector<cl_spike_event> spikes(100);

    auto start = std::chrono::high_resolution_clock::now();
    int iterations = 10000;
    for (int i = 0; i < iterations; ++i) {
        cl_receive_spikes(ctx, spikes.data(), 100);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "JSON Parsing Time: " << duration / (double)iterations << " µs per iteration.\n";

    cl_destroy(ctx);
}

void benchmark_udp_parsing() {
    std::cout << "Benchmarking Raw UDP Spike Firehose...\n";
    
    int port = 9091;
    int fd = DishConnection::listenUdpFirehose(port);
    if (fd < 0) {
        std::cerr << "Failed to bind port\n";
        return;
    }
    
    int sender_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &dest.sin_addr);

    struct RawSpike {
        uint32_t ts;
        uint8_t ch;
        float amp;
    } __attribute__((packed));

    RawSpike send_spike = { 1000, 42, 1.5f };
    RawSpike recv_spike;
    struct sockaddr_in src;
    socklen_t srclen = sizeof(src);

    int iterations = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        sendto(sender_fd, &send_spike, sizeof(send_spike), 0, (struct sockaddr*)&dest, sizeof(dest));
        recvfrom(fd, &recv_spike, sizeof(recv_spike), 0, (struct sockaddr*)&src, &srclen);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Raw UDP Firehose Time: " << duration / (double)iterations << " µs per spike (Round Trip, Zero JSON Overhead).\n";

    close(sender_fd);
    close(fd);
}

int main() {
    std::cout << "--- cl-sdk-cpp High-Performance Benchmarks ---\n";
    benchmark_json_deserialization();
    benchmark_udp_parsing();
    std::cout << "--- Benchmarks Complete ---\n";
    return 0;
}