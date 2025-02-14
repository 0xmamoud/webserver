#include "../../include/Logger.hpp"

void Logger::log(Logger::LogLevel level, const std::string &message)
{
	const char *color = getColor(level);
	std::string prefix = getPrefix(level);

	std::cout << color << "[" << prefix << "] " << message << COLOR_RESET << std::endl;
}

const char *Logger::getColor(Logger::LogLevel level)
{
	switch (level)
	{
	case INFO:
		return COLOR_GREEN;
	case WARNING:
		return COLOR_YELLOW;
	case ERROR:
		return COLOR_RED;
	case DEBUG:
		return COLOR_BLUE;
	default:
		return COLOR_RESET;
	}
}

std::string Logger::getPrefix(Logger::LogLevel level)
{
	switch (level)
	{
	case INFO:
		return "INFO";
	case WARNING:
		return "WARNING";
	case ERROR:
		return "ERROR";
	case DEBUG:
		return "DEBUG";
	default:
		return "UNKNOWN";
	}
}
