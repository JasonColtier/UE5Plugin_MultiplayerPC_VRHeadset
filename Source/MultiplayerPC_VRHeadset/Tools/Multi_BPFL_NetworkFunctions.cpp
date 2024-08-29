// Fill out your copyright notice in the Description page of Project Settings.


#include "Multi_BPFL_NetworkFunctions.h"

void UMulti_BPFL_NetworkFunctions::ServerTravel(const UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level)
{
	const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	UE_LOG(LogTemp, Log, TEXT("starting seamless travel to %s"), *LevelName.ToString() );
	WorldContextObject->GetWorld()->ServerTravel(LevelName.ToString());
}