#include "CorticalLabs/CorticalLabs.hpp"
#include <iostream>
#include <vector>

int main() {
    try {
        std::cout << "[example] Booting Cortical Labs C++ OOP Wrapper...\n";
        cortical_labs::DishConnection dish("wss://api.corticallabs.com/v1/dish", "live_key", true);
        dish.connect();
        
        // Simulating 59-channel sensor data map payload
        std::vector<float> data_x(59, 0.75f);
        std::vector<float> data_y(59, -0.4f);
        dish.sendSensorData(123456, data_x, data_y);
        
        // Polling raw spikes from HD-MEA
        auto spikes = dish.receiveSpikes(10);
        for (const auto& spike : spikes) {
            std::cout << "[example] RX Spike: CH=" << (int)spike.channel_id 
                      << " | AMP=" << spike.amplitude 
                      << " | TS=" << spike.timestamp << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}