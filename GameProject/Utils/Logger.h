#pragma once

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <memory>
#include <string>

// Print function to for info, warnings and errors.
#define LOG_INFO(...)				Logger::printInfo(__VA_ARGS__)
#define LOG_PRINT(...)				LOG_INFO(__VA_ARGS__)
#define LOG_WARNING(...)			Logger::printWarning(__VA_ARGS__)
#define LOG_ERROR(...)				Logger::printError(__VA_ARGS__)

// Restrict printing by only print certain types. 
#define LOG_SET_FILTER(...)			Logger::setFilter(__VA_ARGS__)
#define LOG_TYPE_INFO				0x01
#define LOG_TYPE_WARNING			0x02
#define LOG_TYPE_ERROR				0x04

/*
Restrict printing by only print certain types.
To only print warnings and errors:
	LOG_FILTER_WARNING_ERROR;
*/
#define LOG_FILTER_INFO				LOG_SET_FILTER(LOG_TYPE_INFO)
#define LOG_FILTER_WARNING			LOG_SET_FILTER(LOG_TYPE_WARNING)
#define LOG_FILTER_ERROR			LOG_SET_FILTER(LOG_TYPE_ERROR)
#define LOG_FILTER_WARNING_ERROR	LOG_SET_FILTER(LOG_TYPE_WARNING | LOG_TYPE_ERROR)

#define PATH_TO_LOG_FILE "../log.txt"

class Logger
{
public:
	static void init();
	static void destroy();

	template<typename ...Args>
	static void printInfo(const std::string& str, Args ...args);
	template<typename ...Args>
	static void printWarning(const std::string& str, Args ...args);
	template<typename ...Args>
	static void printError(const std::string& str, Args ...args);

	static void setFilter(unsigned int filterTypes);
	static void logToFile(bool toFile = true);

	enum TYPE
	{
		TYPE_INFO = 1,
		TYPE_WARNING = 2,
		TYPE_ERROR = 4
	};

private:
	enum CONSOLE_COLOR
	{
		GREEN = 10,
		BLUE = 11,
		RED,
		PURPLE,
		YELLOW,
		WHITE
	};

	static bool shouldPrint(TYPE type);
	static std::string getCurrentTime();

	template<typename ...Args>
	static void print(TYPE type, const std::string& str, CONSOLE_COLOR color, Args ...args);
	template<typename ...Args>
	static void printToFile(TYPE type, const std::string& str, Args ...args);
	template<typename ...Args>
	static std::string formatString(const std::string& str, Args ...args);

	static HANDLE hstdin, hstdout;
	static CONSOLE_SCREEN_BUFFER_INFO csbi;
	static unsigned int filter;

	static bool writeToFile;
	static std::ofstream file;
};

template<typename ...Args>
inline void Logger::printInfo(const std::string & str, Args ...args)
{
	if (writeToFile)
		printToFile(TYPE_INFO, str, CONSOLE_COLOR::WHITE, args...);
	print(TYPE_INFO, str, CONSOLE_COLOR::WHITE, args...);
}

template<typename ...Args>
inline void Logger::printWarning(const std::string & str, Args ...args)
{
	if (writeToFile)
		printToFile(TYPE_WARNING, str, CONSOLE_COLOR::YELLOW, args...);
	print(TYPE_WARNING, str, CONSOLE_COLOR::YELLOW, args...);
}

template<typename ...Args>
inline void Logger::printError(const std::string & str, Args ...args)
{
	if(writeToFile)
		printToFile(TYPE_ERROR, str, CONSOLE_COLOR::RED, args...);
	print(TYPE_ERROR, str, CONSOLE_COLOR::RED, args...);
}

template<typename ...Args>
inline void Logger::print(TYPE type, const std::string & str, CONSOLE_COLOR color, Args ...args)
{
	if (shouldPrint(type))
	{
		// Tell the user how to stop
		SetConsoleTextAttribute(hstdout, color);
		switch (type)
		{
		case TYPE_ERROR:
		{
			const std::string s = "[ERROR] " + str + "\n";
			printf(s.c_str(), args...);
		}
		break;
		case TYPE_WARNING:
		{
			const std::string s = "[WARNING] " + str + "\n";
			printf(s.c_str(), args...);
		}
		break;
		default:
		{
			const std::string s = "[INFO] " + str + "\n";
			printf(s.c_str(), args...);
		}
		break;
		}

		FlushConsoleInputBuffer(hstdin);
		SetConsoleTextAttribute(hstdout, csbi.wAttributes);
	}
}

template<typename ...Args>
inline void Logger::printToFile(TYPE type, const std::string & str, Args ...args)
{
	if (shouldPrint(type))
	{
		std::string formattedStr;
		std::string currTime = getCurrentTime();
		
		switch (type)
		{
		case TYPE_ERROR:
		{
			const std::string s = "[ERROR] " + currTime + ": " + str + "\n";
			formattedStr = formatString(s.c_str(), args...);
		}
		break;
		case TYPE_WARNING:
		{
			const std::string s = "[WARNING] " + currTime + ": " + str + "\n";
			formattedStr = formatString(s.c_str(), args...);
		}
		break;
		default:
		{
			const std::string s = "[INFO] " + currTime + ": " + str + "\n";
			formattedStr = formatString(s.c_str(), args...);
		}
		break;
		}
		file << formattedStr.c_str();
	}
}

template<typename ...Args>
inline std::string Logger::formatString(const std::string & str, Args ...args)
{
	size_t size = snprintf(nullptr, 0, str.c_str(), args...) + 1;
	std::unique_ptr<char[]> buff(new char[size]);
	snprintf(buff.get(), size, str.c_str(), args...);
	return std::string(buff.get(), buff.get() + size - 1);
}
