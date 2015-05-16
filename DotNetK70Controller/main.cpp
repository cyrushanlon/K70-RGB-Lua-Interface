#include "stdafx.h"

#include<ctime>
#include<list>
#include<conio.h>
#include<fstream>

#include<lua.hpp>

//#include "KeyCoordinates.h"
#include "Device.h"
#include "LuaSetup.h"


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

bool FileExist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

std::vector<int> KeysDownToSend;
std::vector<int> KeysUpToSend;
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	BOOL fEatKeystroke = FALSE;

	if (nCode == HC_ACTION)
	{
		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:	
			KeysDownToSend.push_back(p->vkCode);
			//std::cout << "Down " << p->vkCode << std::endl;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			KeysUpToSend.push_back(p->vkCode);
			//std::cout << "Up " << p->vkCode << std::endl;
			break;
		}

		
	}
	return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

bool Running = false;
void LuaThreadLoop(lua_State *L)
{
	Running = true;
	while (Running)
	{
		Running = RunMain(L); // run main

		if (KeysUpToSend.size() > 0) // run key ups
		{
			for (int i = 0; i < KeysUpToSend.size(); i++)
			{
				RunKeyRelease(L, KeysUpToSend.at(i));
			}
			KeysUpToSend.clear();
			std::vector<int>().swap(KeysUpToSend); // more reliable than .clear()
		}

		if (KeysDownToSend.size() > 0) // run key downs
		{
			if (std::find(KeysDownToSend.begin(), KeysDownToSend.end(), VK_ESCAPE) != KeysDownToSend.end())
			{
				if (std::find(KeysDownToSend.begin(), KeysDownToSend.end(), VK_F1) != KeysDownToSend.end())
				{
					Running = false;
				}
			}
			else
			{
				for (int i = 0; i < KeysDownToSend.size(); i++)
				{
					RunKeyPress(L, KeysDownToSend.at(i));
				}
				KeysDownToSend.clear();
				std::vector<int>().swap(KeysDownToSend); // more reliable than .clear()
			}
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//Setup lua state for loading scripts
	lua_State *L = luaL_newstate();;
	LuaSetup(L);
	 
	Keyboard = new Device(); // convert to smart pointer?

	std::cout << GetTime() << "Looking for Corsair RGB K70/K95..." << std::endl;
	if (Keyboard->InitKeyboard()) // Check that it is found
	{
		std::cout << GetTime() << "Keyboard Initialised." << std::endl;

		bool Done = false;
		std::cout << "Type exit to exit." << std::endl << "Type the name of a valid script file excluding '.lua'." << std::endl;

		while (!Done) // main menu
		{
			bool FileDone = false;
			while (!FileDone)
			{
				std::cout << "> ";
				std::string In = "";
				std::cin >> In;

				if (In != "exit")
				{
					std::string filename = "lua/" + In + ".lua";
					if (FileExist(filename.c_str()))
					{
						lua_settop(L, 0);
						if (luaL_dofile(L, filename.c_str()) != 0)
						{
							std::cout << lua_tostring(L, -1) << std::endl;
						}
						else
						{
							FileDone = true;
						}
					}
					else
					{
						std::cout << "File doesnt exist!" << std::endl;
					}
				}
				else
				{
					Done = true;
					FileDone = true;
				}
			}

			if (!Done)
			{
				std::cout << GetTime() << "Script Open, ESC + F1 to close." << std::endl;
				
				HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
				std::thread LuaThread(LuaThreadLoop,L);

				MSG msg; // message loop to recieve key inputs
				while (!GetMessage(&msg, NULL, NULL, NULL))
				{    
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				LuaThread.join();
				UnhookWindowsHookEx(hhkLowLevelKybd); // releases low level hook to keyboard
				std::cout << GetTime() << "Script complete." << std::endl;
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


