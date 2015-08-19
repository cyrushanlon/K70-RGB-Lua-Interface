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

std::set<int> KeysDown; // Keys that are currently down
std::set<int> KeysDownSent; // Keys that were just pressed and need to be send

std::set<int> KeysUpToSend;

/////////////////////// USES WINDOWS ///////////

//creates custom user message
#define WM_USER_ENDPLS (WM_USER + 100)
//Gets the keys quite low down but needs to be lower
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
			if (std::find(KeysDown.begin(), KeysDown.end(), p->vkCode) == KeysDown.end())
			{
				KeysDown.insert(p->vkCode);
			}
			//std::cout << "Down " << p->vkCode << std::endl;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			KeysUpToSend.insert(p->vkCode);
			//std::cout << "Up " << p->vkCode << std::endl;
			break;
		}
	}
	return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}

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

HHOOK hhkLowLevelKybd;
void SetKeyboardHook()
{
	hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
}

void UnhookKeyboard()
{
	UnhookWindowsHookEx(hhkLowLevelKybd); // releases low level hook to keyboard
}

/////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	// Check if keyboard layout is compatible and create config file for default script etc
	SetupConfig();
	std::cout << "Config: \n" << std::endl;

	for (auto it = ConfigMap.cbegin(); it != ConfigMap.cend(); ++it)
	{
		std::cout << it->first << " : " << it->second << "\n";
	}

	std::cout << std::endl;

	//Setup lua state for loading scripts
	lua_State *L = luaL_newstate();
	LuaSetup(L);
	 
	Keyboard = new Device(); // convert to smart pointer?

	std::cout << GetTime() << "Looking for Corsair RGB K70/K95..." << std::endl;
	if (Keyboard->InitKeyboard()) // Check that it is found
	{
		std::cout << GetTime() << "Keyboard Initialised." << std::endl;

		bool Done = false;
		std::cout << std::endl << "Type exit to exit." << std::endl << "Type the name of a valid script file excluding '.lua'." << std::endl << std::endl;
		while (!Done) // main menu
		{
			bool FileDone = false;
			while (!FileDone)
			{
				FindFiles();
				std::cout << "> ";
				std::string In = "";
				std::cin >> In;

				if (In != "exit")
				{
					std::string filename = "lua/" + In + ".lua";
					if (FileExist(filename.c_str()))
					{
						lua_settop(L, 0);
						RemoveFunctions(L); // defaults any functions to nil
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
				std::cout << GetTime() << "Script opening." << std::endl;
				
				SetKeyboardHook();

				std::cout << GetTime() << "Keyboard hook set." << std::endl;

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
				std::cout << GetTime() << "Lua thread closed." << std::endl;

				UnhookKeyboard();

				std::cout << GetTime() << "Keyboard hook released" << std::endl;

				std::cout << GetTime() << "Done!" << std::endl << std::endl;
			}

			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
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


