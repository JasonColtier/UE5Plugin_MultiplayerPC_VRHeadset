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
	void CreateSession(int32 NumberOfConnections, bool UseLan, TSoftObjectPtr<UWorld> MapToOpen);

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
	UPROPERTY(BlueprintAssignable)
	FCSOnCreateSessionComplete OnCreateSessionCompleteEvent;

	//called when the session started
	UPROPERTY(BlueprintAssignable)
	FCSOnStartSessionComplete OnStartSessionCompleteEvent;

	//delegate called when the session is destroyed !
	UPROPERTY(BlueprintAssignable)
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
	/**************
	  * Delegates and their handles to receive callback from OnlineSession
	 *************/

	//@formatter:off
	//delegate bound to the callback function i implemented
	FOnCreateSessionCompleteDelegate	CreateSessionCompleteDelegate;
	//handle to this delegate, to be able to remove it
	FDelegateHandle						CreateSessionCompleteDelegateHandle;
	
	FOnCreateSessionCompleteDelegate	DestroySessionCompleteDelegate;
	FDelegateHandle						DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate		StartSessionCompleteDelegate;
	FDelegateHandle						StartSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate		FindSessionsCompleteDelegate;
	FDelegateHandle						FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	FDelegateHandle						JoinSessionCompleteDelegateHandle;
	//@formatter:on

	//search session parameters
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	//creation session parameters
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;

	//the map that will be opened once the session is created with "listen"
	UPROPERTY(meta=(Untracked))
	TSoftObjectPtr<UWorld> MapToOpenOnSessionStarted;

	//the parameters used to find the session
	UPROPERTY()
	FFindSessionParameters FindSessionParameters;

	//timer handle to loop search, not looping 
	UPROPERTY()
	FTimerHandle HandleTimerLoopSearch;
};
