#pragma once
#include <ctime>

static std::string GetTime() // needs moving somewhere else (In device.cpp as well)
{
	time_t CurrentTime;
	struct tm localTime;

	time(&CurrentTime);
	localtime_s(&localTime, &CurrentTime);

	std::ostringstream oss;
	oss << "[" << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << "] ";

	return oss.str();
}