#pragma once
#include "stdafx.h"

#include<lua.hpp>
#include"Device.h"

//Made global for now so LuaSetLed etc can access
Device* Keyboard;

//Change so lua passes the led matrix instead of setting LEDs like this
static int LuaSetLed(lua_State *L) // need and x, y, r, g and b integer passed in
{
	int n = lua_gettop(L);
	if (n > 5) n = 5;
	std::vector<int> Args;
	for (int i = 1; i <= n; i++)
	{
		Args.push_back(lua_tonumber(L, i));
	}
	Keyboard->SetLed(Args.at(0), Args.at(1), Args.at(2), Args.at(3), Args.at(4));
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

//C++ can choose when to call the lua function "main"
//This might be slow ill have to see
void RunMain(lua_State* L)
{
	lua_getglobal(L, "main");

	lua_call(L, 0, 1);
	lua_pop(L, 1);
}

void LuaSetup(lua_State* L)
{
	luaL_openlibs(L);

	lua_register(L, "SetLed", LuaSetLed); // Registers SetLed in lua to call LuaSetLed in c++
	lua_register(L, "Update", LuaUpdateKeyboard);
	lua_register(L, "Sleep", LuaSleep);
}