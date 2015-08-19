#pragma once
#include "stdafx.h"

#include <pdh.h>

#include<lua.hpp>
#include"Device.h"

//Made global for now so LuaSetLed etc can access
Device* Keyboard;

//Change so lua passes the led matrix instead of setting LEDs like this
static int LuaSetLed(lua_State *L) // need and x, y, r, g and b integer passed in
{
	std::vector<int> Args;
	std::string KeyStr;
	int n = lua_gettop(L);

	for (int i = 2; i <= n; i++) // these are numbers
		Args.push_back(lua_tonumber(L, i));

	if (n > 5) n = 5;
	if (n == 4) // first is a string
	{
		Keyboard->SetLed(lua_tonumber(L, 1), Args.at(0), Args.at(1), Args.at(2));
	}
	else
	{
		Keyboard->SetLed(lua_tonumber(L, 1), Args.at(0), Args.at(1), Args.at(2), Args.at(3));
	}
	
	return 1;
}

//Lua updates the keyboard through this
static int LuaUpdateKeyboard(lua_State* L)
{
	Keyboard->UpdateDevice();
	return 1;
}

//Lua can set script delays using this
static int LuaSleep(lua_State* L)
{
	Sleep(lua_tonumber(L, 1));
	return 1;
}

//Get CPU usage and give it to lua
static PDH_HQUERY CPUQuery;
static PDH_HCOUNTER CPUTotal;


static void CPUInit()
{
	PdhOpenQuery(NULL, NULL, &CPUQuery);
	PdhAddEnglishCounter(CPUQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &CPUTotal);
	PdhCollectQueryData(CPUQuery);
}


static double GetCPUUsage(){
	PDH_FMT_COUNTERVALUE counterVal;


	PdhCollectQueryData(CPUQuery);
	PdhGetFormattedCounterValue(CPUTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	return counterVal.doubleValue;
}

//Lua can set current CPU usage with this
static int LuaGetCPUUsage(lua_State* L)
{
	lua_pushnumber(L, GetCPUUsage());
	return 1;
}
//

//C++ can choose when to call the lua function "main"
//This might be slow ill have to see
static bool RunMain(lua_State* L)
{
	lua_getglobal(L, "main");
	if (lua_isfunction(L, lua_gettop(L)))
	{
		if (lua_pcall(L, 0, 1, 0) != 0)
		{
			std::cout << lua_tostring(L, -1) << std::endl;
			return false;
		}
		lua_pop(L, 1);
		return true;
	}
	return false;
}

static bool RunKeyPress(lua_State* L, int Key)
{
	if (lua_getglobal(L, "keypress") != 0)
	{
		lua_pushinteger(L, Key); // push first parameter
		if (lua_pcall(L, 1, 1, 0) != 0) // if lua error
		{
			std::cout << lua_tostring(L, -1) << std::endl;
			return false;
		}
		lua_pop(L, 1);
		return true;
	}
	return false;
}

static bool RunKeyRelease(lua_State* L, int Key)
{
	if (lua_getglobal(L, "keyrelease") != 0)
	{
		lua_pushinteger(L, Key); // push first parameter
		if (lua_pcall(L, 1, 1, 0) != 0) // if lua error
		{
			std::cout << lua_tostring(L, -1) << std::endl;
			return false;
		}
		lua_pop(L, 1);
		return true;
	}
	return false;
}

static void RemoveFunctions(lua_State* L)
{
	luaL_dostring(L, "main = nil keypress = nil keyrelease = nil");
}

static void LuaSetup(lua_State* L)
{
	luaL_openlibs(L);

	CPUInit();
	lua_register(L, "SetLed", LuaSetLed); // Registers SetLed in lua to call LuaSetLed in c++
	lua_register(L, "Update", LuaUpdateKeyboard);
	lua_register(L, "Sleep", LuaSleep);
	lua_register(L, "GetCPUUsage", LuaGetCPUUsage);
}