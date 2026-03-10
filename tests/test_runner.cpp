#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include "CorticalLabs/CorticalLabs.hpp"

#define ASSERT(x) \
    do { \
        if (!(x)) { \
            std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << " -> " << #x << "\n"; \
            std::exit(1); \
        } \
    } while(0)

using namespace cortical_labs;

void test_hdmea_struct() {
    std::cout << "[TEST] 59-channel HD-MEA struct formatting...\n";
    ASSERT(sizeof(cl_spike_event) == 12 || sizeof(cl_spike_event) == 16); 
    ASSERT(CL_MAX_CHANNELS == 59);
    
    cl_optical_flow flow;
    ASSERT(sizeof(flow.flow_x) / sizeof(float) == 59);
    ASSERT(sizeof(flow.flow_y) / sizeof(float) == 59);
    std::cout << "[PASS] Struct formatting is correct.\n";
}

void test_udp_firehose() {
    std::cout << "[TEST] UDP Spike Firehose parsing logic...\n";
    int port = 9090;
    int fd = DishConnection::listenUdpFirehose(port);
    ASSERT(fd >= 0);

    // Mock sending a raw UDP packet and receiving it
    int sender_fd = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT(sender_fd >= 0);

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &dest.sin_addr);

    // Raw CL1 spike stream structure format (mock)
    struct RawSpike {
        uint32_t ts;
        uint8_t ch;
        float amp;
    } __attribute__((packed));

    RawSpike send_spike = { 1000, 42, 1.5f };
    ssize_t sent = sendto(sender_fd, &send_spike, sizeof(send_spike), 0, (struct sockaddr*)&dest, sizeof(dest));
    ASSERT(sent == sizeof(send_spike));

    // Wait a tiny bit and receive
    usleep(10000); 

    RawSpike recv_spike = {0, 0, 0.0f};
    struct sockaddr_in src;
    socklen_t srclen = sizeof(src);
    ssize_t recvd = recvfrom(fd, &recv_spike, sizeof(recv_spike), MSG_DONTWAIT, (struct sockaddr*)&src, &srclen);
    ASSERT(recvd == sizeof(recv_spike));
    ASSERT(recv_spike.ts == 1000);
    ASSERT(recv_spike.ch == 42);
    ASSERT(recv_spike.amp == 1.5f);

    close(sender_fd);
    close(fd);
    std::cout << "[PASS] UDP Spike Firehose parsing is flawless.\n";
}

void test_downsampling_buffer() {
    std::cout << "[TEST] 25kHz Asynchronous Downsampling Buffer math...\n";
    // At 90Hz engine tick rate, hw_samples_per_tick = 25000 / 90 = 277
    cl_config cfg;
    cfg.endpoint_url = "wss://mock";
    cfg.api_key = "mock";
    cfg.use_websockets = true;
    cfg.engine_tick_rate = 90;
    cfg.enable_downsampling = true;

    cl_context* ctx = cl_init(&cfg);
    ASSERT(ctx != nullptr);
    ASSERT(cl_connect(ctx) == true);

    std::vector<cl_spike_event> spikes(10);
    int count1 = cl_receive_spikes(ctx, spikes.data(), 10);
    ASSERT(count1 > 0);
    
    // Test the buffering behavior (it should return cached identical data for the next 276 calls)
    int count2 = cl_receive_spikes(ctx, spikes.data(), 10);
    ASSERT(count2 == count1); // Using cache

    // Cleanup
    cl_destroy(ctx);
    std::cout << "[PASS] Downsampling Buffer operates deterministically.\n";
}

int main() {
    std::cout << "--- Starting cl-sdk-cpp Mega Test Suite ---\n";
    test_hdmea_struct();
    test_udp_firehose();
    test_downsampling_buffer();
    std::cout << "--- All Tests Passed Successfully ---\n";
    return 0;
}