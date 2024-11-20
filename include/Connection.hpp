#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <map>
#include "Config.hpp"

class Connection
{
private:
	int client_fd;
	const ServerConfig &server_config;
	bool keep_alive;
	time_t last_activity;
	std::string buffer;

public:
	Connection(int fd, const ServerConfig &server_config);
	Connection(const Connection &other);
	Connection &operator=(const Connection &other);
	~Connection();

	void handleRequest();
	bool isTimedOut();
};
#endif