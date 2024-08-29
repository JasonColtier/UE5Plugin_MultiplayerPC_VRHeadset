// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Multi_PlayerState.generated.h"

/**
 * 
 */

UENUM()
enum ENetworkIdentity
{
	NotInitialized,
	Instructor,
	VRPlayer
};

UCLASS()
class MULTIPLAYERPC_VRHEADSET_API AMulti_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// /**
	//  * Identity of the player
	//  */
	// UPROPERTY(BlueprintReadWrite,Replicated)
	// TEnumAsByte<ENetworkIdentity> Identity = NotInitialized;
	//
	// virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
