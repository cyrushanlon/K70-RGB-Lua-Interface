#pragma once
#include <vector>

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
	bool SetLed(int x, int y, int r, int g, int b);

	void SendUSBMsg(char * data_pkt);
	void UpdateDevice();
public:
	bool InitKeyboard();
	void Run();
	//bool GetMouse(); //look into mouse later
	Device();
	~Device();
};

