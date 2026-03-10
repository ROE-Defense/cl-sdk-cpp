#include "CorticalLabsBPLibrary.h"

// Note: You will need to statically link against the CL SDK here
// #include "CorticalLabs/cl_sdk.h"

TArray<FCorticalSpikeEvent> UCorticalLabsBPLibrary::GetLatestSpikes(int32 MaxSpikes)
{
    TArray<FCorticalSpikeEvent> Result;
    // Mock implementation for boilerplate
    // TODO: cl_receive_spikes(GlobalContext, RawSpikes, MaxSpikes)
    
    // Simulate a fake spike for BP testing
    FCorticalSpikeEvent MockSpike;
    MockSpike.Timestamp = 12345;
    MockSpike.ChannelId = 42;
    MockSpike.Amplitude = 1.0f;
    Result.Add(MockSpike);

    return Result;
}

bool UCorticalLabsBPLibrary::SendOpticalFlow(const TArray<float>& FlowX, const TArray<float>& FlowY, int32 Timestamp)
{
    // TODO: Convert TArray to raw arrays and cl_send_optical_flow(GlobalContext, ...)
    return true;
}