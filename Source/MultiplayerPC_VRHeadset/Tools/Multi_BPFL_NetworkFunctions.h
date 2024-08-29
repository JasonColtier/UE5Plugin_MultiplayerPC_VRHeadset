// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Multi_BPFL_NetworkFunctions.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERPC_VRHEADSET_API UMulti_BPFL_NetworkFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Irwino")
	static void ServerTravel(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level);
};
