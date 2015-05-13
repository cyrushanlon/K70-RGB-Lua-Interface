#include "stdafx.h"

#include<ctime>

#include "Device.h"

std::string GetTime()
{
	time_t CurrentTime;
	struct tm localTime;

	time(&CurrentTime);
	localtime_s(&localTime, &CurrentTime);

	std::ostringstream oss;
	oss << "[" << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << "]";

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

int _tmain(int argc, _TCHAR* argv[])
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE)) // setup control handler to look for events
	{
		//Setup Keyboard connection etc
		Device* Keyboard = new Device(); // dont like using new but eh

		std::cout << "Looking for Corsair K70 RGB..." << std::endl;
		if (Keyboard->InitKeyboard()) // Check that it is found
		{
			std::cout << GetTime() << " Keyboard Initialised." << std::endl;

			Keyboard->Run();

			bool Done = false;
			std::string In;
			while (!Done)
			{
				std::cout << "Type exit to exit: ";
				std::cin >> In;

				if (In == "exit")
				{
					Done = true;
				}
			}

			std::cout << GetTime() << " Closing Thread." << std::endl;
		}
		else // if its not found just end the program
		{
			std::cout << "Corsair K70 RGB keyboard not detected :(" << std::endl;
		}

		delete Keyboard; // convert to smart pointers
		Keyboard = NULL;

		std::cout << GetTime() << " Keyboard pointer deleted.";
	}
	else // if the control handler cant get setup
	{
		std::cout << "ERROR: Could not set control handler" << std::endl;
		return 1;
	}

	Sleep(2000);
	return 0;
}


