#include "CorticalLabs/CorticalLabs.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace cortical_labs;

int main() {
    // Instantiate downsampling buffer using C API directly since wrapper lacks enable_downsampling
    cl_config config = {0};
    config.endpoint_url = "wss://api.corticallabs.com/v1/dish";
    config.api_key = "BENCHMARK_KEY";
    config.use_websockets = true;
    config.engine_tick_rate = 90;
    config.enable_downsampling = true;

    cl_context* ctx = cl_init(&config);
    if (!ctx) {
        std::cerr << "Failed to init SDK." << std::endl;
        return 1;
    }

    cl_connect(ctx);

    const int iterations = 1000000;
    std::vector<double> latencies;
    latencies.reserve(iterations);

    cl_spike_event spikes[10];

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulating "spike insertions" into the downsampling buffer
        cl_receive_spikes(ctx, spikes, 10);
        
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::micro> elapsed = end - start;
        latencies.push_back(elapsed.count());
    }

    // Process statistics
    std::sort(latencies.begin(), latencies.end());
    
    double sum = std::accumulate(latencies.begin(), latencies.end(), 0.0);
    double mean = sum / iterations;
    double p99 = latencies[std::min(static_cast<int>(iterations * 0.99), iterations - 1)];
    double max_lat = latencies.back();

    std::cout << "\n========================================================\n";
    std::cout << "   Cortical Labs HD-MEA Downsampling Buffer Benchmark   \n";
    std::cout << "========================================================\n";
    std::cout << "Insertions Simulated: " << iterations << "\n";
    std::cout << "Mean Latency:         " << mean << " us\n";
    std::cout << "99th Percentile:      " << p99 << " us\n";
    std::cout << "Max Latency:          " << max_lat << " us\n\n";

    // Create ASCII histogram
    std::cout << "Latency Distribution Histogram (Microseconds):\n";
    std::cout << "--------------------------------------------------------\n";
    
    const int num_bins = 20;
    double min_lat = latencies.front();
    // Exclude extreme outliers for histogram calculation if p99 is much smaller than max
    double hist_max = std::min(max_lat, p99 * 2.0); 
    if (hist_max <= min_lat) hist_max = min_lat + 1.0;
    
    double bin_width = (hist_max - min_lat) / num_bins;

    std::vector<int> bins(num_bins, 0);
    int overflow = 0;
    for (double lat : latencies) {
        if (lat >= hist_max) {
            overflow++;
            continue;
        }
        int bin = std::max(0, std::min(static_cast<int>((lat - min_lat) / bin_width), num_bins - 1));
        bins[bin]++;
    }

    int max_freq = *std::max_element(bins.begin(), bins.end());
    if (max_freq == 0) max_freq = 1;

    for (int i = 0; i < num_bins; ++i) {
        double bin_start = min_lat + i * bin_width;
        double bin_end = bin_start + bin_width;
        std::cout << std::fixed << std::setprecision(2) << std::setw(8) << bin_start << " - " 
                  << std::setw(8) << bin_end << " us | ";
        
        int bar_len = static_cast<int>(40.0 * bins[i] / max_freq);
        std::string bar(bar_len, '#');
        std::cout << std::setw(40) << std::left << bar << " " << bins[i] << "\n";
    }
    if (overflow > 0) {
        std::cout << std::setw(8) << " > " << std::setw(11) << hist_max << " us | " 
                  << std::setw(40) << std::left << std::string(1, '+') << " " << overflow << "\n";
    }
    std::cout << "========================================================\n";

    cl_destroy(ctx);

    return 0;
}