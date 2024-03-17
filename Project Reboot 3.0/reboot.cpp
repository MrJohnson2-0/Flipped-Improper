#include "reboot.h"

#include "SoftObjectPtr.h"

#include "KismetStringLibrary.h"

UObject* Assets::LoadAsset(FName Name, bool ShowDelayTimes)
{
	static UObject* (*LoadAssetOriginal)(FName a1, bool a2) = decltype(LoadAssetOriginal)(Assets::LoadAsset);

	return LoadAssetOriginal(Name, ShowDelayTimes);
}




template <typename T>
T* Get23466464646(void* addr, uint64_t off) { return (T*)(__int64(addr) + off); }

namespace SkibidiToilet {
	UObject* GetPlayerStateFromController(UObject* Controller)
	{
		static auto PlayerStateOffset = Controller->GetOffset("PlayerState");

		return *Get23466464646<UObject*>(Controller, PlayerStateOffset);
	}
	UObject* GetCurrentWeapon(UObject* Pawn)
	{
		static auto CurrentWeaponOffset = Pawn->GetOffset("CurrentWeapon");

		return *Get23466464646<UObject*>(Pawn, CurrentWeaponOffset);
	}
	UObject* GetPawnFromController(UObject* Controller)
	{
		static auto PawnOffset = Controller->GetOffset("Pawn");

		return *Get23466464646<UObject*>(Controller, PawnOffset);
	}

	FVector GetActorLocation(UObject* Actor)
	{
		static auto K2_GetActorLocationFN = FindObject<UFunction>("/Script/Engine.Actor.K2_GetActorLocation");

		FVector loc;
		Actor->ProcessEvent(K2_GetActorLocationFN, &loc);

		return loc;
	}

	std::string GetPlayerName(UObject* Controller)
	{
		// A controller without a player has no "owner"
		// return (Player != NULL) ? NetConnection : NULL;

		static auto NetConnectionOffset = Controller->GetOffset("NetConnection");

		auto Connection = *Get23466464646<UObject*>(Controller, NetConnectionOffset);

		if (!Connection)
			return "NO_CONNECTION";

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

			return RequestURLStr;
		}

		return "INVALID_REQUEST_URL";
	}

	BothVector GetActorLocationDynamic(UObject* Actor)
	{
		if (Fortnite_Version < 20)
			return BothVector(GetActorLocation(Actor));

		static auto K2_GetActorLocationFN = FindObject<UFunction>("/Script/Engine.Actor.K2_GetActorLocation");

		DVector loc;
		Actor->ProcessEvent(K2_GetActorLocationFN, &loc);

		return BothVector(loc);
	}
	FString GetIPf(UObject* PlayerState)
	{
		static auto SavedNetworkAddressOffset = PlayerState->GetOffset("SavedNetworkAddress");
		auto SavedNetworkAddress = Get23466464646<FString>(PlayerState, SavedNetworkAddressOffset);

		return *SavedNetworkAddress;
	}
	
}

UObject* Assets::LoadSoftObject(void* SoftObjectPtr)
{
	if (Engine_Version == 416)
	{
		auto tAssetPtr = (TAssetPtr<UObject>*)SoftObjectPtr;
		// return LoadAsset(tAssetPtr->AssetPtr.ObjectID.AssetLongPathname.);
		return nullptr; // later
	}

	auto tSoftObjectPtr = (TSoftObjectPtr<UObject>*)SoftObjectPtr;

	// if (auto WeakObject = tSoftObjectPtr->GetByWeakObject())
		// return WeakObject;

	return Assets::LoadAsset(tSoftObjectPtr->SoftObjectPtr.ObjectID.AssetPathName);
}