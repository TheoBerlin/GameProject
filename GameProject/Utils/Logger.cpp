#include "Logger.h"

#include <chrono>
#include <ctime>

HANDLE Logger::hstdin = (void*)0;
HANDLE Logger::hstdout = (void*)0;
CONSOLE_SCREEN_BUFFER_INFO Logger::csbi = {};
unsigned int Logger::filter = 0;
std::ofstream Logger::file;
bool Logger::writeToFile = true;

void Logger::init()
{
	hstdin = GetStdHandle(STD_INPUT_HANDLE);
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// Remember how things were when we started
	GetConsoleScreenBufferInfo(hstdout, &csbi);

	file.open(PATH_TO_LOG_FILE);
}

void Logger::destroy()
{
	file.close();
}

void Logger::setFilter(unsigned int filterTypes)
{
	filter = filterTypes;
}

void Logger::logToFile(bool toFile)
{
	writeToFile = toFile;
}

bool Logger::shouldPrint(TYPE type)
{
	if (filter == 0)
		return true;
	if ((filter & type) == TYPE::TYPE_INFO)
		return true;
	if ((filter & type) == TYPE::TYPE_WARNING)
		return true;
	if ((filter & type) == TYPE::TYPE_ERROR)
		return true;
	return false;
}

std::string Logger::getCurrentTime()
{
	std::time_t t = std::time(0);
	struct tm now;
	char str[255] = { 0 };
	localtime_s(&now, &t);
	return std::to_string(now.tm_hour) + ":" + std::to_string(now.tm_min) + ":" + std::to_string(now.tm_sec) + ":";
	
	
}
