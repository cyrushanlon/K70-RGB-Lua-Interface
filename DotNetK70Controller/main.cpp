#include "stdafx.h"

#include<ctime>
#include<list>

#include<lua.hpp>

#include "Device.h"

std::string GetTime()
{
	time_t CurrentTime;
	struct tm localTime;

	time(&CurrentTime);
	localtime_s(&localTime, &CurrentTime);

	std::ostringstream oss;
	oss << "[" << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << "] ";

	return oss.str();
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// CTRL-CLOSE: confirm that the user wants to exit.
	case CTRL_CLOSE_EVENT: // somehow call destructor of the objects from here
		return(TRUE);

	default:
		return FALSE;
	}
}

//Made global for now so LuaSetLed can access
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

int _tmain(int argc, _TCHAR* argv[])
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE)) // setup control handler to look for events
	{
		//Setup lua state for loading scripts
		lua_State *L;
		L = luaL_newstate();

		luaL_openlibs(L);

		lua_register(L, "SetLed" ,LuaSetLed); // Registers SetLed in lua to call LuaSetLed in c++

		Keyboard = new Device(); // convert to smart pointer?

		std::cout << GetTime() << "Looking for Corsair RGB K70/K95..." << std::endl;
		if (Keyboard->InitKeyboard()) // Check that it is found
		{
			luaL_dofile(L, "lua/hello.lua");// changes tilde to red!

			std::cout << GetTime() << "Keyboard Initialised." << std::endl;

			std::cout << GetTime() << "Lua connection established." << std::endl;

			Keyboard->Run();

			bool Done = false;
			std::string In;
			while (!Done)
			{
				std::cout << GetTime() << "Type exit to exit: ";
				std::cin >> In;

				if (In == "exit")
				{
					Done = true;
				}
			}

			std::cout << GetTime() << "Closing Thread." << std::endl;
		}
		else // if its not found just end the program
		{
			std::cout << GetTime() << "Corsair K70 RGB keyboard not detected :(" << std::endl;
		}

		delete Keyboard;
		Keyboard = NULL;

		lua_close(L);

		std::cout << GetTime() << "Keyboard pointer deleted.";
	}
	else // if the control handler cant get setup
	{
		std::cout << GetTime() << "ERROR: Could not set control handler" << std::endl;
		return 1;
	}

	Sleep(1000);
	return 0;
}


