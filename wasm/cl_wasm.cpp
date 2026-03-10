#include <emscripten.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

extern "C" {

EMSCRIPTEN_KEEPALIVE
void init_sdk() {
    // Initialize random seed or any mock state for the demo
}

EMSCRIPTEN_KEEPALIVE
int process_telemetry() {
    // Mock processing step for the 25kHz -> 90Hz downsampling buffer
    return 1;
}

EMSCRIPTEN_KEEPALIVE
float get_channel_voltage(int channel) {
    // Simulate a high-frequency spike potential for the demo
    float r = (float)rand() / (float)RAND_MAX;
    if (r > 0.95f) { // 5% chance of a spike
        return r;
    }
    return 0.0f; // Baseline
}

}
