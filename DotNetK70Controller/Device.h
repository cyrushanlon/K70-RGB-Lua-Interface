#pragma once
#include <vector>
#include <thread>

#include <lua.hpp>

class Device
{
private:
	HANDLE DeviceHandle;

	std::vector<unsigned char> KeyVec;
	std::vector<float> SizeVec;

	unsigned char led_matrix[7][92];

	char red_val[144];
	char grn_val[144];
	char blu_val[144];

	char data_pkt[5][64];
	
	HANDLE GetDeviceHandle(unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);
	bool IsMatchingDevice(wchar_t *pDeviceID, unsigned int uiVID, unsigned int uiPID, unsigned int uiMI);

	bool IsDeviceValid;
	void SendUSBMsg(char * data_pkt);

	std::thread RunThread;
	bool StopRun;
	void RunFunction();

public:
	int SetLed(int x, int y, int r, int g, int b);
	int SetLed(int n, int r, int g, int b);
	void UpdateDevice();
	bool InitKeyboard();
	void Run();

	HANDLE GetDeviceHandle();

	//bool GetMouse(); //look into mouse later
	Device();
	~Device();
};