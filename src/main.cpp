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

		// for (std::vector<ServerConfig>::iterator it = config.servers.begin(); it != config.servers.end(); ++it)
		// {
		// 	std::cout << "Server Name: " << it->server_name << std::endl;
		// 	std::cout << "Host: " << it->host << std::endl;
		// 	std::cout << "Port: " << it->port << std::endl;
		// 	std::cout << "Root: " << it->root << std::endl;
		// 	std::cout << "Body Size: " << it->body_size << std::endl;
		// 	for (std::map<int, std::string>::iterator it2 = it->error_pages.begin(); it2 != it->error_pages.end(); ++it2)
		// 	{
		// 		std::cout << "Error Page: " << it2->first << " " << it2->second << std::endl;
		// 	}
		// 	for (std::map<std::string, LocationConfig>::iterator it2 = it->locations.begin(); it2 != it->locations.end(); ++it2)
		// 	{
		// 		std::cout << "Location Path: " << it2->first << std::endl;
		// 		std::cout << "Root: " << it2->second.root << std::endl;
		// 		std::cout << "Index: " << it2->second.index << std::endl;
		// 		std::cout << "Autoindex: " << it2->second.autoindex << std::endl;
		// 		for (std::vector<std::string>::iterator it3 = it2->second.methods.begin(); it3 != it2->second.methods.end(); ++it3)
		// 		{
		// 			std::cout << "Method: " << *it3 << std::endl;
		// 		}
		// 		std::cout << "CGI Path: " << it2->second.cgi_path << std::endl;
		// 		std::cout << "CGI Bin: " << it2->second.cgi_bin << std::endl;
		// 		std::cout << "Upload Path: " << it2->second.upload_path << std::endl;
		// 		std::cout << "Redirect: " << it2->second.redirect << std::endl;
		// 	}
		// 	std::cout << "============================================" << std::endl;
		// }

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