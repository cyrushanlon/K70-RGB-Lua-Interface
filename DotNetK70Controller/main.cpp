#include "stdafx.h"

#include<ctime>
#include<list>
#include<conio.h>
#include<fstream>
#include<set>

#include<lua.hpp>

//#include "KeyCoordinates.h"
#include "dirent.h"

#include "Device.h"
#include "LuaSetup.h"
#include "Config.h"

/////////////////////// USES WINDOWS ///////////
void FindFiles()
{
	DIR *dpdf;
	struct dirent *epdf;

	dpdf = opendir("lua");
	std::cout << "Files in lua folder:" << std::endl;
	if (dpdf != NULL)
	{
		int i = -2;
		while (epdf = readdir(dpdf))
		{
			i++;
			if ((i != -1) && (i != 0)) // silly work around to hide . and ..
			{
				std::cout << i << " : " << epdf->d_name << std::endl;
			}
		}
		std::cout << std::endl;
	}
}
/////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	// Check if keyboard layout is compatible and create config file for default script etc
	SetupConfig();

	std::cout << std::endl;

	//Setup lua state for loading scripts
	lua_State *L = luaL_newstate();
	LuaSetup(L);
	 
	Keyboard = new Device(); // convert to smart pointer?

	std::cout << GetTime() << "Looking for Corsair RGB K70/K95..." << std::endl;
	if (Keyboard->InitKeyboard()) // Check that it is found
	{
		std::cout << GetTime() << "Keyboard Initialised." << std::endl;

		if (ConfigMap.at("DefaultScript") != "") //If there is no default script
		{
			std::string DefaultScriptName = "lua/" + ConfigMap.at("DefaultScript") + ".lua";

			if (FileExist(DefaultScriptName.c_str()))
			{
				std::cout << std::endl << GetTime() << "Default Script loaded." << std::endl;
				RunScript(L, DefaultScriptName);
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // windows call I think
			}
			else
			{
				std::cout << GetTime() << "Default Script is invalid." << std::endl;
			}
		}


		std::cout << std::endl << "Type exit to exit." << std::endl << "Type the name of a valid script file excluding '.lua'." << std::endl << std::endl;

		bool Done = false;
		while (!Done) // main menu
		{
			FindFiles();
			std::cout << "> ";
			std::string In = "";
			std::cin >> In;

			if (In != "exit")
			{
				RunScript(L, "lua/" + In + ".lua");
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // windows call I think
			}
			else
			{
				Done = true;
			}
		}

	}
	else // if its not found just end the program
	{
		std::cout << GetTime() << "Corsair K70 RGB keyboard not detected :(" << std::endl;
	}

	delete Keyboard;
	Keyboard = NULL;

	lua_close(L);

	std::cout << GetTime() << "Keyboard pointer deleted." << std::endl << "lua state closed.";

	Sleep(1000);
	return 0;
}


