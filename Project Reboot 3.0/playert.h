#pragma once

#include <iostream>
#include <fstream>
#include "UnrealString.h"

namespace PlayerProperTrust
{
	static FString GetfPlayerName(UObject* Controller)
	{
		if (!Controller)
			return FString();

		static auto McpProfileGroupOffset = GetOffset(Controller, "McpProfileGroup");
		auto MCPProfileGroupPTR = (UObject**)(__int64(Controller) + McpProfileGroupOffset);

		FString Name; // = *PlayerState->Member<FString>(("PlayerNamePrivate"));

		if (!MCPProfileGroupPTR || !*MCPProfileGroupPTR)
		{
			auto PlayerState = Helper::GetPlayerStateFromController(Controller);

			if (PlayerState)
			{
				static auto fn = PlayerState->Function(("GetPlayerName"));

				if (fn)
					PlayerState->ProcessEvent(fn, &Name);
			}
		}
		else
		{
			auto MCPProfileGroup = *MCPProfileGroupPTR;

			static auto PlayerNameOffset = GetOffset(MCPProfileGroup, "PlayerName");
			Name = *(FString*)(__int64(MCPProfileGroup) + PlayerNameOffset);
		}

		return Name;
	}
}