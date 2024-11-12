#include "../../include/ConfigParser.hpp"

ConfigParser::ConfigParser() : filepath(""), inside_location_block(false), inside_server_block(false) {}

ConfigParser::ConfigParser(const std::string &path) : filepath(path), inside_location_block(false), inside_server_block(false) {}

ConfigParser::ConfigParser(const ConfigParser &other) { *this = other; }

ConfigParser &ConfigParser::operator=(const ConfigParser &other)
{
	(void)other;
	return *this;
}

ConfigParser::~ConfigParser() {}

Config ConfigParser::parseConfig()
{
	std::ifstream file(this->filepath.c_str());
	if (!file.is_open())
		throw InvalidConfigException();

	Config config;
	std::string line;
	ServerConfig current_server;
	LocationConfig current_location;

	while (std::getline(file, line))
	{
		line = this->trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		if (line == "server {")
			handleNewServer(config, current_server);
		else if (line == "}")
			handleClosingBracket(config, current_server, current_location);
		else if (this->inside_server_block)
			handleBlocks(current_server, current_location, line);
	}

	file.close();

	return config;
};

void ConfigParser::handleNewServer(Config &config, ServerConfig &current_server)
{
	if (this->inside_server_block)
		config.servers.push_back(current_server);
	current_server = ServerConfig();
	this->inside_server_block = true;
};

void ConfigParser::handleClosingBracket(Config &config, ServerConfig &current_server, LocationConfig &current_location)
{
	if (this->inside_location_block)
	{
		current_server.locations[current_location.path] = current_location;
		this->inside_location_block = false;
	}
	else if (this->inside_server_block)
	{
		config.servers.push_back(current_server);
		this->inside_server_block = false;
	}
};

void ConfigParser::handleBlocks(ServerConfig &current_server, LocationConfig &current_location, const std::string &line)
{
	if (line.find("location") != std::string::npos)
		handleNewLocation(current_server, current_location, line);
	else if (this->inside_location_block)
		handleLocationConfig(current_location, line);
	else
		handleServerConfig(current_server, line);
};

void ConfigParser::handleNewLocation(ServerConfig &current_server, LocationConfig &current_location, const std::string &line)
{
	if (this->inside_location_block)
		current_server.locations[current_location.path] = current_location;
	current_location = LocationConfig();
	std::vector<std::string> tokens = split(line, ' ');
	current_location.path = tokens[1];
	this->inside_location_block = true;
};

void ConfigParser::handleLocationConfig(LocationConfig &current_location, const std::string &line)
{
	if (line.find("root") != std::string::npos)
		current_location.root = getValue(line);
	else if (line.find("index") != std::string::npos)
		current_location.index = getValue(line);
	else if (line.find("methods") != std::string::npos)
		current_location.methods = split(getValue(line), ' ');
	else if (line.find("cgi_pass") != std::string::npos)
		current_location.cgi_pass = getValue(line);
};

void ConfigParser::handleServerConfig(ServerConfig &current_server, const std::string &line)
{
	if (line.find("server_name") != std::string::npos)
		current_server.server_name = getValue(line);
	else if (line.find("host") != std::string::npos)
		current_server.host = getValue(line);
	else if (line.find("port") != std::string::npos)
		current_server.port = std::atoi(getValue(line).c_str());
	else if (line.find("root") != std::string::npos)
		current_server.root = getValue(line);
	else if (line.find("body_size") != std::string::npos)
		current_server.client_max_body_size = std::atoi(getValue(line).c_str());
	else if (line.find("error_page") != std::string::npos)
	{
		std::vector<std::string> tokens = split(line, ' ');
		current_server.error_pages[std::atoi(tokens[1].c_str())] = tokens[2];
	}
};

std::string ConfigParser::trim(const std::string &str)
{
	std::string trimmed = str;
	trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
	trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

	if (!trimmed.empty() && trimmed[trimmed.size() - 1] == ';')
		trimmed.erase(trimmed.size() - 1);
	return trimmed;
};

std::string ConfigParser::getValue(const std::string &line)
{
	std::string value = line.substr(line.find_first_of(" \t") + 1);
	return this->trim(value);
};

std::vector<std::string> ConfigParser::split(const std::string &str, const char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		tokens.push_back(this->trim(item));
	}
	return tokens;
};

const char *ConfigParser::InvalidConfigException::what() const throw()
{
	return "Invalid configuration file";
};
