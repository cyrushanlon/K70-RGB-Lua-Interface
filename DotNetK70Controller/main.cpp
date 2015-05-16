#include "stdafx.h"

#include<ctime>
#include<list>
#include<conio.h>
#include<fstream>

#include<lua.hpp>

//#include "KeyCoordinates.h"
#include "dirent.h"
#include "Device.h"
#include "LuaSetup.h"

//creates custom user message
#define WM_USER_ENDPLS (WM_USER + 100)

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
			if ((i != -1) && (i != 0)) // silly 
			{
				std::cout << i << " : " << epdf->d_name << std::endl;
				Sleep(50); // makes it look cool I guess
			}
		}
		std::cout << std::endl;
	}
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
			if (std::find(KeysDownToSend.begin(), KeysDownToSend.end(), p->vkCode) == KeysDownToSend.end())
			{
				KeysDownToSend.push_back(p->vkCode);
			}	
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
void LuaThreadLoop(lua_State *L, DWORD HomeThread)
{
	Running = true;
	while (Running)
	{
		Running = RunMain(L); // run main

		if (KeysDownToSend.size() > 0) // run key downs
		{
			if ((std::find(KeysDownToSend.begin(), KeysDownToSend.end(), VK_LCONTROL) != KeysDownToSend.end()) &&	
			   (std::find(KeysDownToSend.begin(), KeysDownToSend.end(), VK_LMENU) != KeysDownToSend.end()) &&
			   (std::find(KeysDownToSend.begin(), KeysDownToSend.end(), VK_END) != KeysDownToSend.end()))
			{
				Running = false;
				break; // probs bad
			}
			else
			{
				for (int i = 0; i < KeysDownToSend.size(); i++)
				{
					RunKeyPress(L, KeysDownToSend.at(i));
				}		
			}
		}
		
		if (KeysUpToSend.size() > 0) // run key ups
		{
			for (int i = 0; i < KeysUpToSend.size(); i++)
			{
				RunKeyRelease(L, KeysUpToSend.at(i));
			}

		}

		for (int i = 0; i < KeysUpToSend.size(); i++)
		{
			auto Find = std::find(KeysDownToSend.begin(), KeysDownToSend.end(), KeysUpToSend.at(i));
			if (Find != KeysDownToSend.end())
			{
				KeysDownToSend.erase(Find);
			}
		}
		KeysUpToSend.clear();
	}

	KeysDownToSend.clear();
	KeysUpToSend.clear();
	PostThreadMessage(HomeThread, WM_USER_ENDPLS, 0, 0);
}

int _tmain(int argc, _TCHAR* argv[])
{
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
				
				HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);

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

				UnhookWindowsHookEx(hhkLowLevelKybd); // releases low level hook to keyboard

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


