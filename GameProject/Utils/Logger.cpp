#include "Logger.h"

#include <chrono>
#include <ctime>

HANDLE Logger::hstdin = (void*)0;
HANDLE Logger::hstdout = (void*)0;
CONSOLE_SCREEN_BUFFER_INFO Logger::csbi = {};
unsigned int Logger::filter = 0;
std::ofstream Logger::file;
bool Logger::writeToFile = false;

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
	
	auto t = std::chrono::system_clock::now();
	std::time_t t2 = std::chrono::system_clock::to_time_t(t);
	char buff[255] = {0};
	ctime_s(buff, 255, &t2);
	int i = 0;
	while (i < 255 && buff[i] != '\0')
	{
		if (buff[i] == '\n')
		{
			buff[i] = '\0';
			i = 256;
		}
		i++;
	}
	return std::string(buff);
	
	/*
	std::time_t t = std::time(0);
	struct tm buff;
	char str[255] = { 0 };
	asctime_s(str, sizeof str, localtime_s(&buff, &t));
	std::tm* now = ;
	return std::to_string(now->tm_hour) + ":" + std::to_string(now->tm_min) + ":" + std::to_string(now->tm_sec);
	*/
	
}
