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

	Logger::log(Logger::INFO, "Setting up configuration file...");

	std::ifstream file(this->filepath.c_str());
	if (!file.is_open())
		throw InvalidConfigException();

	Logger::log(Logger::INFO, "Configuration file loaded successfully");

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
			this->handleNewServer(config, current_server);
		else if (line == "}")
			this->handleClosingBracket(config, current_server, current_location);
		else if (this->inside_server_block)
			this->handleBlocks(current_server, current_location, line);
	}
	file.close();

	Logger::log(Logger::INFO, "Handling redirections...");
	this->handleRedirection(config);
	Logger::log(Logger::INFO, "Redirections handled successfully");

	Logger::log(Logger::INFO, "Normalizing servers configuration...");
	this->normalizeServerConfig(config);
	this->normalizeLocationConfig(config);

	Logger::log(Logger::INFO, "Configuration normalized successfully");

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
		this->handleNewLocation(current_server, current_location, line);
	else if (this->inside_location_block)
		this->handleLocationConfig(current_location, line);
	else
		this->handleServerConfig(current_server, line);
};

void ConfigParser::handleNewLocation(ServerConfig &current_server, LocationConfig &current_location, const std::string &line)
{
	if (this->inside_location_block)
		current_server.locations[current_location.path] = current_location;
	current_location = LocationConfig();
	std::vector<std::string> tokens = this->split(line, ' ');
	current_location.path = tokens[1];
	this->inside_location_block = true;
};

void ConfigParser::handleLocationConfig(LocationConfig &current_location, const std::string &line)
{
	if (line.find("root") != std::string::npos)
		current_location.root = this->getValue(line);
	else if (line.find("autoindex") != std::string::npos)
	{
		std::string value = this->getValue(line);
		if (value == "on")
			current_location.autoindex = 1;
	}
	else if (line.find("index") != std::string::npos)
		current_location.index = this->getValue(line);
	else if (line.find("methods") != std::string::npos)
		current_location.methods = this->split(getValue(line), ' ');
	else if (line.find("cgi_path") != std::string::npos)
		current_location.cgi_path = this->getValue(line);
	else if (line.find("cgi_bin") != std::string::npos)
		current_location.cgi_bin = this->getValue(line);
	else if (line.find("upload_path") != std::string::npos)
		current_location.upload_path = this->getValue(line);
	else if (line.find("redirect") != std::string::npos)
		current_location.redirect = this->getValue(line);
};

void ConfigParser::handleServerConfig(ServerConfig &current_server, const std::string &line)
{
	if (line.find("server_name") != std::string::npos)
		current_server.server_name = this->getValue(line);
	else if (line.find("host") != std::string::npos)
		current_server.host = this->getValue(line);
	else if (line.find("port") != std::string::npos)
		current_server.port = std::atoi(this->getValue(line).c_str());
	else if (line.find("root") != std::string::npos)
		current_server.root = this->getValue(line);
	else if (line.find("body_size") != std::string::npos)
		current_server.body_size = std::atoi(this->getValue(line).c_str());
	else if (line.find("error_page") != std::string::npos)
	{
		std::vector<std::string> tokens = this->split(line, ' ');
		current_server.error_pages[std::atoi(tokens[1].c_str())] = tokens[2];
	}
};

void ConfigParser::normalizeServerConfig(Config &config)
{
	for (std::vector<ServerConfig>::iterator it = config.servers.begin(); it != config.servers.end(); ++it)
	{
		if (it->server_name.empty())
			it->server_name = "localhost:4000";
		if (it->host.empty())
			it->host = "localhost";
		if (it->port == 0)
			throw std::runtime_error("Port number is required");
		if (it->body_size == 0)
			it->body_size = 1000000;
		if (it->error_pages.find(403) == it->error_pages.end())
			it->error_pages[403] = "error_pages/403.html";
		if (it->error_pages.find(404) == it->error_pages.end())
			it->error_pages[404] = "error_pages/404.html";
		if (it->error_pages.find(405) == it->error_pages.end())
			it->error_pages[405] = "error_pages/405.html";
		if (it->error_pages.find(413) == it->error_pages.end())
			it->error_pages[413] = "error_pages/413.html";
		if (it->error_pages.find(500) == it->error_pages.end())
			it->error_pages[500] = "error_pages/500.html";
	}
};

void ConfigParser::normalizeLocationConfig(Config &config)
{
	for (std::vector<ServerConfig>::iterator it = config.servers.begin(); it != config.servers.end(); ++it)
	{
		for (std::map<std::string, LocationConfig>::iterator it2 = it->locations.begin(); it2 != it->locations.end(); ++it2)
		{
			if (it2->second.root.empty())
			{
				if (it->root.empty())
					throw std::runtime_error("No root path specified");
				it2->second.root = it->root;
			}
			if (it2->second.methods.empty())
			{
				it2->second.methods.push_back("GET");
				it2->second.methods.push_back("POST");
				it2->second.methods.push_back("DELETE");
			}
			if (it2->second.upload_path.empty())
				it2->second.upload_path = it2->second.root;
			if (!it2->second.cgi_path.empty() && it2->second.cgi_bin.empty())
				throw std::runtime_error("CGI extension is required");
			if (!it2->second.cgi_bin.empty() && it2->second.cgi_path.empty())
				throw std::runtime_error("CGI path is required");
		}
	}
}

void ConfigParser::handleRedirection(Config &config)
{
	for (std::vector<ServerConfig>::iterator it = config.servers.begin(); it != config.servers.end(); ++it)
	{
		for (std::map<std::string, LocationConfig>::iterator it2 = it->locations.begin(); it2 != it->locations.end(); ++it2)
		{
			if (!it2->second.redirect.empty())
			{
				std::string server_name = it2->second.redirect.substr(0, it2->second.redirect.find('/'));
				ServerConfig redirection = this->findServerConfig(server_name, config);
				it2->second.servers[it2->second.redirect] = redirection;
			}
		}
	};
};

ServerConfig ConfigParser::findServerConfig(const std::string &server_name, const Config &config)
{
	for (std::vector<ServerConfig>::const_iterator it = config.servers.begin(); it != config.servers.end(); ++it)
	{
		if (it->server_name == server_name)
			return *it;
	}
	throw std::runtime_error("No Redirection found, please check your configuration file");
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
;