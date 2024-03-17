#pragma once

// TODO: Update ImGUI

#pragma comment(lib, "d3d9.lib")

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3d9.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx9.h>

#include <string>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_stdlib.h>
#include <vector>
#include <format>
#include <imgui/imgui_internal.h>
#include "discord.h"
#include <chrono> // for std::chrono
#include <thread> // for std::this_thread
#include <curl/curl.h>
#include <set>
#include <fstream>
#include <olectl.h>

#include "objectviewer.h"
#include "FortAthenaMutator_Disco.h"
#include "globals.h"
#include "Fonts/ruda-bold.h"
#include "Vector.h"
#include "reboot.h"
#include "FortGameModeAthena.h"
#include "UnrealString.h"
#include "KismetTextLibrary.h"
#include "KismetSystemLibrary.h"
#include "GameplayStatics.h"
#include "Text.h"
#include <Images/reboot_icon.h>
#include "FortGadgetItemDefinition.h"
#include "FortWeaponItemDefinition.h"
#include "events.h"
#include "FortAthenaMutator_Heist.h"
#include "BGA.h"
#include "vendingmachine.h"
#include "die.h"
#include "calendar.h"
#include "moderation.h"
using namespace std;

#define GAME_TAB 1
#define PLAYERS_TAB 2
#define GAMEMODE_TAB 3
#define THANOS_TAB 4
#define EVENT_TAB 5
#define CALENDAR_TAB 6
#define ZONE_TAB 7
#define DUMP_TAB 8
#define UNBAN_TAB 9
#define FUN_TAB 10
#define WEATHER_TAB 11
#define LATEGAME_TAB 12
#define DEVELOPER_TAB 13
#define DEBUGLOG_TAB 14
#define SETTINGS_TAB 15
#define CREDITS_TAB 16

#define MAIN_PLAYERTAB 1
#define INVENTORY_PLAYERTAB 2
#define LOADOUT_PLAYERTAB 4
#define FUN_PLAYERTAB 5

extern inline int StartReverseZonePhase = 7;
extern inline int EndReverseZonePhase = 5;
extern inline float StartingShield = 0;
extern inline bool bEnableReverseZone = false;
extern inline int AmountOfPlayersWhenBusStart = 0; 
extern inline bool bHandleDeath = true;
extern inline bool bUseCustomMap = false;
extern inline std::string CustomMapName = "";
extern inline int AmountToSubtractIndex = 1;
extern inline int SecondsUntilTravel = 50;
extern inline bool bSwitchedInitialLevel = false;
extern inline bool bStartedBus = false;
extern inline bool bIsInAutoRestart = false;
extern inline float AutoBusStartSeconds = 60;
extern inline int NumRequiredPlayersToStart = 2;
extern inline bool bDebugPrintLooting = false;
extern inline bool bDebugPrintFloorLoot = false;
extern inline bool bDebugPrintSwapping = false;
extern inline bool bEnableBotTick = true;
extern inline bool bZoneReversing = false;
extern inline bool bEnableCombinePickup = false;
extern inline int AmountOfBotsToSpawn = 0;
extern inline bool bEnableRebooting = true;
extern inline bool bEngineDebugLogs = false;

extern inline bool bShouldDestroyAllPlayerBuilds = false;
extern inline int AmountOfHealthSiphon = 50;



// THE BASE CODE IS FROM IMGUI GITHUB

static inline LPDIRECT3D9              g_pD3D = NULL;
static inline LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static inline D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
static inline bool CreateDeviceD3D(HWND hWnd);
static inline void CleanupDeviceD3D();
static inline void ResetDevice();
static inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static inline void SetIsLategame(bool Value)
{
	Globals::bLateGame.store(Value);
	StartingShield = Value ? 100 : 0;
}

static inline void Restart() // todo move?
{
	FString LevelA = Engine_Version < 424
		? L"open Athena_Terrain" : Engine_Version >= 500 ? Engine_Version >= 501
		? L"open Asteria_Terrain"
		: Globals::bCreative ? L"open Creative_NoApollo_Terrain"
		: L"open Artemis_Terrain"
		: Globals::bCreative ? L"open Creative_NoApollo_Terrain"
		: L"open Apollo_Terrain";

	static auto BeaconClass = FindObject<UClass>(L"/Script/FortniteGame.FortOnlineBeaconHost");
	auto AllFortBeacons = UGameplayStatics::GetAllActorsOfClass(GetWorld(), BeaconClass);

	for (int i = 0; i < AllFortBeacons.Num(); ++i)
	{
		AllFortBeacons.at(i)->K2_DestroyActor();
	}

	AllFortBeacons.Free();

	Globals::bInitializedPlaylist = false;
	Globals::bStartedListening = false;
	Globals::bHitReadyToStartMatch = false;
	Globals::bStartedBus = false;
	AmountOfRestarts++;

	LOG_INFO(LogDev, "Switching!");

	if (Fortnite_Version >= 3) // idk what ver
	{
		((AGameMode*)GetWorld()->GetGameMode())->RestartGame();
	}
	else
	{
		UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), LevelA, nullptr);
	}

	/*

	auto& LevelCollections = GetWorld()->Get<TArray<__int64>>("LevelCollections");
	int LevelCollectionSize = FindObject<UStruct>("/Script/Engine.LevelCollection")->GetPropertiesSize();

	*(UNetDriver**)(__int64(LevelCollections.AtPtr(0, LevelCollectionSize)) + 0x10) = nullptr;
	*(UNetDriver**)(__int64(LevelCollections.AtPtr(1, LevelCollectionSize)) + 0x10) = nullptr;

	*/

	// UGameplayStatics::OpenLevel(GetWorld(), UKismetStringLibrary::Conv_StringToName(LevelA), true, FString());
}

static inline std::string wstring_to_utf8(const std::wstring& str)
{
	if (str.empty()) return {};
	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
	std::string str_to(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &str_to[0], size_needed, nullptr, nullptr);
	return str_to;
}

static inline void InitFont()
{
	ImFontConfig FontConfig;
	FontConfig.FontDataOwnedByAtlas = false;
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)ruda_bold_data, sizeof(ruda_bold_data), 17.f, &FontConfig);
}

static inline void InitStyle()
{
	auto& mStyle = ImGui::GetStyle();
	mStyle.FramePadding = ImVec2(4, 2);
	mStyle.ItemSpacing = ImVec2(6, 2);
	mStyle.ItemInnerSpacing = ImVec2(6, 4);
	mStyle.Alpha = 0.95f;
	mStyle.WindowRounding = 4.0f;
	mStyle.FrameRounding = 2.0f;
	mStyle.IndentSpacing = 6.0f;
	mStyle.ItemInnerSpacing = ImVec2(2, 4);
	mStyle.ColumnsMinSpacing = 50.0f;
	mStyle.GrabMinSize = 14.0f;
	mStyle.GrabRounding = 16.0f;
	mStyle.ScrollbarSize = 12.0f;
	mStyle.ScrollbarRounding = 16.0f;

	ImGuiStyle& style = mStyle;
	style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);  // Text color
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);  // #121212
	style.Colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.00f);  // Transparent border
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);  // #393939
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.78f);  // Hovered frame background
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active frame background
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);  // Title background
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.14f, 0.17f, 0.75f);  // Collapsed title background
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active title background
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.14f, 0.17f, 0.47f);  // Menu bar background
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);  // Scrollbar background
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);  // Scrollbar grab
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.78f);  // Hovered scrollbar grab
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active scrollbar grab
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);  // Checkmark color
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);  // Slider grab color
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active slider grab color
	style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);  // Button color
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.86f);  // Hovered button color
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active button color
	style.Colors[ImGuiCol_Header] = ImVec4(0.56f, 0.56f, 0.56f, 0.76f);  // Header color
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.86f);  // Hovered header color
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active header color
	style.Colors[ImGuiCol_Separator] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);  // Separator color
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.78f);  // Hovered separator color
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active separator color
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);  // Resize grip color
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.78f);  // Hovered resize grip color
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Active resize grip color
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);  // Plot lines color
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Hovered plot lines color
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);  // Plot histogram color
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);  // Hovered plot histogram color
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.56f, 0.56f, 0.56f, 0.43f);  // Selected text background color
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.17f, 0.9f);
}

static inline void TextCentered(const std::string& text, bool bNewLine = true) {
	if (bNewLine)
		ImGui::NewLine();

	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text.c_str()).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	ImGui::TextWrapped(text.c_str());
	ImGui::PopTextWrapPos();
}

static inline bool ButtonCentered(const std::string& text, bool bNewLine = true) {
	if (bNewLine)
		ImGui::NewLine();

	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text.c_str()).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	auto res = ImGui::Button(text.c_str());
	ImGui::PopTextWrapPos();
	return res;
}

static inline void InputVector(const std::string& baseText, FVector* vec)
{
#ifdef ABOVE_S20
	ImGui::InputDouble((baseText + " X").c_str(), &vec->X);
	ImGui::InputDouble((baseText + " Y").c_str(), &vec->Y);
	ImGui::InputDouble((baseText + " Z").c_str(), &vec->Z);
#else
	ImGui::InputFloat((baseText + " X").c_str(), &vec->X);
	ImGui::InputFloat((baseText + " Y").c_str(), &vec->Y);
	ImGui::InputFloat((baseText + " Z").c_str(), &vec->Z);
#endif
}

static int Width = 640;
static int Height = 480;

static int Tab = 1;
static int PlayerTab = -1;
static bool bIsEditingInventory = false;
static bool bInformationTab = false;
static int playerTabTab = MAIN_PLAYERTAB;

static inline void StaticUI()
{
	if (IsRestartingSupported())
	{
		// ImGui::Checkbox("Auto Restart", &Globals::bAutoRestart);

		if (Globals::bAutoRestart)
		{
			ImGui::InputFloat(std::format("How long after {} players join the bus will start", NumRequiredPlayersToStart).c_str(), &AutoBusStartSeconds);
			ImGui::InputInt("Num Players required for bus auto timer", &NumRequiredPlayersToStart);
		}
	}

	ImGui::InputInt("Shield/Health for siphon", &AmountOfHealthSiphon);


ImGui::Checkbox("Log ProcessEvent", &Globals::bLogProcessEvent);
	// ImGui::InputInt("Amount of bots to spawn", &AmountOfBotsToSpawn);


	ImGui::Checkbox("Infinite Ammo", &Globals::bInfiniteAmmo);
	ImGui::Checkbox("Infinite Materials", &Globals::bInfiniteMaterials);

	ImGui::Checkbox("No MCP (Don't change unless you know what this is)", &Globals::bNoMCP);

	if (Addresses::ApplyGadgetData && Addresses::RemoveGadgetData && Engine_Version < 424)
	{
		ImGui::Checkbox("Enable AGIDs (Don't change unless you know what this is)", &Globals::bEnableAGIDs);
	}
}

static inline void MainTabs()
{
	// std::ofstream bannedStream(Moderation::Banning::GetFilePath());

	if (ImGui::BeginTabBar(""))
	{
		if (ImGui::BeginTabItem("Game"))
		{
			Tab = GAME_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		if (Globals::bStartedListening)
		{
			if (ImGui::BeginTabItem("Players"))
			{
				Tab = PLAYERS_TAB;
				ImGui::EndTabItem();
			} 
		}

		if (ImGui::BeginTabItem("Gamemode"))
		{
			Tab = GAMEMODE_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		// if (Events::HasEvent())
		if (Globals::bGoingToPlayEvent)
		{
			if (ImGui::BeginTabItem(("Event")))
			{
				Tab = EVENT_TAB;
				PlayerTab = -1;
				bInformationTab = false;
				ImGui::EndTabItem();
			}
		}

		if (ImGui::BeginTabItem("Calendar Events"))
		{
			Tab = CALENDAR_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Zone")))
		{
			Tab = ZONE_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Dump"))
		{
			Tab = DUMP_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Fun"))
		{
			Tab = FUN_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		if (Globals::bLateGame.load() && ImGui::BeginTabItem("Lategame"))
		{
			Tab = LATEGAME_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}



		/* if (ImGui::BeginTabItem(("Settings")))
		{
			Tab = SETTINGS_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		} */

		// maybe a Replication Stats for >3.3?

		if (ImGui::BeginTabItem(("Credits")))
		{
			Tab = CREDITS_TAB;
			PlayerTab = -1;
			bInformationTab = false;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
static size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* RES)
{
	if (!contents || !RES)
		return 0;

	((std::string*)RES)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t LogData(char* contents, size_t size, size_t nmemb, void* RES)
{
	if (!contents || !RES)
		return 0;

	//((std::string*)RES)->append((char*)contents, size * nmemb);
	LOG_DEBUG(LogDev, "Response: %s", contents);
	return size * nmemb;
}
static size_t write_callback(char* ptr, size_t size, size_t nmenb, void* userdata) {
	((std::string*)userdata)->append(ptr, size * nmenb);
	return size * nmenb;
}


class DefaultAPI
{
public:
	DefaultAPI()
	{
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();

		if (!curl)
		{
			return;
		}

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));
	}

	~DefaultAPI() {
		curl_global_cleanup();
		curl_easy_cleanup(curl);
	}

	FORCEINLINE bool PerformAction(const std::string& Endpoint, std::string* OutResponse = nullptr)
	{
		try
		{
			std::string URL = "http://3.71.114.115:2750/" + Endpoint;


			auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

			if (out1 != CURLE_OK)
			{
				LOG_ERROR(LogDev, "Curl setopt failed!\n");
				return false;
			}

			std::string TemporaryBuffer;
			if (OutResponse)
			{
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &TemporaryBuffer);
			}
			else {
				curl_easy_reset(curl);
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));
				auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
				

				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LogData);

				if (out1 != CURLE_OK)
				{
					LOG_ERROR(LogDev, "Curl setopt failed!");
					return false;
				}
			}

			auto out2 = curl_easy_perform(curl);
			//log_debug("%s\n", out2);

			if (out2 != CURLE_OK)
			{
				LOG_ERROR(LogDev, "Request failed!");
				return false;
			}

			if (OutResponse != nullptr) *OutResponse = TemporaryBuffer;
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	FORCEINLINE bool PerformActionMMS(const std::string& Endpoint, std::string* OutResponse = nullptr)
	{
		try
		{
			std::string URL = "https://3.71.114.115:2750/" + Endpoint;

			auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

			if (out1 != CURLE_OK)
			{
				LOG_ERROR(LogDev, "Curl setopt failed!");
				return false;
			}

			std::string TemporaryBuffer;
			if (OutResponse)
			{
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &TemporaryBuffer);
			}
			else {
				curl_easy_reset(curl);
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));
				auto out1 = curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

				if (out1 != CURLE_OK)
				{
					LOG_ERROR(LogDev, "Curl setopt failed!");
					return false;
				}
			}

			auto out2 = curl_easy_perform(curl);
			//log_debug("%s\n", out2);
			if (out2 != CURLE_OK)
			{
				LOG_ERROR(LogDev, "Request failed!");
				return false;
			}
		}
		catch (...)
		{
			return false;
		}

		return true;
	}
protected:
	CURL*curl;
};

namespace MatchmakerAPI {
	inline auto split = [](std::string s, std::string delimiter) {
		size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		std::string token;
		std::vector<std::string> res;

		while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
			token = s.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delim_len;
			res.push_back(token);
		}

		res.push_back(s.substr(pos_start));
		return res;
		};

	inline bool MarkServerOnlinev2(std::string REGION, std::string PlayerCap, std::string Port, std::string Session, std::string Playlist, std::string CustomCode) {
		std::string v = "19.10";
		std::string p = split(PlaylistName, ".")[1];
		std::string Endpoint = std::format("flipped/gs/create/session/{}/{}/{}/{}/{}{}{}", REGION, "3.71.114.115", Port, Playlist, "Flipped", p, v);

		std::string fullEndpoint = "http://3.71.114.115:2750/" + Endpoint;

		curl_global_init(CURL_GLOBAL_ALL);
		CURL* curl = curl_easy_init();
		if (!curl) {
			LOG_ERROR(LogDev, "Failed to initialize libcurl.");
			curl_global_cleanup();
		}

		//Set URL to API endpoint
		curl_easy_setopt(curl, CURLOPT_URL, fullEndpoint.c_str());


		// Set callback function for response body
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		// Create a buffer to store the response body
		std::string response_body;

		// Set the buffer as the user-defined data for the callback function
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

		// Perform HTTP request
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			//log_error("Failed to perform HTTP request: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("Failed to perform HTTP request for getting skin");
			return false;
		}

		// Check HTTP response code
		long response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (response_code >= 200 && response_code < 300) {
			// HTTP request successful, check response body
			curl_easy_cleanup(curl);
			curl_global_cleanup();

			//UptimeWebHook.send_message("HTTP request successful for getting skin" + response_body);
			return true;

		}
		else {
			// HTTP request failed
			//log_error("HTTP request failed with status code %ld.\n", response_code);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("HTTP request failed with status code " + std::to_string(response_code) + " for getting skin");
			return false;
		}

	}

	inline bool SetServerStatus(std::string status) {
		std::string v = "19.10";
		std::string p = split(PlaylistName, ".")[1];
		std::string Endpoint = std::format("flipped/gs/status/set/{}{}{}/{}", "Flipped", p, v, status);

		std::string fullEndpoint = "http://3.71.114.115:2750/" + Endpoint;

		curl_global_init(CURL_GLOBAL_ALL);
		CURL* curl = curl_easy_init();
		if (!curl) {
			LOG_ERROR(LogDev, "Failed to initialize libcurl.\n");
			curl_global_cleanup();
		}

		//Set URL to API endpoint
		curl_easy_setopt(curl, CURLOPT_URL, fullEndpoint.c_str());


		// Set callback function for response body
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		// Create a buffer to store the response body
		std::string response_body;

		// Set the buffer as the user-defined data for the callback function
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

		// Perform HTTP request
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			//log_error("Failed to perform HTTP request: %s\n", curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("Failed to perform HTTP request for getting skin");
			return false;
		}

		// Check HTTP response code
		long response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (response_code >= 200 && response_code < 300) {
			// HTTP request successful, check response body
			curl_easy_cleanup(curl);
			curl_global_cleanup();

			//UptimeWebHook.send_message("HTTP request successful for getting skin" + response_body);
			return true;

		}
		else {
			// HTTP request failed
			//log_error("HTTP request failed with status code %ld.\n", response_code);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			//UptimeWebHook.send_message("HTTP request failed with status code " + std::to_string(response_code) + " for getting skin");
			return false;
		}

	}


}
static inline void PlayerTabs()
{
	if (ImGui::BeginTabBar(""))
	{
		if (ImGui::BeginTabItem("Main"))
		{
			playerTabTab = MAIN_PLAYERTAB;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Inventory")))
		{
			playerTabTab = INVENTORY_PLAYERTAB;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Cosmetics")))
		{
			playerTabTab = LOADOUT_PLAYERTAB;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(("Fun")))
		{
			playerTabTab = FUN_PLAYERTAB;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
static inline DWORD WINAPI LateGameThread(LPVOID)
{
	float MaxTickRate = 30;

	auto GameMode = Cast<AFortGameModeAthena>(GetWorld()->GetGameMode());
	auto GameState = Cast<AFortGameStateAthena>(GameMode->GetGameState());

	auto GetAircrafts = [&]() -> std::vector<AActor*>
		{
			static auto AircraftsOffset = GameState->GetOffset("Aircrafts", false);
			std::vector<AActor*> Aircrafts;

			if (AircraftsOffset == -1)
			{
				// GameState->Aircraft

				static auto FortAthenaAircraftClass = FindObject<UClass>(L"/Script/FortniteGame.FortAthenaAircraft");
				auto AllAircrafts = UGameplayStatics::GetAllActorsOfClass(GetWorld(), FortAthenaAircraftClass);

				for (int i = 0; i < AllAircrafts.Num(); i++)
				{
					Aircrafts.push_back(AllAircrafts.at(i));
				}

				AllAircrafts.Free();
			}
			else
			{
				const auto& GameStateAircrafts = GameState->Get<TArray<AActor*>>(AircraftsOffset);

				for (int i = 0; i < GameStateAircrafts.Num(); i++)
				{
					Aircrafts.push_back(GameStateAircrafts.at(i));
				}
			}

			return Aircrafts;
		};

	GameMode->StartAircraftPhase();

	while (GetAircrafts().size() <= 0)
	{
		Sleep(1000 / MaxTickRate);
	}

	static auto SafeZoneLocationsOffset = GameMode->GetOffset("SafeZoneLocations");
	const TArray<FVector>& SafeZoneLocations = GameMode->Get<TArray<FVector>>(SafeZoneLocationsOffset);

	if (SafeZoneLocations.Num() < 4)
	{
		LOG_WARN(LogLateGame, "Unable to find SafeZoneLocation! Disabling lategame..");
		SetIsLategame(false);
		return 0;
	}

	const FVector ZoneCenterLocation = SafeZoneLocations.at(3);

	FVector LocationToStartAircraft = ZoneCenterLocation;
	LocationToStartAircraft.Z += 20000;

	auto Aircrafts = GetAircrafts();

	float DropStartTime = GameState->GetServerWorldTimeSeconds() + 5.f;
	float FlightSpeed = 0.0f;

	for (int i = 0; i < Aircrafts.size(); ++i)
	{
		auto CurrentAircraft = Aircrafts.at(i);
		CurrentAircraft->TeleportTo(LocationToStartAircraft, FRotator());

		static auto FlightInfoOffset = CurrentAircraft->GetOffset("FlightInfo", false);

		if (FlightInfoOffset == -1)
		{
			static auto FlightStartLocationOffset = CurrentAircraft->GetOffset("FlightStartLocation");
			static auto FlightSpeedOffset = CurrentAircraft->GetOffset("FlightSpeed");
			static auto DropStartTimeOffset = CurrentAircraft->GetOffset("DropStartTime");

			CurrentAircraft->Get<FVector>(FlightStartLocationOffset) = LocationToStartAircraft;
			CurrentAircraft->Get<float>(FlightSpeedOffset) = FlightSpeed;
			CurrentAircraft->Get<float>(DropStartTimeOffset) = DropStartTime;
		}
		else
		{
			auto FlightInfo = CurrentAircraft->GetPtr<FAircraftFlightInfo>(FlightInfoOffset);

			FlightInfo->GetFlightSpeed() = FlightSpeed;
			FlightInfo->GetFlightStartLocation() = LocationToStartAircraft;
			FlightInfo->GetTimeTillDropStart() = DropStartTime;
		}
	}

	while (GameState->GetGamePhase() != EAthenaGamePhase::Aircraft)
	{
		Sleep(1000 / MaxTickRate);
	}

	while (GameState->GetGamePhase() == EAthenaGamePhase::Aircraft)
	{
		Sleep(1000 / MaxTickRate);
	}

	static auto World_NetDriverOffset = GetWorld()->GetOffset("NetDriver");
	auto WorldNetDriver = GetWorld()->Get<UNetDriver*>(World_NetDriverOffset);
	auto& ClientConnections = WorldNetDriver->GetClientConnections();

	for (int z = 0; z < ClientConnections.Num(); z++)
	{
		auto ClientConnection = ClientConnections.at(z);
		auto FortPC = Cast<AFortPlayerController>(ClientConnection->GetPlayerController());

		if (!FortPC)
			continue;

		auto WorldInventory = FortPC->GetWorldInventory();

		if (!WorldInventory)
			continue;

		static auto WoodItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
		static auto StoneItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
		static auto MetalItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

		static auto HandCanon = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03");
		static auto Rifle = FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/RedDotAR/WID_Assault_RedDotAR_Athena_C.WID_Assault_RedDotAR_Athena_C");
		static auto Shotgun = FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_VR.WID_Shotgun_CoreBurst_Athena_VR")
			? FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_VR.WID_Shotgun_CoreBurst_Athena_VR")
			: FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_VR.WID_Shotgun_CoreBurst_Athena_VR");
		static auto SMG = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03")
			? FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03")
			: FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03");

		static auto MiniShields = FindObject<UFortItemDefinition>(L"/ParallelGameplay/Items/WestSausage/WID_WestSausage_Parallel_L_M.WID_WestSausage_Parallel_L_M");

		static auto Shells = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
		static auto ChugSplash = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Consumables/ChillBronco/Athena_ChillBronco.Athena_ChillBronco");
		static auto Medium = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
		static auto Light = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight");
		static auto Heavy = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");

		WorldInventory->AddItem(WoodItemData, nullptr, 500);
		WorldInventory->AddItem(StoneItemData, nullptr, 500);
		WorldInventory->AddItem(MetalItemData, nullptr, 500);
		WorldInventory->AddItem(Rifle, nullptr, 1);
		WorldInventory->AddItem(Shotgun, nullptr, 1);
		WorldInventory->AddItem(HandCanon, nullptr, 1);
		//WorldInventory->AddItem(SMG, nullptr, 1);
		WorldInventory->AddItem(MiniShields, nullptr, 3);
		WorldInventory->AddItem(ChugSplash, nullptr, 6);
		WorldInventory->AddItem(Shells, nullptr, 50);
		WorldInventory->AddItem(Medium, nullptr, 250);
		WorldInventory->AddItem(Light, nullptr, 250);
		WorldInventory->AddItem(Heavy, nullptr, 30);

		WorldInventory->Update();

		//FortPC->GetMyFortPawn()->SetShield(100);
	}

	static auto SafeZonesStartTimeOffset = GameState->GetOffset("SafeZonesStartTime");
	GameState->Get<float>(SafeZonesStartTimeOffset) = 0.001f;

	return 0;
}
static inline void NoGUI()
{
	Globals::bStarted = bStartedBus;
	bool bLoaded = true;
	bool uc = false;
	Globals::bUptime = true;
	auto GameState = Cast<AFortGameStateAthena>(((AFortGameMode*)GetWorld()->GetGameMode())->GetGameState());

	if (Globals::bMMEnabled)
	{

	}
	if (GameState->GetPlayersLeft() == 0 && Globals::bPlayerHasJoined)
	{
		Globals::bBuggyAsf = true;
		UptimeWebHook.send_embed("Servers restarting", "EU Server restarting, server has malfunctioned", 0x000000);
		Globals::bSentEnded = true;
		MatchmakerAPI::SetServerStatus("offline");
		std::system("taskkill /f /im FortniteClient-Win64-Shipping.exe");
	}

	if (!Globals::bSentStarted && Globals::bStarted)
	{
		UptimeWebHook.send_embed("Match Started", "EU Match has just started with " + std::to_string(GameState->GetPlayersLeft()) + " players", 0xff7700);
		MatchmakerAPI::SetServerStatus("offline");
		Globals::bSentStarted = true;
	}



	if (Globals::bEnded && !Globals::bSentEnded)
	{
		UptimeWebHook.send_embed("Servers Restarting", "EU Servers are restarting", 0x000000);
		Globals::bSentEnded = true;
	}

	if (!Globals::bSentUptime && Globals::bUptime)
	{
		UptimeWebHook.send_message("<@&1189492123016380436>");
		UptimeWebHook.send_embed("Servers started", "EU Server started", 0x72f289);
		auto split = [](std::string s, std::string delimiter) {
			size_t pos_start = 0, pos_end, delim_len = delimiter.length();
			std::string token;
			std::vector<std::string> res;

			while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
				token = s.substr(pos_start, pos_end - pos_start);
				pos_start = pos_end + delim_len;
				res.push_back(token);
			}

			res.push_back(s.substr(pos_start));
			return res;
			};
		std::string p = split(PlaylistName, ".")[1];
		MatchmakerAPI::MarkServerOnlinev2("EU", "30", "7777", p, "Playlist_DefaultSolo", "");
		MatchmakerAPI::SetServerStatus("online");
		Globals::bSentUptime = true;
	}
	if (GameState->GetPlayersLeft() >= 1)
	{
		Globals::bPlayerHasJoined = true;

	}
	if (GameState->GetPlayersLeft() >= Globals::bMaxPlayers)
	{
		MatchmakerAPI::SetServerStatus("offline");
	}
	if (Globals::bUptime && !uc)
	{
		uc = true;
		Sleep(1000);
		Globals::UPTime += 1;
		uc = false;
	}
	if (Globals::UPTime >= 1500)
	{
		Globals::bBuggyAsf = true;
		UptimeWebHook.send_embed("Servers restarting", "EU Server restarting, match has exceeded time limit", 0x000000);
		Globals::bSentEnded = true;
		MatchmakerAPI::SetServerStatus("offline");
		std::system("taskkill /f /im FortniteClient-Win64-Shipping.exe");
	}

	if (!Globals::bBuggyAsf && Globals::bPlayerHasJoined && GameState->GetPlayersLeft() <= 0 && GameState->GetGamePhase() == EAthenaGamePhase::Warmup)
	{
		Globals::bBuggyAsf = true;
		float SecondsToWait = 60;
		Sleep(SecondsToWait * 1000);
		if (!Globals::bSentEnded && GameState->GetPlayersLeft() == 0)
		{
			UptimeWebHook.send_status("The server bugged, server is restarting", "Europe", "Battle Royale Solos", GameState->GetPlayersLeft(), 0xf5902c);
			Globals::bSentEnded = true;
			MatchmakerAPI::SetServerStatus("offline");
			std::system("taskkill /f /im FortniteClient-Win64-Shipping.exe");
		}

	}
	else if (GameState->GetPlayersLeft() >= 1 && GameState->GetGamePhase() == EAthenaGamePhase::Warmup)
	{
		// i like reps penis (player joined lmfao game server stupid
	}
	if (GameState->GetPlayersLeft() >= 2 && GameState->GetGamePhase() == EAthenaGamePhase::Warmup)
	{
		float SecondsToWait = 60;
		Sleep(SecondsToWait * 1000);
		bStartedBus = true;

		Globals::bStarted = true;

		auto GameMode = (AFortGameModeAthena*)GetWorld()->GetGameMode();
		auto GameState = Cast<AFortGameStateAthena>(GameMode->GetGameState());

		AmountOfPlayersWhenBusStart = GameState->GetPlayersLeft();

		if (Globals::bLateGame.load())
		{
			CreateThread(0, 0, LateGameThread, 0, 0, 0);
		}
		else
		{
			GameMode->StartAircraftPhase();
			static auto World_NetDriverOffset = GetWorld()->GetOffset("NetDriver");
			auto WorldNetDriver = GetWorld()->Get<UNetDriver*>(World_NetDriverOffset);
			auto& ClientConnections = WorldNetDriver->GetClientConnections();

			for (int z = 0; z < ClientConnections.Num(); z++)
			{
				auto ClientConnection = ClientConnections.at(z);
				auto FortPC = Cast<AFortPlayerController>(ClientConnection->GetPlayerController());

				if (!FortPC)
					continue;

				auto WorldInventory = FortPC->GetWorldInventory();

				if (!WorldInventory)
					continue;

				static auto WoodItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
				static auto StoneItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
				static auto MetalItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

				static auto Shotgun = FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_SR.WID_Shotgun_CoreBurst_Athena_SR")
					? FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_SR.WID_Shotgun_CoreBurst_Athena_SR")
					: FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_SR.WID_Shotgun_CoreBurst_Athena_SR");

				static auto Shells = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");

				WorldInventory->AddItem(WoodItemData, nullptr, 500);
				WorldInventory->AddItem(StoneItemData, nullptr, 500);
				WorldInventory->AddItem(MetalItemData, nullptr, 500);
				WorldInventory->AddItem(Shotgun, nullptr, 1);
				WorldInventory->AddItem(Shells, nullptr, 50);

				WorldInventory->Update();
			}
		}
	}

}

static inline DWORD WINAPI NoGUIThread(LPVOID) // Completly fix the no gui issue, but gui will not work tho.
{
	while (true)
	{
		NoGUI();
	}

	return 0;
}
static inline void MainUI()
{
	bool bLoaded = true;
	bool uc = false;
	Globals::bUptime = true;
	Globals::bIsGuiAlive = true;
	auto GameState = Cast<AFortGameStateAthena>(((AFortGameMode*)GetWorld()->GetGameMode())->GetGameState());

	if (PlayerTab == -1)
	{
		MainTabs();

		if (Tab == GAME_TAB)
		{
			if (bLoaded)
			{
				StaticUI();
				if (!Globals::bSentStarted && Globals::bStarted)
				{
					UptimeWebHook.send_embed("Match Started", "EU Match has just started with " + std::to_string(GameState->GetPlayersLeft()) + " players", 0xff7700);
					MatchmakerAPI::SetServerStatus("offline");
					Globals::bSentStarted = true;
				}



						if (Globals::bEnded && !Globals::bSentEnded)
						{
							UptimeWebHook.send_embed("Servers Restarting", "EU Servers are restarting", 0x000000);
							Globals::bSentEnded = true;
						}

				if (!Globals::bSentUptime && Globals::bUptime)
				{
					UptimeWebHook.send_message("<@&1189492123016380436>");
					UptimeWebHook.send_embed("Servers started", "EU Server started", 0x72f289);
								auto split = [](std::string s, std::string delimiter) {
									size_t pos_start = 0, pos_end, delim_len = delimiter.length();
									std::string token;
									std::vector<std::string> res;

									while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
										token = s.substr(pos_start, pos_end - pos_start);
										pos_start = pos_end + delim_len;
										res.push_back(token);
									}

									res.push_back(s.substr(pos_start));
									return res;
									};
								std::string p = split(PlaylistName, ".")[1];
								MatchmakerAPI::MarkServerOnlinev2("EU", "30", "7777", p, "Playlist_DefaultSolo", "");
								MatchmakerAPI::SetServerStatus("online");
								Globals::bSentUptime = true;
				}
				if (GameState->GetPlayersLeft() >= 1)
				{
								Globals::bPlayerHasJoined = true;

				}
				if (GameState->GetPlayersLeft() >= Globals::bMaxPlayers)
				{
					MatchmakerAPI::SetServerStatus("offline");
				}
				if (Globals::bUptime && !uc)
				{
					uc = true;
					Sleep(1000);
					Globals::UPTime += 1;
					uc = false;
				}
				if (Globals::UPTime >= 1500)
				{
					Globals::bBuggyAsf = true;
					UptimeWebHook.send_embed("Servers restarting", "EU Server restarting, match has exceeded time limit", 0x000000);
					Globals::bSentEnded = true;
					MatchmakerAPI::SetServerStatus("offline");
					std::system("taskkill /f /im FortniteClient-Win64-Shipping.exe");
				}
				
				if (!Globals::bBuggyAsf && Globals::bPlayerHasJoined && GameState->GetPlayersLeft() <= 0 && GameState->GetGamePhase() == EAthenaGamePhase::Warmup)
				{
								Globals::bBuggyAsf = true;
								float SecondsToWait = 60;
								Sleep(SecondsToWait * 1000);
								if (!Globals::bSentEnded && GameState->GetPlayersLeft() == 0)
								{
									UptimeWebHook.send_status("The server bugged, server is restarting", "Europe", "Battle Royale Solos", GameState->GetPlayersLeft(), 0xf5902c);
									Globals::bSentEnded = true; 
									MatchmakerAPI::SetServerStatus("offline");
										std::system("taskkill /f /im FortniteClient-Win64-Shipping.exe");
								}
				
				}
				else if (GameState->GetPlayersLeft() >= 1 && GameState->GetGamePhase() == EAthenaGamePhase::Warmup)
				{
								// i like reps penis (player joined lmfao game server stupid
				}
				if (GameState->GetPlayersLeft() >= 2 && GameState->GetGamePhase() == EAthenaGamePhase::Warmup)
				{
					float SecondsToWait = 60;
					Sleep(SecondsToWait * 1000);
					bStartedBus = true;

					Globals::bStarted = true;

					auto GameMode = (AFortGameModeAthena*)GetWorld()->GetGameMode();
					auto GameState = Cast<AFortGameStateAthena>(GameMode->GetGameState());

					AmountOfPlayersWhenBusStart = GameState->GetPlayersLeft();

					if (Globals::bLateGame.load())
					{
						CreateThread(0, 0, LateGameThread, 0, 0, 0);
					}
					else
					{
						GameMode->StartAircraftPhase();
						static auto World_NetDriverOffset = GetWorld()->GetOffset("NetDriver");
						auto WorldNetDriver = GetWorld()->Get<UNetDriver*>(World_NetDriverOffset);
						auto& ClientConnections = WorldNetDriver->GetClientConnections();

						for (int z = 0; z < ClientConnections.Num(); z++)
						{
							auto ClientConnection = ClientConnections.at(z);
							auto FortPC = Cast<AFortPlayerController>(ClientConnection->GetPlayerController());

							if (!FortPC)
								continue;

							auto WorldInventory = FortPC->GetWorldInventory();

							if (!WorldInventory)
								continue;

							static auto WoodItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
							static auto StoneItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
							static auto MetalItemData = FindObject<UFortItemDefinition>(L"/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

							static auto Shotgun = FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_SR.WID_Shotgun_CoreBurst_Athena_SR")
								? FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_SR.WID_Shotgun_CoreBurst_Athena_SR")
								: FindObject<UFortItemDefinition>(L"/FlipperGameplay/Items/Weapons/BurstShotgun/WID_Shotgun_CoreBurst_Athena_SR.WID_Shotgun_CoreBurst_Athena_SR");

							static auto Shells = FindObject<UFortItemDefinition>(L"/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");

							WorldInventory->AddItem(WoodItemData, nullptr, 500);
							WorldInventory->AddItem(StoneItemData, nullptr, 500);
							WorldInventory->AddItem(MetalItemData, nullptr, 500);
							WorldInventory->AddItem(Shotgun, nullptr, 1);
							WorldInventory->AddItem(Shells, nullptr, 50);

							WorldInventory->Update();
						}
					}
				}
				if (!Globals::bStartedBus)
				{
					bool bWillBeLategame = Globals::bLateGame.load();
					ImGui::Checkbox("Lategame", &bWillBeLategame);
					SetIsLategame(bWillBeLategame);
				}

				ImGui::Text(std::format("Joinable {}", Globals::bStartedListening).c_str());

				static std::string ConsoleCommand;

				ImGui::InputText("Console command", &ConsoleCommand);

				if (ImGui::Button("Execute console command"))
				{
					auto wstr = std::wstring(ConsoleCommand.begin(), ConsoleCommand.end());

					auto aa = wstr.c_str();
					FString cmd = aa;

					UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), cmd, nullptr);
				}

				if (!bIsInAutoRestart && Engine_Version < 424 && ImGui::Button("Restart"))
				{
					if (Engine_Version < 424)
					{
						Restart();
						LOG_INFO(LogGame, "Restarting!");
					}
					else
					{
						LOG_ERROR(LogGame, "Restarting is not supported on chapter 2 and above!");
					}
				}

				if (!Globals::bStartedBus)
				{
					if (Globals::bLateGame.load() || Fortnite_Version >= 11)
					{
						if (ImGui::Button("Start Bus"))
						{
							Globals::bStartedBus = true;

							auto GameMode = (AFortGameModeAthena*)GetWorld()->GetGameMode();
							auto GameState = Cast<AFortGameStateAthena>(GameMode->GetGameState());

							AmountOfPlayersWhenBusStart = GameState->GetPlayersLeft();

							if (Globals::bLateGame.load())
							{
								CreateThread(0, 0, LateGameThread, 0, 0, 0);
							}
							else
							{
								static auto WarmupCountdownEndTimeOffset = GameState->GetOffset("WarmupCountdownEndTime");
								// GameState->Get<float>(WarmupCountdownEndTimeOffset) = UGameplayStatics::GetTimeSeconds(GetWorld()) + 10;

								float TimeSeconds = GameState->GetServerWorldTimeSeconds(); // UGameplayStatics::GetTimeSeconds(GetWorld());
								float Duration = 10;
								float EarlyDuration = Duration;

								static auto WarmupCountdownStartTimeOffset = GameState->GetOffset("WarmupCountdownStartTime");
								static auto WarmupCountdownDurationOffset = GameMode->GetOffset("WarmupCountdownDuration");
								static auto WarmupEarlyCountdownDurationOffset = GameMode->GetOffset("WarmupEarlyCountdownDuration");

								GameState->Get<float>(WarmupCountdownEndTimeOffset) = TimeSeconds + Duration;
								GameMode->Get<float>(WarmupCountdownDurationOffset) = Duration;

								// GameState->Get<float>(WarmupCountdownStartTimeOffset) = TimeSeconds;
								GameMode->Get<float>(WarmupEarlyCountdownDurationOffset) = EarlyDuration;
							}
						}
					}
					else
					{
						if (ImGui::Button("Start Bus Countdown"))
						{
							Globals::bStartedBus = true;

							auto GameMode = (AFortGameMode*)GetWorld()->GetGameMode();
							auto GameState = Cast<AFortGameStateAthena>(GameMode->GetGameState());

							AmountOfPlayersWhenBusStart = GameState->GetPlayersLeft(); // scuffed!!!!

							if (Fortnite_Version == 1.11)
							{
								static auto OverrideBattleBusSkin = FindObject(L"/Game/Athena/Items/Cosmetics/BattleBuses/BBID_WinterBus.BBID_WinterBus");
								LOG_INFO(LogDev, "OverrideBattleBusSkin: {}", __int64(OverrideBattleBusSkin));

								if (OverrideBattleBusSkin)
								{
									static auto AssetManagerOffset = GetEngine()->GetOffset("AssetManager");
									auto AssetManager = GetEngine()->Get(AssetManagerOffset);

									if (AssetManager)
									{
										static auto AthenaGameDataOffset = AssetManager->GetOffset("AthenaGameData");
										auto AthenaGameData = AssetManager->Get(AthenaGameDataOffset);

										if (AthenaGameData)
										{
											static auto DefaultBattleBusSkinOffset = AthenaGameData->GetOffset("DefaultBattleBusSkin");
											AthenaGameData->Get(DefaultBattleBusSkinOffset) = OverrideBattleBusSkin;
										}
									}

									static auto DefaultBattleBusOffset = GameState->GetOffset("DefaultBattleBus");
									GameState->Get(DefaultBattleBusOffset) = OverrideBattleBusSkin;

									static auto FortAthenaAircraftClass = FindObject<UClass>("/Script/FortniteGame.FortAthenaAircraft");
									auto AllAircrafts = UGameplayStatics::GetAllActorsOfClass(GetWorld(), FortAthenaAircraftClass);

									for (int i = 0; i < AllAircrafts.Num(); i++)
									{
										auto Aircraft = AllAircrafts.at(i);

										static auto DefaultBusSkinOffset = Aircraft->GetOffset("DefaultBusSkin");
										Aircraft->Get(DefaultBusSkinOffset) = OverrideBattleBusSkin;

										static auto SpawnedCosmeticActorOffset = Aircraft->GetOffset("SpawnedCosmeticActor");
										auto SpawnedCosmeticActor = Aircraft->Get<AActor*>(SpawnedCosmeticActorOffset);

										if (SpawnedCosmeticActor)
										{
											static auto ActiveSkinOffset = SpawnedCosmeticActor->GetOffset("ActiveSkin");
											SpawnedCosmeticActor->Get(ActiveSkinOffset) = OverrideBattleBusSkin;
										}
									}
								}
							}

							static auto WarmupCountdownEndTimeOffset = GameState->GetOffset("WarmupCountdownEndTime");
							// GameState->Get<float>(WarmupCountdownEndTimeOffset) = UGameplayStatics::GetTimeSeconds(GetWorld()) + 10;

							float TimeSeconds = GameState->GetServerWorldTimeSeconds(); // UGameplayStatics::GetTimeSeconds(GetWorld());
							float Duration = 10;
							float EarlyDuration = Duration;

							static auto WarmupCountdownStartTimeOffset = GameState->GetOffset("WarmupCountdownStartTime");
							static auto WarmupCountdownDurationOffset = GameMode->GetOffset("WarmupCountdownDuration");
							static auto WarmupEarlyCountdownDurationOffset = GameMode->GetOffset("WarmupEarlyCountdownDuration");

							GameState->Get<float>(WarmupCountdownEndTimeOffset) = TimeSeconds + Duration;
							GameMode->Get<float>(WarmupCountdownDurationOffset) = Duration;

							// GameState->Get<float>(WarmupCountdownStartTimeOffset) = TimeSeconds;
							GameMode->Get<float>(WarmupEarlyCountdownDurationOffset) = EarlyDuration;
						}
					}
				}
			}
		}

		else if (Tab == PLAYERS_TAB)
		{
			vector<pair<UObject*, UObject*>> AllSigmaControllers;
			auto world = GetWorld();

			if (world)
			{
				static auto NetDriverOffset = world->GetOffset("NetDriver");
				auto NetDriver = *(UObject**)(__int64(world) + NetDriverOffset);

				if (NetDriver)
				{
					static auto ClientConnectionsOffset = NetDriver->GetOffset("ClientConnections");
					auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

					if (ClientConnections)
					{
						for (int i = 0; i < ClientConnections->Num(); i++)
						{
							auto Connection = ClientConnections->At(i);

							if (!Connection)
								continue;

							static auto Connection_PlayerControllerOffset = Connection->GetOffset("PlayerController");
							auto CurrentController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);

							if (CurrentController)
							{
								AllSigmaControllers.push_back({ CurrentController, Connection });
							}
						}
					}

					ImGui::Text(("Players Connected: " + std::to_string(AllSigmaControllers.size())).c_str());

					for (int i = 0; i < AllSigmaControllers.size(); i++)
					{
						auto& CurrentPair = AllSigmaControllers.at(i);
						auto CurrentPlayerState = SkibidiToilet::GetPlayerStateFromController(CurrentPair.first);

						if (!CurrentPlayerState)
						{
							std::cout << "tf!\n";
							continue;
						}

						FString NameFStr;

						/* static auto GetPlayerName = FindObject<UFunction>("/Script/Engine.PlayerState.GetPlayerName");
						// static auto GetPlayerName = FindObject<UFunction>("/Script/FortniteGame.FortPlayerStateZone.GetPlayerNameForStreaming");
						CurrentPlayerState->ProcessEvent(GetPlayerName, &NameFStr);

						const wchar_t* NameWCStr = NameFStr.Data.Data;
						std::wstring NameWStr = std::wstring(NameWCStr);
						std::string Name = NameFStr.ToString(); // std::string(NameWStr.begin(), NameWStr.end());

						auto NameCStr = Name.c_str(); */

						auto Connection = CurrentPair.second;
						auto RequestURL = *GetRequestURL(Connection);

						if (RequestURL.Data.Data && RequestURL.Data.ArrayNum)
						{
							auto RequestURLStr = RequestURL.ToString();

							std::size_t pos = RequestURLStr.find("Name=");

							if (pos != std::string::npos) {
								std::size_t end_pos = RequestURLStr.find('?', pos);

								if (end_pos != std::string::npos)
									RequestURLStr = RequestURLStr.substr(pos + 5, end_pos - pos - 5);
							}

							auto RequestURLCStr = RequestURLStr.c_str();

							if (ImGui::Button(RequestURLCStr))
							{
								std::cout << "RequestURLStr: " << RequestURLStr << '\n';
								std::cout << "wtf! " << i << '\n';
								// std::cout << "Name: " << Name << '\n';
								PlayerTab = i;
							}
						}
					}
				}
			}
		}

		else if (Tab == EVENT_TAB)
		{
			if (ImGui::Button(std::format("Start {}", GetEventName()).c_str()))
			{
				StartEvent();
			}

			if (Fortnite_Version == 18.40)
			{
				if (ImGui::Button("Remove Storm Effect"))
				{
					auto ClientConnections = GetWorld()->GetNetDriver()->GetClientConnections();

					for (int i = 0; i < ClientConnections.Num(); i++)
					{
						auto CurrentController = (AFortPlayerControllerAthena*)ClientConnections.At(i)->GetPlayerController();

						static auto StormEffectClass = FindObject<UClass>(L"/Game/Athena/SafeZone/GE_OutsideSafeZoneDamage.GE_OutsideSafeZoneDamage_C");
						auto PlayerState = CurrentController->GetPlayerStateAthena();
						PlayerState->GetAbilitySystemComponent()->RemoveActiveGameplayEffectBySourceEffect(StormEffectClass, 1, PlayerState->GetAbilitySystemComponent());
					}
				}
			}

			if (Fortnite_Version == 8.51)
			{
				if (ImGui::Button("Unvault DrumGun"))
				{
					static auto SetUnvaultItemNameFn = FindObject<UFunction>(L"/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.SetUnvaultItemName");
					auto EventScripting = GetEventScripting();

					if (EventScripting)
					{
						FName Name = UKismetStringLibrary::Conv_StringToName(L"DrumGun");
						EventScripting->ProcessEvent(SetUnvaultItemNameFn, &Name);

						static auto PillarsConcludedFn = FindObject<UFunction>(L"/Game/Athena/Prototype/Blueprints/White/BP_SnowScripting.BP_SnowScripting_C.PillarsConcluded");
						EventScripting->ProcessEvent(PillarsConcludedFn, &Name);
					}
				}
			}
		}

		else if (Tab == CALENDAR_TAB)
		{
			if (Calendar::HasSnowModification())
			{
				static bool bFirst = false;

				static float FullSnowValue = Calendar::GetFullSnowMapValue();
				static float NoSnowValue = 0.0f;
				static float SnowValue = 0.0f;

				ImGui::SliderFloat(("Snow Level"), &SnowValue, 0, FullSnowValue);

				if (ImGui::Button("Set Snow Level"))
				{
					Calendar::SetSnow(SnowValue);
				}

				if (ImGui::Button("Toggle Full Snow Map"))
				{
					bFirst ? Calendar::SetSnow(NoSnowValue) : Calendar::SetSnow(FullSnowValue);

					bFirst = !bFirst;
				}
			}

			if (Calendar::HasNYE())
			{
				if (ImGui::Button("Start New Years Eve Event"))
				{
					Calendar::StartNYE();
				}
			}

			if (std::floor(Fortnite_Version) == 13)
			{
				static UObject* WL = FindObject("/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Apollo_WaterSetup_2");

				if (WL)
				{
					static auto MaxWaterLevelOffset = WL->GetOffset("MaxWaterLevel");

					static int MaxWaterLevel = WL->Get<int>(MaxWaterLevelOffset);
					static int WaterLevel = 0;

					ImGui::SliderInt("WaterLevel", &WaterLevel, 0, MaxWaterLevel);

					if (ImGui::Button("Set Water Level"))
					{
						Calendar::SetWaterLevel(WaterLevel);
					}
				}
			}
		}

		else if (Tab == ZONE_TAB)
		{
			if (ImGui::Button("Start Safe Zone"))
			{
				UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"startsafezone", nullptr);
			}

			if (ImGui::Button("Pause Safe Zone"))
			{
				UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"pausesafezone", nullptr);
			}

			if (ImGui::Button("Skip Zone"))
			{
				UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"skipsafezone", nullptr);
			}

			if (ImGui::Button("Start Shrink Safe Zone"))
			{
				UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), L"startshrinksafezone", nullptr);
			}

			if (ImGui::Button("Skip Shrink Safe Zone"))
			{
				auto GameMode = Cast<AFortGameModeAthena>(GetWorld()->GetGameMode());
				auto SafeZoneIndicator = GameMode->GetSafeZoneIndicator();

				if (SafeZoneIndicator)
				{
					SafeZoneIndicator->SkipShrinkSafeZone();
				}
			}
		}

		else if (Tab == DUMP_TAB)
		{
			ImGui::Text("These will all be in your Win64 folder!");

			static std::string FortniteVersionStr = std::format("Fortnite Version {}\n\n", std::to_string(Fortnite_Version));

			if (ImGui::Button("Dump Objects"))
			{
				auto ObjectNum = ChunkedObjects ? ChunkedObjects->Num() : UnchunkedObjects ? UnchunkedObjects->Num() : 0;

				std::ofstream obj("ObjectsDump.txt");

				obj << FortniteVersionStr;

				for (int i = 0; i < ObjectNum; i++)
				{
					auto CurrentObject = GetObjectByIndex(i);

					if (!CurrentObject)
						continue;

					obj << CurrentObject->GetFullName() << '\n';
				}
			}

			if (ImGui::Button("Dump Skins (Skins.txt)"))
			{
				std::ofstream SkinsFile("Skins.txt");

				if (SkinsFile.is_open())
				{
					SkinsFile << FortniteVersionStr;

					static auto CIDClass = FindObject<UClass>("/Script/FortniteGame.AthenaCharacterItemDefinition");

					auto AllObjects = GetAllObjectsOfClass(CIDClass);

					for (int i = 0; i < AllObjects.size(); i++)
					{
						auto CurrentCID = AllObjects.at(i);

						static auto DisplayNameOffset = CurrentCID->GetOffset("DisplayName");

						FString DisplayNameFStr = UKismetTextLibrary::Conv_TextToString(CurrentCID->Get<FText>(DisplayNameOffset));

						if (!DisplayNameFStr.Data.Data)
							continue;

						SkinsFile << std::format("[{}] {}\n", DisplayNameFStr.ToString(), CurrentCID->GetPathName());
					}
				}
			}

			if (ImGui::Button("Dump Playlists (Playlists.txt)"))
			{
				std::ofstream PlaylistsFile("Playlists.txt");

				if (PlaylistsFile.is_open())
				{
					PlaylistsFile << FortniteVersionStr;
					static auto FortPlaylistClass = FindObject<UClass>("/Script/FortniteGame.FortPlaylist");
					// static auto FortPlaylistClass = FindObject("Class /Script/FortniteGame.FortPlaylistAthena");

					auto AllObjects = GetAllObjectsOfClass(FortPlaylistClass);

					for (int i = 0; i < AllObjects.size(); i++)
					{
						auto Object = AllObjects.at(i);

						static auto UIDisplayNameOffset = Object->GetOffset("UIDisplayName");
						FString PlaylistNameFStr = UKismetTextLibrary::Conv_TextToString(Object->Get<FText>(UIDisplayNameOffset));

						if (!PlaylistNameFStr.Data.Data)
							continue;

						std::string PlaylistName = PlaylistNameFStr.ToString();

						PlaylistsFile << std::format("[{}] {}\n", PlaylistName, Object->GetPathName());
					}
				}
				else
					std::cout << "Failed to open playlist file!\n";
			}

			if (ImGui::Button("Dump Weapons (Weapons.txt)"))
			{
				std::ofstream WeaponsFile("Weapons.txt");

				if (WeaponsFile.is_open())
				{
					WeaponsFile << FortniteVersionStr;

					auto DumpItemDefinitionClass = [&WeaponsFile](UClass* Class) {
						auto AllObjects = GetAllObjectsOfClass(Class);

						for (int i = 0; i < AllObjects.size(); i++)
						{
							auto Object = AllObjects.at(i);

							static auto DisplayNameOffset = Object->GetOffset("DisplayName");
							FString ItemDefinitionFStr = UKismetTextLibrary::Conv_TextToString(Object->Get<FText>(DisplayNameOffset));

							if (!ItemDefinitionFStr.Data.Data)
								continue;

							std::string ItemDefinitionName = ItemDefinitionFStr.ToString();

							// check if it contains gallery or playset and just ignore?

							WeaponsFile << std::format("[{}] {}\n", ItemDefinitionName, Object->GetPathName());
						}
					};

					DumpItemDefinitionClass(UFortWeaponItemDefinition::StaticClass());
					DumpItemDefinitionClass(UFortGadgetItemDefinition::StaticClass());
					DumpItemDefinitionClass(FindObject<UClass>("/Script/FortniteGame.FortAmmoItemDefinition"));
				}
				else
					std::cout << "Failed to open playlist file!\n";
			}
		}
		else if (Tab == UNBAN_TAB)
		{

		}
		else if (Tab == FUN_TAB)
		{
			static std::string ItemToGrantEveryone;
			static int AmountToGrantEveryone = 1;

			ImGui::InputFloat("Starting Shield", &StartingShield);
			ImGui::InputText("Item to Give", &ItemToGrantEveryone);
			ImGui::InputInt("Amount to Give", &AmountToGrantEveryone);

			if (ImGui::Button("Destroy all player builds"))
			{
				bShouldDestroyAllPlayerBuilds = true;
			}

			if (ImGui::Button("Give Item to Everyone"))
			{
				auto ItemDefinition = FindObject<UFortItemDefinition>(ItemToGrantEveryone, nullptr, ANY_PACKAGE);
				
				if (ItemDefinition)
				{
					static auto World_NetDriverOffset = GetWorld()->GetOffset("NetDriver");
					auto WorldNetDriver = GetWorld()->Get<UNetDriver*>(World_NetDriverOffset);
					auto& ClientConnections = WorldNetDriver->GetClientConnections();

					for (int i = 0; i < ClientConnections.Num(); i++)
					{
						auto PlayerController = Cast<AFortPlayerController>(ClientConnections.at(i)->GetPlayerController());

						if (!PlayerController->IsValidLowLevel())
							continue;

						auto WorldInventory = PlayerController->GetWorldInventory();

						if (!WorldInventory->IsValidLowLevel())
							continue;

						bool bShouldUpdate = false;
						WorldInventory->AddItem(ItemDefinition, &bShouldUpdate, AmountToGrantEveryone);

						if (bShouldUpdate)
							WorldInventory->Update();
					}
				}
				else
				{
					ItemToGrantEveryone = "";
					LOG_WARN(LogUI, "Invalid Item Definition!");
				}
			}

			auto GameState = Cast<AFortGameStateAthena>(GetWorld()->GetGameState());

			if (GameState)
			{
				static auto DefaultGliderRedeployCanRedeployOffset = FindOffsetStruct("/Script/FortniteGame.FortGameStateAthena", "DefaultGliderRedeployCanRedeploy", false);
				static auto DefaultParachuteDeployTraceForGroundDistanceOffset = GameState->GetOffset("DefaultParachuteDeployTraceForGroundDistance", false);

				if (Globals::bStartedListening) // it resets accordingly to ProHenis b4 this
				{
					if (DefaultParachuteDeployTraceForGroundDistanceOffset != -1)
					{
						ImGui::InputFloat("Automatic Parachute Pullout Distance", GameState->GetPtr<float>(DefaultParachuteDeployTraceForGroundDistanceOffset));
					}
				}

				if (DefaultGliderRedeployCanRedeployOffset != -1)
				{
					bool EnableGliderRedeploy = (bool)GameState->Get<float>(DefaultGliderRedeployCanRedeployOffset);

					if (ImGui::Checkbox("Enable Glider Redeploy", &EnableGliderRedeploy))
					{
						GameState->Get<float>(DefaultGliderRedeployCanRedeployOffset) = EnableGliderRedeploy;
					}
				}

				GET_PLAYLIST(GameState);

				if (CurrentPlaylist)
				{
					bool bRespawning = CurrentPlaylist->GetRespawnType() == EAthenaRespawnType::InfiniteRespawn || CurrentPlaylist->GetRespawnType() == EAthenaRespawnType::InfiniteRespawnExceptStorm;

					if (ImGui::Checkbox("Respawning", &bRespawning))
					{
						CurrentPlaylist->GetRespawnType() = (EAthenaRespawnType)bRespawning;
					}
				}
			}
		}

		

		else if (Tab == LATEGAME_TAB)
		{
			if (bEnableReverseZone)
				ImGui::Text(std::format("Currently {}eversing zone", bZoneReversing ? "r" : "not r").c_str());

			ImGui::Checkbox("Enable Reverse Zone (EXPERIMENTAL)", &bEnableReverseZone);

			if (bEnableReverseZone)
			{
				ImGui::InputInt("Start Reversing Phase", &StartReverseZonePhase);
				ImGui::InputInt("End Reversing Phase", &EndReverseZonePhase);
			}
		}
		else if (Tab == CREDITS_TAB)
		{
			ImGui::Text("Milxnor is Sigma");
		}
	}
	else if (PlayerTab != 2435892 && bLoaded)
	{

		vector<pair<UObject*, UObject*>> AllSigmaControllers;
		auto World = GetWorld();
		{
			static auto NetDriverOffset = World->GetOffset("NetDriver");
			auto NetDriver = *(UObject**)(__int64(World) + NetDriverOffset);

			if (NetDriver)
			{
				static auto ClientConnectionsOffset = NetDriver->GetOffset("ClientConnections");
				auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

				if (ClientConnections)
				{
					for (int i = 0; i < ClientConnections->Num(); i++)
					{
						auto Connection = ClientConnections->At(i);

						if (!Connection)
							continue;

						static auto Connection_PlayerControllerOffset = Connection->GetOffset("PlayerController");
						auto CurrentController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);

						if (CurrentController)
						{
							AllSigmaControllers.push_back({ CurrentController, Connection });
						}
					}
				}
			}
		}
		if (PlayerTab < AllSigmaControllers.size())
		{
			PlayerTabs();

			auto& CurrentPair = AllSigmaControllers.at(PlayerTab);
			auto CurrentController = CurrentPair.first;
			auto CurrentPawn = SkibidiToilet::GetPawnFromController(CurrentController);
			auto CurrentPlayerState = SkibidiToilet::GetPlayerStateFromController(CurrentController);
			if (CurrentPlayerState)
			{
				FString NameFStr;

				auto Connection = CurrentPair.second;
				auto RequestURL = *GetRequestURL(Connection);

				if (RequestURL.Data.Data)
				{
					auto RequestURLStr = RequestURL.ToString();

					std::size_t pos = RequestURLStr.find("Name=");

					if (pos != std::string::npos) {
						std::size_t end_pos = RequestURLStr.find('?', pos);

						if (end_pos != std::string::npos)
							RequestURLStr = RequestURLStr.substr(pos + 5, end_pos - pos - 5);
					}

					auto RequestURLCStr = RequestURLStr.c_str();
					ImGui::Text(("Viewing " + RequestURLStr).c_str());

					if (playerTabTab == MAIN_PLAYERTAB)
					{
						static std::string WID;
						static std::string KickReason = "You Have Been Banned";
						static int stud = 0;

						ImGui::InputText("WID To Give", &WID);
						ImGui::InputText("Kick Reason", &KickReason);

						if (CurrentPawn)
						{
							auto CurrentWeapon = SkibidiToilet::GetCurrentWeapon(CurrentPawn);
							static auto AmmoCountOffset = FindOffsetStruct("Class /Script/FortniteGame.FortWeapon", "AmmoCount");

							auto AmmoCountPtr = (int*)(__int64(CurrentWeapon) + AmmoCountOffset);

							if (ImGui::Button("Spawn Pickup with WID"))
							{
								std::string cpywid = WID;

								if (cpywid.find(".") == std::string::npos)
									cpywid = std::format("{}.{}", cpywid, cpywid);

								if (cpywid.find(" ") != std::string::npos)
									cpywid = cpywid.substr(cpywid.find(" ") + 1);

								auto SEX = Cast<UFortWorldItemDefinition>(FindObject(WID, nullptr, ANY_PACKAGE));

								auto Location = SkibidiToilet::GetActorLocationDynamic(CurrentPawn);
								int count = 1;
								int amount = 1;
								auto GameState = Cast<AFortGameStateAthena>(GetWorld()->GetGameState());

								PickupCreateData CreateData;
								CreateData.ItemEntry = FFortItemEntry::MakeItemEntry(SEX, count, -1, MAX_DURABILITY, SEX->GetFinalLevel(GameState->GetWorldLevel()));
								CreateData.SpawnLocation2 = Location;
								CreateData.bShouldFreeItemEntryWhenDeconstructed = true;

								for (int i = 0; i < amount; i++)
								{
									AFortPickup::SpawnPickup(CreateData);
								}
							}
						}
						if (ImGui::Button("Kick"))
						{
							std::wstring wstr = std::wstring(KickReason.begin(), KickReason.end());
							FString Reason;
							Reason.Set(wstr.c_str());

							static auto ClientReturnToMainMenu = FindObject<UFunction>("/Script/Engine.PlayerController.ClientReturnToMainMenu");
							CurrentController->ProcessEvent(ClientReturnToMainMenu, &Reason);
						}
						if (ImGui::Button("Ban"))
						{
							BanPlayer(CurrentController);
						}

					}
					else if (playerTabTab == FUN_PLAYERTAB)
					{
						string classname;
						int Amount;
						ImGui::InputText("Class Name Here", &classname);
						ImGui::InputInt("Amount", &Amount);
						static auto BGAClass = FindObject<UClass>(L"/Script/Engine.BlueprintGeneratedClass");
						static auto ClassClass = FindObject<UClass>(L"/Script/CoreUObject.Class");
						auto ClassObj = classname.contains("/Script/") ? FindObject<UClass>(classname, ClassClass) : LoadObject<UClass>(classname, BGAClass);
					}
				}

			}
		}

		ImGui::NewLine();
		if (ImGui::Button("Back"))
		{
			PlayerTab = -1;
		}
	}
}

static inline void PregameUI()
{
	StaticUI();

	if (Engine_Version >= 422 && Engine_Version < 424)
	{
		ImGui::Checkbox("Creative", &Globals::bCreative);
	}

	if (Addresses::SetZoneToIndex)
	{
		bool bWillBeLategame = Globals::bLateGame.load();
		ImGui::Checkbox("Lategame", &bWillBeLategame);
		SetIsLategame(bWillBeLategame);
	}

	if (HasEvent())
	{
		ImGui::Checkbox("Play Event", &Globals::bGoingToPlayEvent);
	}

	if (!bSwitchedInitialLevel)
	{
		// ImGui::Checkbox("Use Custom Map", &bUseCustomMap);

		if (bUseCustomMap)
		{
			// ImGui::InputText("Custom Map", &CustomMapName);
		}

		ImGui::SliderInt("Seconds until load into map", &SecondsUntilTravel, 1, 100);
	}
		
	if (!Globals::bCreative)
		ImGui::InputText("Playlist", &PlaylistName);
}

static inline HICON LoadIconFromMemory(const char* bytes, int bytes_size, const wchar_t* IconName) {
	HANDLE hMemory = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bytes_size, IconName);
	if (hMemory == NULL) {
		return NULL;
	}

	LPVOID lpBuffer = MapViewOfFile(hMemory, FILE_MAP_READ, 0, 0, bytes_size);

	if (lpBuffer == NULL) {
		CloseHandle(hMemory);
		return NULL;
	}

	ICONINFO icon_info;

	if (!GetIconInfo((HICON)lpBuffer, &icon_info)) {
		UnmapViewOfFile(lpBuffer);
		CloseHandle(hMemory);
		return NULL;
	}

	HICON hIcon = CreateIconIndirect(&icon_info);
	UnmapViewOfFile(lpBuffer);
	CloseHandle(hMemory);
	return hIcon;
}

static inline DWORD WINAPI GuiThread(LPVOID)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"RebootClass", NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindowExW(0L, wc.lpszClassName, L"Project Reboot", (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX), 100, 100, Width, Height, NULL, NULL, wc.hInstance, NULL);

	if (false) // idk why this dont work
	{
		auto hIcon = LoadIconFromMemory((const char*)reboot_icon_data, strlen((const char*)reboot_icon_data), L"RebootIco");
		SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	}

	// SetWindowLongPtrW(hwnd, GWL_STYLE, WS_POPUP); // Disables windows title bar at the cost of dragging and some quality

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.IniFilename = NULL; // Disable imgui.ini generation.
	io.DisplaySize = ImGui::GetMainViewport()->Size;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	InitFont();
	InitStyle();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
	// static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	// io.Fonts->AddFontFromFileTTF("Reboot Resources/fonts/fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);

	bool done = false;

	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				// done = true;
				break;
			}
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto WindowSize = ImGui::GetMainViewport()->Size;
		// ImGui::SetNextWindowPos(ImVec2(WindowSize.x * 0.5f, WindowSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // Center
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

		tagRECT rect;

		if (GetWindowRect(hwnd, &rect))
		{
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		}

		if (!ImGui::IsWindowCollapsed())
		{
			ImGui::Begin("Project Reboot 3.0", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

			Globals::bInitializedPlaylist ? MainUI() : PregameUI();

			ImGui::End();
		}

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

static inline bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

static inline void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

static inline void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static inline LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// my implementation of window dragging..
	/* {
		static int dababy = 0;
		if (dababy > 100) // wait until gui is initialized ig?
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton(0)))
			{
				// if (LOWORD(lParam) > 255 && HIWORD(lParam) > 255)
				{
					POINT p;
					GetCursorPos(&p);

					SetWindowPos(hWnd, nullptr, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}
		dababy++;
	} */

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}