#include "stdafx.h"
#include "KeyboardLayout.h"
#include <fstream>
#include <string>

#include "dirent.h"

KeyboardLayout::KeyboardLayout()
{
	GetKeyboardLayout();

	KeyVec = 
	{
		137, 8, 20, 255,
		0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 6, 18, 30, 42, 32, 44, 56, 68, 255,
		1, 13, 25, 37, 49, 61, 73, 85, 97, 109, 121, 133, 7, 31, 54, 66, 78, 80, 92, 104, 116, 255,
		2, 14, 26, 38, 50, 62, 74, 86, 98, 110, 122, 134, 90, 102, 43, 55, 67, 9, 21, 33, 128, 255,
		3, 15, 27, 39, 51, 63, 75, 87, 99, 111, 123, 135, 114, 126, 57, 69, 81, 128, 255,
		4, 16, 28, 40, 52, 64, 76, 88, 100, 112, 124, 136, 79, 103, 93, 105, 117, 140, 255,
		5, 17, 29, 53, 89, 101, 113, 91, 115, 127, 139, 129, 141, 140, 255,
	};

	SizeVec = 
	{
		-15.5, 1, 1, -2.5, 1, -2, 0,
		1, -.5, 1, 1, 1, 1, -.75, 1, 1, 1, 1, -.75, 1, 1, 1, 1, -.5, 1, 1, 1, -.5, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, -.5, 1, 1, 1, -.5, 1, 1, 1, 1, 0,
		1.5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.5, -.5, 1, 1, 1, -.5, 1, 1, 1, 1, 0,
		1.75, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.25, -4, 1, 1, 1, 1, 0,
		1.25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2.75, -1.5, 1, -1.5, 1, 1, 1, 1, 0,
		1.5, 1, 1.25, 6.5, 1.25, 1, 1, 1.5, -.5, 1, 1, 1, -.5, 2, 1, 1, 0,
	};
}


KeyboardLayout::~KeyboardLayout()
{
}

//needs to be the physical keyboard layout
void KeyboardLayout::GetKeyboardLayout()
{
	//SizeVec
	//KeynumMap
	//SizeVec

	Layout = std::to_string(reinterpret_cast<int>(::GetKeyboardLayout(0)));

	if (!CheckLayoutFile()) /// use default layout
	{
		std::cout << "No Layout Found. Trying Default.";
	}
	else // use layout file
	{

	}
}

bool KeyboardLayout::CheckLayoutFile()
{
	DIR *dpdf;
	struct dirent *epdf;

	dpdf = opendir("layouts");

	if (dpdf != NULL)
	{
		int i = 0;
		while (epdf = readdir(dpdf))
		{
			if (Layout == epdf->d_name) // ewwww
			{
				return true;
			}
		}
	}

	return false;
}