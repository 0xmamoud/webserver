#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <map>
#include "Config.hpp"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include "Logger.hpp"

class Connection
{
private:
	int client_fd;
	ServerConfig &server_config;
	bool keep_alive;
	time_t last_activity;
	int timeout;
	std::string buffer;

	bool isRequestComplete();

public:
	Connection(int fd, ServerConfig &server_config);
	Connection &operator=(const Connection &other);
	~Connection();

	void handleRequest();
	bool isTimedOut();
	int getTimeout();
	int getBodySize();
};
#endif
