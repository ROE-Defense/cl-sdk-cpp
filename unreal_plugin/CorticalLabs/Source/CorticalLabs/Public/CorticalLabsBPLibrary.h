#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CorticalLabsBPLibrary.generated.h"

USTRUCT(BlueprintType)
struct FCorticalSpikeEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CorticalLabs")
    int32 Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CorticalLabs")
    int32 ChannelId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CorticalLabs")
    float Amplitude;
};

UCLASS()
class CORTICALLABS_API UCorticalLabsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "CorticalLabs")
    static TArray<FCorticalSpikeEvent> GetLatestSpikes(int32 MaxSpikes);

    UFUNCTION(BlueprintCallable, Category = "CorticalLabs")
    static bool SendOpticalFlow(const TArray<float>& FlowX, const TArray<float>& FlowY, int32 Timestamp);
};