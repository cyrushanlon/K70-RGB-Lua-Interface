#pragma once

#include<map>
#include<vector>

class KeyboardLayout
{
public:
	KeyboardLayout();
	~KeyboardLayout();

	//combine KeynumMap and Keyvec?

	//holds windows key enum and position
	std::map<int, std::pair<int, int> > KeynumMap;
	//holds key character
	std::vector<unsigned char> KeyVec;
	//Holds keyboard spacing
	std::vector<float> SizeVec;

private:
	std::string Layout;
	void GetKeyboardLayout();
	bool CheckLayoutFile();
};

