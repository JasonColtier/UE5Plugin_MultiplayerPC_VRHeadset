// Fill out your copyright notice in the Description page of Project Settings.


#include "Multi_GameInstanceSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Online/OnlineSessionNames.h"

UMulti_SessionSubsystem::UMulti_SessionSubsystem()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted)),
	  DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleted)),
	  FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsCompleted)),
	  JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionCompleted))
{
}

void UMulti_SessionSubsystem::CreateSession(int32 NumberOfConnections, bool UseLan)
{
	UE_LOGFMT(LogTemp, Log, "Creating session");

	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());

	if (!SessionInterface.IsValid())
	{
		OnCreateSessionCompleteEvent.Broadcast(false);
		return;
	}

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->NumPrivateConnections = 0;
	LastSessionSettings->NumPublicConnections = NumberOfConnections;
	LastSessionSettings->bAllowInvites = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = true;
	LastSessionSettings->bIsDedicated = false;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = UseLan;
	LastSessionSettings->bShouldAdvertise = true;


	UE_LOGFMT(LogTemp, Log, "current world : {0}",GetWorld()->GetCurrentLevel()->GetName());


	LastSessionSettings->Set(SETTING_MAPNAME, GetWorld()->GetCurrentLevel()->GetName(), EOnlineDataAdvertisementType::ViaOnlineService);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	if (!SessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		OnCreateSessionCompleteEvent.Broadcast(false);
	}
}

void UMulti_SessionSubsystem::OnCreateSessionCompleted(FName SessionName, bool Successful)
{
	UE_LOGFMT(LogTemp, Log, "Session {0} creation status : {1}", SessionName, Successful);

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	OnCreateSessionCompleteEvent.Broadcast(Successful);
}


void UMulti_SessionSubsystem::DestroySession()
{
	UE_LOGFMT(LogTemp, Log, "Destroying session");

	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());

	if (!SessionInterface.IsValid())
	{
		OnCreateSessionCompleteEvent.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		OnDestroySessionCompleteEvent.Broadcast(false);
	}
}

void UMulti_SessionSubsystem::OnDestroySessionCompleted(FName SessionName, bool Successful)
{
	UE_LOGFMT(LogTemp, Log, "Session {0} destroy status : {1}", SessionName, Successful);

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	OnDestroySessionCompleteEvent.Broadcast(Successful);
}

void UMulti_SessionSubsystem::FindSessions(int32 MaxSearchResults, bool IsLANQuery)
{
	UE_LOGFMT(LogTemp, Log, "Finding session");

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionsCompleteDelegateHandle =
		sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IsLANQuery;

	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);

		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMulti_SessionSubsystem::OnFindSessionsCompleted(bool Successful)
{
	UE_LOGFMT(LogTemp, Log, "Find session status {0}", Successful);

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	UE_LOGFMT(LogTemp, Log, "Number of sessions fond :  {0}", LastSessionSearch->SearchResults.Num());


	if (LastSessionSearch->SearchResults.Num() <= 0 || !Successful)
	{
		OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), Successful);
		return;
	}else
	{
		JoinGameSession(LastSessionSearch->SearchResults[0]);
	}
	
	OnFindSessionsCompleteEvent.Broadcast(LastSessionSearch->SearchResults, Successful);
}

void UMulti_SessionSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{
	UE_LOGFMT(LogTemp, Log, "Joining session");

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegateHandle =
		sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

		OnJoinGameSessionCompleteEvent.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMulti_SessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOGFMT(LogTemp, Log, "Session {0} join status : {1}", SessionName, LexToString(Result));

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	OnJoinGameSessionCompleteEvent.Broadcast(Result);

	if(Result == EOnJoinSessionCompleteResult::Success || Result == EOnJoinSessionCompleteResult::AlreadyInSession)
	{
		TryTravelToCurrentSession();
	}
}

bool UMulti_SessionSubsystem::TryTravelToCurrentSession()
{
	UE_LOGFMT(LogTemp, Log, "Traveling to current session");

	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		return false;
	}

	FString connectString;
	if (!sessionInterface->GetResolvedConnectString(NAME_GameSession, connectString))
	{
		return false;
	}

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->ClientTravel(connectString, TRAVEL_Absolute);
	return true;
}
