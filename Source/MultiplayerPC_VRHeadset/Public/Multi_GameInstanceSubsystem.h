// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Multi_GameInstanceSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnCreateSessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnDestroySessionComplete, bool, Successful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnStartSessionComplete, bool, Successful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCSOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful);
DECLARE_MULTICAST_DELEGATE_OneParam(FCSOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);


//a struct to hold the search parameters when searching in loop
USTRUCT()
struct FFindSessionParameters
{
	GENERATED_BODY()

	int32 MaxSearchResults;
	bool IsLANQuery;
	bool ShouldLoopSearch;
	int DelayBetweenSearch;
};


/**
 * 
 */
UCLASS()
class MULTIPLAYERPC_VRHEADSET_API UMulti_SessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMulti_SessionSubsystem();

	UFUNCTION(BlueprintCallable)
	void CreateSession(int32 NumberOfConnections,bool UseLan,TSoftObjectPtr<UWorld> MapToOpen);

	UFUNCTION()
	void StartSession();
	
	UFUNCTION(BlueprintCallable)
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void FindSessions(int32 MaxSearchResults, bool IsLANQuery, bool ShouldLoopSearch = true, int DelayBetweenSearch = 1);

	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);

	/**************
 	 * Delegates to bind to, they give feedback to the caller
	 *************/
	
	//delegate called when the session is created !
	FCSOnCreateSessionComplete OnCreateSessionCompleteEvent;

	//called when the session started
	FCSOnStartSessionComplete OnStartSessionCompleteEvent;

	//delegate called when the session is destroyed !
	FCSOnDestroySessionComplete OnDestroySessionCompleteEvent;

	//when the find session function finished
	FCSOnFindSessionsComplete OnFindSessionsCompleteEvent;

	//when we joined a game
	FCSOnJoinSessionComplete OnJoinGameSessionCompleteEvent;

protected:

	//function binded and called whend session is created
	void OnCreateSessionCompleted(FName SessionName, bool Successful);

	//function binded and called whend session is destroyed
	void OnDestroySessionCompleted(FName SessionName, bool Successful);

	void OnStartSessionCompleted(FName SessionName, bool Successful);

	void OnFindSessionsCompleted(bool Successful);

	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	bool TryTravelToCurrentSession();

private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	
	FOnCreateSessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	UPROPERTY(meta=(Untracked))
	TSoftObjectPtr<UWorld> MapToOpenOnSessionStarted;

	UPROPERTY()
	FFindSessionParameters FindSessionParameters;

	UPROPERTY()
	FTimerHandle HandleTimerLoopSearch;
};
