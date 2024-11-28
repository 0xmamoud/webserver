#include "../../include/Utility.hpp"

std::vector<std::string> split(const std::string &str, const std::string &delim)
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = 0;

	while ((end = str.find(delim, start)) != std::string::npos)
	{
		std::string token = str.substr(start, end - start);
		if (!token.empty())
		{
			tokens.push_back(token);
		}
		start = end + delim.length();
	}

	std::string lastToken = str.substr(start);
	if (!lastToken.empty())
	{
		tokens.push_back(lastToken);
	}

	return tokens;
}

std::string trim(const std::string &str, const std::string &whitespace)
{
	size_t start = str.find_first_not_of(whitespace);
	size_t end = str.find_last_not_of(whitespace);

	if (start == std::string::npos || end == std::string::npos)
	{
		return "";
	}

	return str.substr(start, end - start + 1);
}

std::string getDate()
{
	std::time_t now = std::time(0);
	std::tm *gmtm = std::gmtime(&now);

	char buf[30];
	std::strftime(buf, 30, "%a, %d %b %Y %H:%M:%S GMT", gmtm);

	return std::string(buf);
}