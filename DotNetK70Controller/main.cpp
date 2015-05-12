#include "stdafx.h"

#include <iostream>
#include <ctime>
#include <sstream>

extern "C"
{
	#include <hidsdi.h>
	#include <cfgmgr32.h>
}

#include <Setupapi.h>

HANDLE dev;

static int init_keyboard();

static void update_keyboard();

static void set_led(int x, int y, int r, int g, int b);

HANDLE GetDeviceHandle(unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);

char red_val[144];
char grn_val[144];
char blu_val[144];

char data_pkt[5][64] = { { 0 } };

unsigned char position_map[] = {
	137, 8, 20, 255,
	0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 6, 18, 30, 42, 32, 44, 56, 68, 255,
	1, 13, 25, 37, 49, 61, 73, 85, 97, 109, 121, 133, 7, 31, 54, 66, 78, 80, 92, 104, 116, 255,
	2, 14, 26, 38, 50, 62, 74, 86, 98, 110, 122, 134, 90, 102, 43, 55, 67, 9, 21, 33, 128, 255,
	3, 15, 27, 39, 51, 63, 75, 87, 99, 111, 123, 135, 114, 126, 57, 69, 81, 128, 255,
	4 ,16, 28, 40, 52, 64, 76, 88, 100, 112, 124, 136, 79, 103, 93, 105, 117, 140, 255,
	5, 17, 29, 53, 89, 101, 113, 91, 115, 127, 139, 129, 141, 140, 255,
};

float size_map[] = {
	-15.5, 1, 1, -2.5, 1, -2, 0,
	1, -.5, 1, 1, 1, 1, -.75, 1, 1, 1, 1, -.75, 1, 1, 1, 1, -.5, 1, 1, 1, -.5, 1, 1, 1, 1, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, -.5, 1, 1, 1, -.5, 1, 1, 1, 1, 0,
	1.5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5, -.5, 1, 1, 1, -.5, 1, 1, 1, 1, 0,
	1.75, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1.25, -4, 1, 1, 1, 1, 0,
	1.25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2.75, -1.5, 1, -1.5, 1, 1, 1, 1, 0,
	1.5, 1, 1.25, 6.5, 1.25, 1, 1, 1.5, -.5, 1, 1, 1, -.5, 2, 1, 1, 0,
};

unsigned char led_matrix[7][92];

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// CTRL-CLOSE: confirm that the user wants to exit.
	case CTRL_CLOSE_EVENT:
		std::cout << "Closing device handle!";
		CloseHandle(dev);
		return(TRUE);

	default:
		return FALSE;
	}
}

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

int _tmain(int argc, _TCHAR* argv[])
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		if (init_keyboard() == 1) return 1; // end the program if it cant grab the keyboard
		else std::cout << GetTime() << " Keyboard Initialised." << std::endl;
		
		// init variables for the display
		int r = 0;
		int g = 7;
		int b = 0;

		int x = 1;
		int xInc = 3;

		while (true) // display loop
		{
			x += xInc;
			if ((x < 0) || (x > 91)) xInc *= -1;

			for (int i = 0; i < 7; i++)
			{
				set_led(x, i, r, g, b);
			}

			int Newx = x;

			if (xInc < 0)
				Newx = Newx - 5;
			else
				Newx = Newx + 5;

			for (int i = 0; i < 7; i++)
			{
				set_led(Newx, i, 7, 0, 0);
			}

			Sleep(25);

			update_keyboard();
		}
	}
	else
	{
		std::cout << "ERROR: Could not set control handler" << std::endl;
		return 1;
	}
	return 0;
}

static void set_led(int x, int y, int r, int g, int b)
{
	if (x < 0) x = 0;
	else if (x > 91) x = 91;

	if (y < 0) y = 0;
	else if (y > 6) y = 6;

	int led = led_matrix[y][x];

	if (led >= 144)
	{
		return;
	}

	if (r > 7) r = 7;
	if (g > 7) g = 7;
	if (b > 7) b = 7;

	//std::cout << GetTime() << " " << r << " " << g << " " << b << " " << std::endl;

	r = 7 - r;
	g = 7 - g;
	b = 7 - b;

	red_val[led] = r;
	grn_val[led] = g;
	blu_val[led] = b;
}

static int init_keyboard()
{
	std::cout << "Searching for Corsair K70 RGB keyboard..." << std::endl;

	dev = GetDeviceHandle(0x1B1C, 0x1B13, 0x3);

	if (dev == NULL)
	{
		std::cout << "Searching for Corsair K95 RGB keyboard..." << std::endl;
		dev = GetDeviceHandle(0x1B1C, 0x1B11, 0x3);
	}

	if (dev == NULL)
	{
		std::cout << "Corsair K70 RGB keyboard not detected :(" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Corsair K70 RGB keyboard detected successfully :)" << std::endl;
	}

	// Construct XY lookup table
	unsigned char *keys = position_map;
	float *sizes = size_map;
	for (int y = 0; y < 7; y++) // traverse column
	{
		unsigned char key;
		int size = 0;

		for (int x = 0; x < 92; x++) // traverse row
		{
			if (size == 0) // next key
			{
				float sizef = *sizes++;
				if (sizef < 0) // if its a gap
				{
					size = -sizef * 4;
					key = 255;
				}
				else // if its a key
				{
					key = *keys++;
					size = sizef * 4;
				}
			}

			led_matrix[y][x] = key;
			size--; // moves along the row
		}

		if (*keys++ != 255 || *sizes++ != 0) // if the row isnt terminated with a size of 0 or the key val 255
		{
			std::cout << "Bad line: " << y << std::endl;
			return 1;
		}
	}

	return 0;
}

static void send_usb_msg(char * data_pkt)
{
	char usb_pkt[65];
	usb_pkt[0] = 1;
	for (int i = 1; i < 65; i++)
	{
		usb_pkt[i] = data_pkt[i - 1];
	}
	int c = HidD_SetFeature(dev, usb_pkt, 65);
	if (c != 1)
	{
		std::cout << "Device lost!" << std::endl;
	}
	Sleep(1);
}

static void update_keyboard()
{
	// Perform USB control message to keyboard
	//
	// Request Type:  0x21
	// Request:       0x09
	// Value          0x0300
	// Index:         0x03
	// Size:          64

	data_pkt[0][0] = 0x7F;
	data_pkt[0][1] = 0x01;
	data_pkt[0][2] = 0x3C;

	data_pkt[1][0] = 0x7F;
	data_pkt[1][1] = 0x02;
	data_pkt[1][2] = 0x3C;

	data_pkt[2][0] = 0x7F;
	data_pkt[2][1] = 0x03;
	data_pkt[2][2] = 0x3C;

	data_pkt[3][0] = 0x7F;
	data_pkt[3][1] = 0x04;
	data_pkt[3][2] = 0x24;

	data_pkt[4][0] = 0x07;
	data_pkt[4][1] = 0x27;
	data_pkt[4][4] = 0xD8;

	for (int i = 0; i < 60; i++)
	{
		data_pkt[0][i + 4] = red_val[i * 2 + 1] << 4 | red_val[i * 2];
	}

	for (int i = 0; i < 12; i++)
	{
		data_pkt[1][i + 4] = red_val[i * 2 + 121] << 4 | red_val[i * 2 + 120];
	}

	for (int i = 0; i < 48; i++)
	{
		data_pkt[1][i + 16] = grn_val[i * 2 + 1] << 4 | grn_val[i * 2];
	}

	for (int i = 0; i < 24; i++)
	{
		data_pkt[2][i + 4] = grn_val[i * 2 + 97] << 4 | grn_val[i * 2 + 96];
	}

	for (int i = 0; i < 36; i++)
	{
		data_pkt[2][i + 28] = blu_val[i * 2 + 1] << 4 | blu_val[i * 2];
	}

	for (int i = 0; i < 36; i++)
	{
		data_pkt[3][i + 4] = blu_val[i * 2 + 73] << 4 | blu_val[i * 2 + 72];
	}

	send_usb_msg(data_pkt[0]);
	send_usb_msg(data_pkt[1]);
	send_usb_msg(data_pkt[2]);
	send_usb_msg(data_pkt[3]);
	send_usb_msg(data_pkt[4]);

}

//==================================================================================================
// Code by http://www.reddit.com/user/chrisgzy
//==================================================================================================
bool IsMatchingDevice(wchar_t *pDeviceID, unsigned int uiVID, unsigned int uiPID, unsigned int uiMI)
{
	unsigned int uiLocalVID = 0, uiLocalPID = 0, uiLocalMI = 0;

	wchar_t *context1 = NULL;
	LPWSTR pszToken = wcstok_s(pDeviceID, L"\\#&", &context1);
	while (pszToken)
	{
		std::wstring tokenStr(pszToken);
		if (tokenStr.find(L"VID_", 0, 4) != std::wstring::npos)
		{
			std::wistringstream iss(tokenStr.substr(4));
			iss >> std::hex >> uiLocalVID;
		}
		else if (tokenStr.find(L"PID_", 0, 4) != std::wstring::npos)
		{
			std::wistringstream iss(tokenStr.substr(4));
			iss >> std::hex >> uiLocalPID;
		}
		else if (tokenStr.find(L"MI_", 0, 3) != std::wstring::npos)
		{
			std::wistringstream iss(tokenStr.substr(3));
			iss >> std::hex >> uiLocalMI;
		}

		pszToken = wcstok_s(0, L"\\#&", &context1);
	}

	if (uiVID != uiLocalVID || uiPID != uiLocalPID || uiMI != uiLocalMI)
		return false;

	return true;
}


//==================================================================================================
// Code by http://www.reddit.com/user/chrisgzy
//==================================================================================================
HANDLE GetDeviceHandle(unsigned int uiVID, unsigned int uiPID, unsigned int uiMI)
{
	const GUID GUID_DEVINTERFACE_HID = { 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 };
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_HID, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE)
		return 0;

	HANDLE hReturn = 0;

	SP_DEVINFO_DATA deviceData = { 0 };
	deviceData.cbSize = sizeof(SP_DEVINFO_DATA);

	for (unsigned int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &deviceData); ++i)
	{
		wchar_t wszDeviceID[MAX_DEVICE_ID_LEN];
		if (CM_Get_Device_IDW(deviceData.DevInst, wszDeviceID, MAX_DEVICE_ID_LEN, 0))
			continue;

		if (!IsMatchingDevice(wszDeviceID, uiVID, uiPID, uiMI))
			continue;

		SP_INTERFACE_DEVICE_DATA interfaceData = { 0 };
		interfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

		if (!SetupDiEnumDeviceInterfaces(hDevInfo, &deviceData, &GUID_DEVINTERFACE_HID, 0, &interfaceData))
			break;

		DWORD dwRequiredSize = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, 0, 0, &dwRequiredSize, 0);

		SP_INTERFACE_DEVICE_DETAIL_DATA *pData = (SP_INTERFACE_DEVICE_DETAIL_DATA *)new unsigned char[dwRequiredSize];
		pData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, pData, dwRequiredSize, 0, 0))
		{
			delete pData;
			break;
		}

		HANDLE hDevice = CreateFile(pData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			delete pData;
			break;
		}

		hReturn = hDevice;
		break;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);


	return hReturn;
}
