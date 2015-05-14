#pragma once
#include<map>
#include<string>
#include<utility>

struct coord
{
	int x;
	int y;
};
std::map<std::string, std::pair<int, int> > KeynumMap = 
{ 
	{ "ESC", std::make_pair(0, 1) },	{ "F1", std::make_pair(7, 1) },		{ "F2", std::make_pair(12, 1) },	{ "F3", std::make_pair(17, 1) },	{ "F4", std::make_pair(22, 1) },{ "F5", std::make_pair(30, 1) },{ "F6", std::make_pair(35, 1) },
	{ "TILDE", std::make_pair(0, 2) },	{ "1", std::make_pair(5, 2) },		{ "2", std::make_pair(10, 2) },		{ "3", std::make_pair(15, 2) },		{ "4", std::make_pair(20, 2) }, { "5", std::make_pair(25, 2) }, { "6", std::make_pair(30, 2) },
	{ "TAB", std::make_pair(0, 3) },	{ "Q", std::make_pair(7, 3) },		{ "W", std::make_pair(12, 3) },		{ "E", std::make_pair(17, 3) },		{ "R", std::make_pair(22, 3) }, { "T", std::make_pair(27, 3) },	{ "Y", std::make_pair(32, 3) },
	{ "CAPSLOCK", std::make_pair(0, 4)},{ "A", std::make_pair(8, 4) },		{ "S", std::make_pair(13, 4) },		{ "D", std::make_pair(18, 4) },		{ "F", std::make_pair(23, 4) }, { "G", std::make_pair(28, 4) },	{ "H", std::make_pair(33, 4) },
	{ "LSHIFT", std::make_pair(0, 5) }, { "BSLASH", std::make_pair(6, 5) }, { "Z", std::make_pair(11, 5) },		{ "X", std::make_pair(16, 5) },		{ "C", std::make_pair(21, 1) },	{ "V", std::make_pair(26, 5) },	{ "B", std::make_pair(31, 5) },
	{ "LCTRL", std::make_pair(0, 6) },	{ "LWIN", std::make_pair(7, 6) },	{ "LALT", std::make_pair(12, 6) },	{ "SPACE", std::make_pair(17,6) },


}