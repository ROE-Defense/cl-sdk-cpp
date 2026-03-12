#include "CorticalLabs/CorticalLabs.hpp"
#include <iostream>
#include <chrono>

using namespace cortical_labs;

int main() {
    try {
        DishConnection dish("simulator", "simulator");
        dish.connect();

        auto start = std::chrono::high_resolution_clock::now();
        
        uint64_t total_spikes = 0;
        int iterations = 10000;
        
        for (int i = 0; i < iterations; i++) {
            std::vector<float> sx(59, 0.0f);
            std::vector<float> sy(59, 0.0f);
            
            // Stimulate 10 specific channels heavily
            for(int j=0; j<10; j++) {
                sx[j] = 10.0f; 
            }
            
            dish.sendSensorData(i, sx, sy);
            auto spikes = dish.receiveSpikes(512);
            total_spikes += spikes.size();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        
        std::cout << "--- SIMULATOR STRESS TEST ---\n";
        std::cout << "Iterations: " << iterations << "\n";
        std::cout << "Total Spikes Emitted: " << total_spikes << "\n";
        std::cout << "Time elapsed: " << diff.count() << " s\n";
        std::cout << "Hz (Ticks/sec): " << iterations / diff.count() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
