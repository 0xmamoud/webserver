#include <iostream>
#include "../include/ConfigParser.hpp"
#include "../include/Server.hpp"
#include "../include/Logger.hpp"
#include "../include/SignalHandler.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	try
	{

		signal(SIGINT, SignalHandler::handleStop);
		ConfigParser parser(argv[1]);
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