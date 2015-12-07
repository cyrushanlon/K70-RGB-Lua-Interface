#pragma once
#include "stdafx.h"

#include <pdh.h>

#include <lua.hpp>
#include "Device.h"
#include "WindowsInput.h"

// custom user message for closing lua script
#define WM_USER_ENDPLS (WM_USER + 100) 

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

//C++ side of lua script execution
void LuaThreadLoop(lua_State *L, DWORD HomeThread)
{
	bool Running = true;

	//Main script loop
	while (Running)
	{
		Running = RunMain(L); // run main lua

		if (KeysDown.size() > 0) // if there are any pressed keys
		{
			//Escape command to leave script
			if ((std::find(KeysDown.begin(), KeysDown.end(), VK_LCONTROL) != KeysDown.end()) &&
				(std::find(KeysDown.begin(), KeysDown.end(), VK_LMENU) != KeysDown.end()) &&
				(std::find(KeysDown.begin(), KeysDown.end(), VK_END) != KeysDown.end()))
			{
				Running = false;
				break; // probs bad
			}
			else // if close script command isnt used
			{
				//Find keys that are down but not yet sent
				for (std::set<int>::iterator i = KeysDown.begin(); i != KeysDown.end(); i++)
				{
					int Element = *i;
					//if current key hasnt yet been sent
					if (std::find(KeysDownSent.begin(), KeysDownSent.end(), Element) == KeysDownSent.end())
					{
						//Send the key
						RunKeyPress(L, Element);
					}
				}
			}
		}

		// removes keys that are up from keysdown and sends keyup 
		if (KeysUpToSend.size() > 0) // if there are any keys up that were down
		{
			for (std::set<int>::iterator i = KeysUpToSend.begin(); i != KeysUpToSend.end(); i++)
			{
				int Element = *i;
				auto Find = std::find(KeysDown.begin(), KeysDown.end(), Element);
				if (Find != KeysDown.end())
				{
					RunKeyRelease(L, Element);
					KeysDown.erase(Find);
				}
			}
		}

		//Cleans stuff up
		if (KeysDown.size() > 100) // temp flush to make sure no overflow
		{
			KeysDown.clear();
		}

		KeysDownSent.clear();
		KeysDownSent = KeysDown;
		KeysUpToSend.clear();
	}

	KeysDown.clear();
	KeysUpToSend.clear();
	PostThreadMessage(HomeThread, WM_USER_ENDPLS, 0, 0);
}

//Handles windows messages and handles script termination

std::string GetTime() // needs moving somewhere else
{
	time_t CurrentTime;
	struct tm localTime;

	time(&CurrentTime);
	localtime_s(&localTime, &CurrentTime);

	std::ostringstream oss;
	oss << "[" << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << "] ";

	return oss.str();
}

bool inline FileExist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void RunScript(lua_State *L, std::string FileName)
{
	if (FileExist(FileName.c_str()))
	{
		lua_settop(L, 0);
		RemoveFunctions(L); // defaults any functions to nil
		if (luaL_dofile(L, FileName.c_str()) != 0) // if there is an error with loading lua file into state
		{
			std::cout << lua_tostring(L, -1) << std::endl;
		}
		else
		{
			SetKeyboardHook();

			std::cout << std::endl << GetTime() << "Keyboard hook set." << std::endl;

			std::thread LuaThread(LuaThreadLoop, L, GetCurrentThreadId());

			std::cout << GetTime() << "Lua thread open." << std::endl;
			std::cout << GetTime() << "Script running..." << std::endl << std::endl;
			std::cout << GetTime() << "(LControl + LAlt + End) to end script." << std::endl;

			MSG msg; // message loop to recieve key inputs
			while (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				if (msg.message == WM_USER_ENDPLS)
				{
					break;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			LuaThread.join();
			std::cout << std::endl << GetTime() << "Lua thread closed." << std::endl;

			UnhookKeyboard();

			std::cout << GetTime() << "Keyboard hook released" << std::endl;

			std::cout << GetTime() << "Done!" << std::endl << std::endl;
		}
	}
	else
	{
		std::cout << std::endl << "File doesnt exist!" << std::endl;
	}

}