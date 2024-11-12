#include <iostream>
#include "../include/ConfigParser.hpp"

int main()
{
	ConfigParser parser("server.conf");
	try
	{
		Config config = parser.parseConfig();
		for (std::vector<ServerConfig>::iterator it = config.servers.begin(); it != config.servers.end(); ++it)
		{
			std::cout << "Server name: " << it->server_name << std::endl;
			std::cout << "Host: " << it->host << std::endl;
			std::cout << "Port: " << it->port << std::endl;
			std::cout << "Root: " << it->root << std::endl;
			std::cout << "Client max body size: " << it->client_max_body_size << std::endl;
			std::cout << "Error pages:" << std::endl;
			for (std::map<int, std::string>::iterator it2 = it->error_pages.begin(); it2 != it->error_pages.end(); ++it2)
				std::cout << it2->first << " " << it2->second << std::endl;
			std::cout << "Locations:" << std::endl;
			for (std::map<std::string, LocationConfig>::iterator it2 = it->locations.begin(); it2 != it->locations.end(); ++it2)
			{
				std::cout << "Path: " << it2->first << std::endl;
				std::cout << "Root: " << it2->second.root << std::endl;
				std::cout << "Index: " << it2->second.index << std::endl;
				std::cout << "Methods:" << std::endl;
				for (std::vector<std::string>::iterator it3 = it2->second.methods.begin(); it3 != it2->second.methods.end(); ++it3)
					std::cout << *it3 << std::endl;
				std::cout << "Cgi pass: " << it2->second.cgi_pass << std::endl;
			}

			std::cout << std::endl;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}