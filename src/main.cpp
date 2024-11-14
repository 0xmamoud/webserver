#include <iostream>
#include "../include/ConfigParser.hpp"
#include "../include/Server.hpp"

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
		std::cerr << e.what() << '\n';
	}

	return 0;
}