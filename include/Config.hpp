#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>

struct LocationConfig
{
	std::string path;
	std::string root;
	std::string index;
	int autoindex;
	std::vector<std::string> methods;
	std::string cgi_pass;
};

struct ServerConfig
{
	std::string server_name;
	std::string host;
	int port;
	std::string root;
	int body_size;
	std::map<int, std::string> error_pages;
	std::map<std::string, LocationConfig> locations;
};

struct Config
{
	std::vector<ServerConfig> servers;
};

#endif