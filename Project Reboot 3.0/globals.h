#pragma once

#include <atomic>

#include "inc.h"

namespace Globals
{
	extern inline bool bCreative = false;
	extern inline bool bGoingToPlayEvent = false;
	extern inline bool bEnableAGIDs = true;
	extern inline bool bNoMCP = false;
	extern inline bool bStarted = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bEnded = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bUptime = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bSentUptime = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bSentEnded = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bSentStarted = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bPlayerHasJoined = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bBuggyAsf = false; // i wanna fuck rep cuz rep is so hot
	extern inline bool bLogProcessEvent = false;

	// extern inline bool bLateGame = false;
	extern inline std::atomic<bool> bLateGame(false);

	extern inline bool bInfiniteMaterials = false;
	extern inline bool bInfiniteAmmo = false;

	extern inline int EmbedColor = 346642;



	extern inline std::string backendapikey = "Impregnated";
	extern inline std::string FullAddress = "http://167.114.124.103:3551/api/vbucks?apikey=" + backendapikey; // Set to your backend public ip and port
	extern inline std::string FullAddressXP = "http://167.114.124.103:3551/api/xp?apikey=" + backendapikey; // Set to your backend public ip and port
	extern inline std::string bWebhookMessage = "Ready Up Into Flipped";
	extern inline bool bHitReadyToStartMatch = false;
	extern inline bool bInitializedPlaylist = false;
	extern inline bool bStartedListening = false;
	extern inline bool bAutoRestart = true; // doesnt work fyi
	extern inline bool bFillVendingMachines = true;
	extern inline int AmountOfListens = 0; 
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