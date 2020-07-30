/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

#include "script.h"
#include "keyboard.h"
#include <string>
#include <vector>

LPCSTR pedModel = "a_m_y_latino_01";
DWORD	vehUpdateTime;

std::vector<Ped> peds;

std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

void update_status_text()
{
	if (GetTickCount() < statusTextDrawTicksMax)
	{
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(0.55, 0.55);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
		if (statusTextGxtEntry)
		{
			UI::_SET_TEXT_ENTRY((char*)statusText.c_str());
		}
		else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char*)statusText.c_str());
		}
		UI::_DRAW_TEXT(0.5, 0.5);
	}
}

void set_status_text(std::string str, DWORD time = 2500, bool isGxtEntry = false)
{
	statusText = str;
	statusTextDrawTicksMax = GetTickCount() + time;
	statusTextGxtEntry = isGxtEntry;
}


void relaseFarAndDiedPeds() {
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
	std::vector<Ped>::iterator it = peds.begin();
	
	while (it != peds.end())
	{
		Vector3 coords = ENTITY::GET_ENTITY_COORDS(*it, TRUE);
		float dist = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(coords.x, coords.y, coords.z, playerCoords.x, playerCoords.y, playerCoords.z, TRUE);
		if (dist > 300 || PED::IS_PED_DEAD_OR_DYING(*it, 1))
		{
			ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&*it);
			it = peds.erase(it);
			set_status_text(std::to_string(peds.size()));
		}
		else {
			it++;
		}
	}
}

void update()
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerCoords = ENTITY::GET_ENTITY_COORDS(playerPed, TRUE);
	
	// fix car
	if (IsKeyJustUp(0x54) && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{      
		Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		VEHICLE::SET_VEHICLE_FIXED(veh);
	}

	// spawn BodyGuards
	if (IsKeyJustUp(0x42) && peds.size() < 7)
	{
		DWORD model = GAMEPLAY::GET_HASH_KEY((char*)pedModel);
		if (STREAMING::IS_MODEL_IN_CDIMAGE(model) && 
			STREAMING::IS_MODEL_VALID(model)
		) {
			STREAMING::REQUEST_MODEL(model);
			while (!STREAMING::HAS_MODEL_LOADED(model)) WAIT(0);
			Ped spawnedPed = PED::CREATE_PED(26, model, playerCoords.x + 1, playerCoords.y + 1, playerCoords.z, 90.0, TRUE, TRUE);
			peds.push_back(spawnedPed);
			int groupIndex = PED::GET_PED_GROUP_INDEX(playerPed);
			PED::SET_PED_AS_GROUP_LEADER(playerPed, groupIndex);
			PED::SET_PED_AS_GROUP_MEMBER(spawnedPed, groupIndex);
			PED::SET_PED_NEVER_LEAVES_GROUP(spawnedPed, true);
			
			WEAPON::GIVE_WEAPON_TO_PED(spawnedPed, GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_ASSAULTRIFLE"), 9999, false, true);
			WAIT(100);
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);
		}
	}

	// выгнать из машины
	if (IsKeyJustUp(0x55) && PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		std::vector<Ped>::iterator it = peds.begin();
		Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		while (it != peds.end()) {
			AI::TASK_LEAVE_VEHICLE(*it, veh, 4160);
			it++;
		}
	}
	
	relaseFarAndDiedPeds();
	update_status_text();
}

void main()
{		
	while (true)
	{
		update();
		WAIT(0);
	}
}

void ScriptMain()
{	
	srand(GetTickCount());
	main();
}
