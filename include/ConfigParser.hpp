#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

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

class ConfigParser
{
private:
	const std::string filepath;
	bool inside_location_block;
	bool inside_server_block;

	std::string trim(const std::string &str);
	std::string getValue(const std::string &line);
	std::vector<std::string> split(const std::string &str, const char delim);

	void handleNewServer(Config &config, ServerConfig &current_server);
	void handleClosingBracket(Config &config, ServerConfig &current_server, LocationConfig &current_location);
	void handleBlocks(ServerConfig &current_server, LocationConfig &current_location, const std::string &line);
	void handleNewLocation(ServerConfig &current_server, LocationConfig &current_location, const std::string &line);
	void handleServerConfig(ServerConfig &current_server, const std::string &line);
	void handleLocationConfig(LocationConfig &current_location, const std::string &line);

	void normalizeServerConfig(Config &config);
	void normalizeLocationConfig(Config &config);

public:
	ConfigParser();
	ConfigParser(const std::string &path);
	ConfigParser(const ConfigParser &other);
	ConfigParser &operator=(const ConfigParser &other);
	~ConfigParser();

	Config parseConfig();

	class InvalidConfigException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};
};

#endif