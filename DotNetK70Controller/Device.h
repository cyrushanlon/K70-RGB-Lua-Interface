#pragma once
#include <vector>
#include <thread>

#include <lua.hpp>
#include "KeyboardLayout.h"

class Device
{
private:
	HANDLE DeviceHandle;
	KeyboardLayout Layout;

	//std::vector<unsigned char> KeyVec;
	//std::vector<float> SizeVec;

	unsigned char led_matrix[7][92];

	char red_val[144];
	char grn_val[144];
	char blu_val[144];

	unsigned char data_pkt[5][64];
	
	HANDLE GetDeviceHandle(unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);
	bool IsMatchingDevice(wchar_t *pDeviceID, unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);

	void SendUSBMsg(unsigned char * data_pkt);

	std::thread RunThread;
	bool StopRun;
	void RunFunction();

public:
	bool SetLed(int x, int y, int r, int g, int b);
	bool SetLed(int n, int r, int g, int b);
	void UpdateDevice();
	bool InitKeyboard();
	void Run();

	HANDLE GetDeviceHandle();

	//bool GetMouse(); //look into mouse later
	Device();
	~Device();
};