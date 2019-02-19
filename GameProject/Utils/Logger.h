#pragma once

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <memory>
#include <string>

// Print function for info, warnings and errors.
#define LOG_INFO(...)				Logger::printInfo(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_PRINT(...)				LOG_INFO(__VA_ARGS__)
#define LOG_WARNING(...)			Logger::printWarning(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)				Logger::printError(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_SUCCESS(...)			Logger::printSuccess(__FILE__, __func__, __LINE__, __VA_ARGS__)

// Functions to change the console color.
#define LOG_START_COLOR_PASS(color) Logger::startColorPass(color)
#define LOG_END_COLOR_PASS()		Logger::endColorPass();

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
	enum CONSOLE_COLOR
	{
		GREEN = 10,
		BLUE = 11,
		RED,
		PURPLE,
		YELLOW,
		WHITE
	};

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
		filePathStr: The path to the file which name will be printed.
		line: The line number which will be printed.
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printInfo(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string& str, Args&& ...args);

	/*
	Print a formatted yellow string with [WARNING] as a prefix.
	Arguments:
		filePathStr: The path to the file which name will be printed.
		line: The line number which will be printed.
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printWarning(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string& str, Args&& ...args);

	/*
	Print a formatted red string with [ERROR] as a prefix.
	Arguments:
		filePathStr: The path to the file which name will be printed.
		line: The line number which will be printed.
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printError(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string& str, Args&& ...args);

	/*
	Print a formatted green string with [SUCCESS] as a prefix.
	Arguments:
		filePathStr: The path to the file which name will be printed.
		line: The line number which will be printed.
		str: The formatted string. Eg. "Test: %d"
		args: List of corresponding formateted specifier. Eg. %03.5ld
	*/
	template<typename ...Args>
	static void printSuccess(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string& str, Args&& ...args);

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

	/*
	Set the console color.
	Arguments:
		color: The color to use when coloring the text.
	*/
	static void startColorPass(CONSOLE_COLOR color);

	/*
	Reset the console color.
	*/
	static void endColorPass();

	enum TYPE
	{
		TYPE_INFO = 1,
		TYPE_WARNING = 2,
		TYPE_ERROR = 4,
		TYPE_SUCCESS = 8
	};

private:
	static bool shouldPrint(TYPE type);
	static std::string getCurrentTime();

	/*
	Will format the string and print it to console and to file if writeToFile is true.
	*/
	template<typename ...Args>
	static void formatPrint(TYPE type, const std::string& str, const std::string& fileStr, const std::string& functionStr, unsigned int line, CONSOLE_COLOR color, Args&& ...args);
	template<typename ...Args>
	static void print(const std::string& str, CONSOLE_COLOR color, Args&& ...args);
	template<typename ...Args>
	static void printToFile(const std::string& str, Args&& ...args);
	template<typename ...Args>
	static std::string formatString(const std::string& str, Args&& ...args);

	static HANDLE hstdin, hstdout;
	static CONSOLE_SCREEN_BUFFER_INFO csbi;
	static unsigned int filter;

	static bool writeToFile;
	static std::ofstream file;
};

template<typename ...Args>
inline void Logger::printInfo(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string & str, Args&& ...args)
{
	formatPrint(TYPE_INFO, str, filePathStr, functionStr, line, CONSOLE_COLOR::WHITE, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::printWarning(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string & str, Args&& ...args)
{
	formatPrint(TYPE_WARNING, str, filePathStr, functionStr, line, CONSOLE_COLOR::YELLOW, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::printError(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string & str, Args&& ...args)
{
	formatPrint(TYPE_ERROR, str, filePathStr, functionStr, line, CONSOLE_COLOR::RED, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::printSuccess(const std::string& filePathStr, const std::string& functionStr, unsigned int line, const std::string & str, Args && ...args)
{
	formatPrint(TYPE_SUCCESS, str, filePathStr, functionStr, line, CONSOLE_COLOR::GREEN, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::formatPrint(TYPE type, const std::string & str, const std::string & filePathStr, const std::string& functionStr, unsigned int line, CONSOLE_COLOR color, Args && ...args)
{
	if (shouldPrint(type))
	{
		// Fetch name of file from path.
		std::string fileStr = filePathStr;
		size_t found = fileStr.rfind("/");
		if (found == std::string::npos) {
			found = fileStr.rfind("\\");
			if (found == std::string::npos) {
				found = 0;
			}
		}
		fileStr = fileStr.substr(found + (found != 0 ? 1 : 0));

		std::string formattedStr = fileStr + ", " + functionStr + ", " + std::to_string(line) + ": " + str + "\n";

		static std::string finalStr;
		static std::string typeStr;
		std::string currTimeStr = writeToFile ? (getCurrentTime() + ", ") : "";

		switch (type)
		{
		case TYPE_ERROR:
			typeStr = "[ERROR]   ";
		break;
		case TYPE_WARNING:
			typeStr = "[WARNING] ";
		break;
		case TYPE_SUCCESS:
			typeStr = "[SUCCESS] ";
		break;
		default:
			typeStr = "[INFO]    ";
		break;
		}

		if (writeToFile) {
			finalStr = typeStr + currTimeStr + formattedStr;
			printToFile(finalStr, std::forward<Args>(args)...);
		}
		finalStr = typeStr + formattedStr;
		print(finalStr, color, std::forward<Args>(args)...);
	}
}

template<typename ...Args>
inline void Logger::print(const std::string & str, CONSOLE_COLOR color, Args&& ...args)
{
	startColorPass(color);
	printf(str.c_str(), args...);
	endColorPass();
}

template<typename ...Args>
inline void Logger::printToFile(const std::string & str, Args&& ...args)
{
	std::string formattedStr = formatString(str, std::forward<Args>(args)...);
	file << formattedStr.c_str();
}

template<typename ...Args>
inline std::string Logger::formatString(const std::string & str, Args&& ...args)
{
	size_t size = snprintf(nullptr, 0, str.c_str(), std::forward<Args>(args)...) + 1;
	std::unique_ptr<char[]> buff(new char[size]);
	snprintf(buff.get(), size, str.c_str(), std::forward<Args>(args)...);
	return std::string(buff.get(), buff.get() + size - 1);
}
