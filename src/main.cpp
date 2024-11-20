#include <iostream>
#include "../include/ConfigParser.hpp"
#include "../include/Server.hpp"
#include "../include/Logger.hpp"

int main()
{
	try
	{
		ConfigParser parser("server.conf");
		Config config = parser.parseConfig();

		Server server(config);
		server.run();
	}
	catch (const std::exception &e)
	{
		Logger::log(Logger::ERROR, e.what());
		return 1;
	}

	return 0;
}