#include "CorticalLabs.hpp"
#include <iostream>
#include <vector>

int main() {
    try {
        std::cout << "[example] Booting Cortical Labs C++ OOP Wrapper...\n";
        cortical_labs::DishConnection dish("wss://api.corticallabs.com/v1/dish", "live_key", true);
        dish.connect();
        
        // Simulating 59-channel flow map payload
        std::vector<float> flow_x(59, 0.75f);
        std::vector<float> flow_y(59, -0.4f);
        dish.sendOpticalFlow(123456, flow_x, flow_y);
        
        // Polling raw SNN spikes from HD-MEA
        auto spikes = dish.receiveSpikes(10);
        for (const auto& spike : spikes) {
            std::cout << "[example] ⚡ RX Spike: CH=" << (int)spike.channel_id 
                      << " | AMP=" << spike.amplitude 
                      << " | TS=" << spike.timestamp << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}