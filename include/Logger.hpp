#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>

// ANSI color codes
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

class Logger
{
public:
	enum LogLevel
	{
		INFO,
		WARNING,
		ERROR,
		DEBUG
	};

	static void log(LogLevel level, const std::string &message);

private:
	Logger();
	Logger(const Logger &other);
	Logger &operator=(const Logger &other);
	~Logger();

	static const char *getColor(LogLevel level);
	static std::string getPrefix(LogLevel level);
};

#endif