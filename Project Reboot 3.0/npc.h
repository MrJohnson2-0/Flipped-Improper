#pragma once

#include "UObjectGlobals.h"
#include "Engine.h"
#include "globals.h"
#include <set>
#include "NetSerialization.h"
#include "RandomStream.h"
#include "Class.h"

static inline std::string GetNPC(std::string NPCToCheck)
{
	std::string NPCClass;

	if (Fortnite_Version >= 13.00)
	{
		if (NPCToCheck == "shark")
			NPCClass = "/SpicySake/Pawns/NPC_Pawn_SpicySake_Parent.NPC_Pawn_SpicySake_Parent_C";
	}

	if (Fortnite_Version >= 16.00)
	{
		if (NPCToCheck == "chicken")
			NPCClass = "/Irwin/AI/Prey/Nug/Pawns/NPC_Pawn_Irwin_Prey_Nug.NPC_Pawn_Irwin_Prey_Nug_C";
		else if (NPCToCheck == "boar")
			NPCClass = "/Irwin/AI/Prey/Burt/Pawns/NPC_Pawn_Irwin_Prey_Burt.NPC_Pawn_Irwin_Prey_Burt_C";
		else if (NPCToCheck == "wolf")
			NPCClass = "/Irwin/AI/Predators/Grandma/Pawns/NPC_Pawn_Irwin_Predator_Grandma.NPC_Pawn_Irwin_Predator_Grandma_C";
		else if (NPCToCheck == "raptor")
			NPCClass = "/Irwin/AI/Predators/Robert/Pawns/NPC_Pawn_Irwin_Predator_Robert.NPC_Pawn_Irwin_Predator_Robert_C";
		else if (NPCToCheck == "frog")
			NPCClass = "/Irwin/AI/Simple/Smackie/Pawns/NPC_Pawn_Irwin_Simple_Smackie.NPC_Pawn_Irwin_Simple_Smackie_C";
		else if (NPCToCheck == "crow")
			NPCClass = "/Irwin/AI/Simple/Avian/Pawns/NPC_Pawn_Irwin_Simple_Avian_Crow.NPC_Pawn_Irwin_Simple_Avian_Crow_C";
	}

	if (Fortnite_Version >= 17.00)
	{
		if (NPCToCheck == "alien")
			NPCClass = "/MotherGameplay/Items/AvacadoEaterBird/BGA_Athena_AvacadoEaterBird.BGA_Athena_AvacadoEaterBird_C";
	}

	if (Fortnite_Version >= 18.21)
	{
		if (NPCToCheck == "caretaker")
			NPCClass = "/Caretaker/Pawns/NPC_Pawn_Irwin_Monster_Caretaker_Smash.NPC_Pawn_Irwin_Monster_Caretaker_Smash_C";
	}

	if (Fortnite_Version >= 19.10)
	{
		if (NPCToCheck == "klombo")
			NPCClass = "/ButterCake/Pawns/NPC_Pawn_ButterCake_Base.NPC_Pawn_ButterCake_Base_C";
		else if (NPCToCheck == "klombo2")
			NPCClass = "/ButterCake/Pawns/NPC_Pawn_ButterCake_A.NPC_Pawn_ButterCake_A_C";
		else if (NPCToCheck == "klombo3")
			NPCClass = "/ButterCake/Pawns/NPC_Pawn_ButterCake_B.NPC_Pawn_ButterCake_B_C";
		else if (NPCToCheck == "klombo4")
			NPCClass = "/ButterCake/Pawns/NPC_Pawn_ButterCake_C.NPC_Pawn_ButterCake_C_C";
		else if (NPCToCheck == "klombo5")
			NPCClass = "/ButterCake/Pawns/NPC_Pawn_ButterCake_D.NPC_Pawn_ButterCake_D_C";
	}

	return NPCClass;
}

static inline std::string GetWeatherCondition(std::string ConditionToCheck)
{
	std::string ConditionClass;

	if (Fortnite_Version >= 19.01)
	{
		if (ConditionToCheck == "tornado")
			ConditionClass = "/Superstorm/Tornado/BP_Tornado.BP_Tornado_C";
		else if (ConditionToCheck == "lightning")
			ConditionClass = "/Superstorm/Lightning/BP_Lightning.BP_Lightning_C";
		else if (ConditionToCheck == "lightningbolt")
			ConditionClass = "/Superstorm/Lightning/BP_LightningBolt.BP_LightningBolt_C";
	}

	return ConditionClass;
}


static inline std::string GetVehicle(std::string VehicleToCheck)
{
	std::string VehicleClass;

	if (VehicleToCheck == "driftboard")
		VehicleClass = "/Game/Athena/DrivableVehicles/JackalVehicle_Athena.JackalVehicle_Athena_C";
	else if (VehicleToCheck == "surfboard")
		VehicleClass = "/Game/Athena/DrivableVehicles/SurfboardVehicle_Athena.SurfboardVehicle_Athena_C";
	else if (VehicleToCheck == "quadcrasher")
		VehicleClass = "/Game/Athena/DrivableVehicles/AntelopeVehicle.AntelopeVehicle_C";
	else if (VehicleToCheck == "baller")
		VehicleClass = "/Game/Athena/DrivableVehicles/Octopus/OctopusVehicle.OctopusVehicle_C";
	else if (VehicleToCheck == "plane")
		VehicleClass = "/Game/Athena/DrivableVehicles/Biplane/BluePrints/FerretVehicle.FerretVehicle_C";
	else if (VehicleToCheck == "golfcart")
		VehicleClass = "/Game/Athena/DrivableVehicles/Golf_Cart/Golf_Cart_Base/Blueprints/GolfCartVehicleSK.GolfCartVehicleSK_C";
	else if (VehicleToCheck == "cannon")
		VehicleClass = "/Game/Athena/DrivableVehicles/PushCannon.PushCannon_C";
	else if (VehicleToCheck == "shoppingcart")
		VehicleClass = "/Game/Athena/DrivableVehicles/ShoppingCartVehicleSK.ShoppingCartVehicleSK_C";
	else if (VehicleToCheck == "brute")
		VehicleClass = "/Game/Athena/DrivableVehicles/Mech/TestMechVehicle.TestMechVehicle_C";
	else if (VehicleToCheck == "truck")
		VehicleClass = "/Valet/BasicTruck/Valet_BasicTruck_Vehicle.Valet_BasicTruck_Vehicle_C";
	else if (VehicleToCheck == "car")
		VehicleClass = "/Valet/BasicCar/Valet_BasicCar_Vehicle.Valet_BasicCar_Vehicle_C";
	else if (VehicleToCheck == "sportcar")
		VehicleClass = "/Valet/SportsCar/Valet_SportsCar_Vehicle.Valet_SportsCar_Vehicle_C";
	else if (VehicleToCheck == "taxi")
		VehicleClass = "/Valet/TaxiCab/Valet_TaxiCab_Vehicle.Valet_TaxiCab_Vehicle_C";
	else if (VehicleToCheck == "mudflap")
		VehicleClass = "/Valet/BigRig/Valet_BigRig_Vehicle.Valet_BigRig_Vehicle_C";
	else if (VehicleToCheck == "starkcar")
		VehicleClass = "/Valet/SportsCar/Valet_SportsCar_Vehicle_HighTower.Valet_SportsCar_Vehicle_HighTower_C";
	else if (VehicleToCheck == "octane")
		VehicleClass = "/Valet/SportsCar/Valet_SportsCar_Vehicle_Turbo.Valet_SportsCar_Vehicle_Turbo_C";
	else if (VehicleToCheck == "boat")
		VehicleClass = "/Game/Athena/DrivableVehicles/Meatball/Meatball_Large/MeatballVehicle_L.MeatballVehicle_L_C";
	else if (VehicleToCheck == "helicopter")
		VehicleClass = "/Hoagie/HoagieVehicle.HoagieVehicle_C";
	else if (VehicleToCheck == "ufo")
		VehicleClass = "/Nevada/Blueprints/Vehicle/Nevada_Vehicle_V2.Nevada_Vehicle_V2_C";
	else if (VehicleToCheck == "ferrari")
		VehicleClass = "/Foray/Vehicle/Foray_Vehicle.Foray_Vehicle_C";

	return VehicleClass;
}