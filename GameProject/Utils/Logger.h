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
#define LOG_SUCCESS(...)			Logger::printSuccess(__VA_ARGS__)

// Restrict printing by only print certain types. 
#define LOG_SET_FILTER(...)			Logger::setFilter(__VA_ARGS__)
#define LOG_TYPE_INFO				0x01
#define LOG_TYPE_WARNING			0x02
#define LOG_TYPE_ERROR				0x04
#define LOG_TYPE_SUCCESS			0x08

/*
Restrict printing by only print certain types.
To only print warnings and errors:
	LOG_FILTER_WARNING_ERROR;
*/
#define LOG_FILTER_INFO				LOG_SET_FILTER(LOG_TYPE_INFO)
#define LOG_FILTER_WARNING			LOG_SET_FILTER(LOG_TYPE_WARNING)
#define LOG_FILTER_ERROR			LOG_SET_FILTER(LOG_TYPE_ERROR)
#define LOG_FILTER_SUCCESS			LOG_SET_FILTER(LOG_TYPE_SUCCESS)
#define LOG_FILTER_WARNING_ERROR	LOG_SET_FILTER(LOG_TYPE_WARNING | LOG_TYPE_ERROR)

#define PATH_TO_LOG_FILE "../log.txt"

class Logger
{
public:
	/*
	Initialize the logger.
	*/
	static void init();
	
	/*
	Clean up the logger.
	*/
	static void destroy();

	/*
	Print a formatted white string with [INFO] as a prefix.
	Arguments:
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printInfo(const std::string& str, Args&& ...args);

	/*
	Print a formatted yellow string with [WARNING] as a prefix.
	Arguments:
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printWarning(const std::string& str, Args&& ...args);

	/*
	Print a formatted red string with [ERROR] as a prefix.
	Arguments:
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printError(const std::string& str, Args&& ...args);

	/*
	Print a formatted green string with [SUCCESS] as a prefix.
	Arguments:
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printSuccess(const std::string& str, Args&& ...args);

	/*
	Show certain information and prevent others to be printed.
	Arguments:
		filterTypes: Flags for the information to be shown. Eg. LOG_TYPE_INFO | LOG_TYPE_ERROR to only print info and error messages.
	*/
	static void setFilter(unsigned int filterTypes);

	/*
	If true, print all messages to the log.txt file also.
	Arguments:
		toFile: If true, print to file, else print only to console.
	*/
	static void logToFile(bool toFile = true);

	enum TYPE
	{
		TYPE_INFO = 1,
		TYPE_WARNING = 2,
		TYPE_ERROR = 4,
		TYPE_SUCCESS = 8
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
	static void print(TYPE type, const std::string& str, CONSOLE_COLOR color, Args&& ...args);
	template<typename ...Args>
	static void printToFile(TYPE type, const std::string& str, Args&& ...args);
	template<typename ...Args>
	static std::string formatString(const std::string& str, Args&& ...args);

	static HANDLE hstdin, hstdout;
	static CONSOLE_SCREEN_BUFFER_INFO csbi;
	static unsigned int filter;

	static bool writeToFile;
	static std::ofstream file;
};

template<typename ...Args>
inline void Logger::printInfo(const std::string & str, Args&& ...args)
{
	if (writeToFile)
		printToFile(TYPE_INFO, str, std::forward<Args>(args)...);
	print(TYPE_INFO, str, CONSOLE_COLOR::WHITE, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::printWarning(const std::string & str, Args&& ...args)
{
	if (writeToFile)
		printToFile(TYPE_WARNING, str, std::forward<Args>(args)...);
	print(TYPE_WARNING, str, CONSOLE_COLOR::YELLOW, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::printError(const std::string & str, Args&& ...args)
{
	if(writeToFile)
		printToFile(TYPE_ERROR, str, CONSOLE_COLOR::RED, std::forward<Args>(args)...);
	print(TYPE_ERROR, str, CONSOLE_COLOR::RED, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::printSuccess(const std::string & str, Args && ...args)
{
	if (writeToFile)
		printToFile(TYPE_SUCCESS, str, std::forward<Args>(args)...);
	print(TYPE_SUCCESS, str, CONSOLE_COLOR::GREEN, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::print(TYPE type, const std::string & str, CONSOLE_COLOR color, Args&& ...args)
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
		case TYPE_SUCCESS:
		{
			const std::string s = "[SUCESS] " + str + "\n";
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
inline void Logger::printToFile(TYPE type, const std::string & str, Args&& ...args)
{
	if (shouldPrint(type))
	{
		std::string formattedStr;
		std::string currTime = getCurrentTime();

		switch (type)
		{
		case TYPE_ERROR:
		{
			const std::string s = "[ERROR]   " + currTime + ": " + str + "\n";
			formattedStr = formatString(s, std::forward<Args>(args)...);
		}
		break;
		case TYPE_WARNING:
		{
			const std::string s = "[WARNING] " + currTime + ": " + str + "\n";
			formattedStr = formatString(s, std::forward<Args>(args)...);
		}
		break;
		default:
		{
			const std::string s = "[INFO]    " + currTime + ": " + str + "\n";
			formattedStr = formatString(s, std::forward<Args>(args)...);
		}
		break;
		}
		file << formattedStr.c_str();
	}
}

template<typename ...Args>
inline std::string Logger::formatString(const std::string & str, Args&& ...args)
{
	size_t size = snprintf(nullptr, 0, str.c_str(), std::forward<Args>(args)...) + 1;
	std::unique_ptr<char[]> buff(new char[size]);
	snprintf(buff.get(), size, str.c_str(), std::forward<Args>(args)...);
	return std::string(buff.get(), buff.get() + size - 1);
}
