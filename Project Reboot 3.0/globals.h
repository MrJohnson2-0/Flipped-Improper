#pragma once

#include <atomic>

#include "inc.h"

namespace Globals
{
	extern inline bool bCreative = false;
	extern inline bool bGoingToPlayEvent = false;
	extern inline bool bEnableAGIDs = true;
	extern inline bool bNoMCP = false;
	extern inline bool bLogProcessEvent = true;
	// extern inline bool bLateGame = false;
	extern inline std::atomic<bool> bLateGame(false);
	extern inline bool bInfiniteMaterials = true;
	extern inline bool bInfiniteAmmo = false;

	extern inline bool bEnableRebootingFRFR = true;
	extern inline bool bEnded = false;
	extern inline bool bPlayerHasJoined = false;
	extern inline bool bBuggyAsf = false;
	extern inline bool bStarted = false;
	extern inline bool bAllowNoGuiMode = true;
	extern inline bool bIsGuiAlive = true;
	extern inline bool bMMEnabled = false;
	extern inline bool bStartedBus = false;
	extern inline bool bUptime = false;
	extern inline bool bSentEnded = false;
	extern inline bool bSentStarted = false;
	extern inline int UPTime = 0;
	extern inline int bMaxPlayers = 30;
	extern inline bool bSentUptime = false;
	extern inline bool bSentStartedBus = false;
	extern inline bool bHitReadyToStartMatch = false;
	extern inline bool bInitializedPlaylist = false;
	extern inline bool bStartedListening = false;
	extern inline bool bAutoRestart = false; // doesnt work fyi
	extern inline bool bFillVendingMachines = true;
	extern inline int AmountOfListens = 0; // TODO: Switch to this for LastNum
}

extern inline int NumToSubtractFromSquadId = 0; // I think 2?

extern inline std::string PlaylistName =
"/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo";
// "/Game/Athena/Playlists/gg/Playlist_Gg_Reverse.Playlist_Gg_Reverse";
// "/Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo";
// "/Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground";
// "/Game/Athena/Playlists/Carmine/Playlist_Carmine.Playlist_Carmine";
// "/Game/Athena/Playlists/Fill/Playlist_Fill_Solo.Playlist_Fill_Solo";
// "/Game/Athena/Playlists/Low/Playlist_Low_Solo.Playlist_Low_Solo";
// "/Game/Athena/Playlists/Bling/Playlist_Bling_Solo.Playlist_Bling_Solo";
// "/Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2";
// "/Game/Athena/Playlists/Ashton/Playlist_Ashton_Sm.Playlist_Ashton_Sm";
// "/Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab";

inline FString* GetRequestURL(UObject* Connection)
{
	if (Engine_Version <= 420)
		return (FString*)(__int64(Connection) + 432);
	if (Fortnite_Version >= 5 && Engine_Version < 424)
		return (FString*)(__int64(Connection) + 424);
	else if (Engine_Version >= 424)
		return (FString*)(__int64(Connection) + 440);

	return nullptr;
}