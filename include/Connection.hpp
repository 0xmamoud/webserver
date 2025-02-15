#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <string.h>
#include <map>
#include <vector>
#include "Config.hpp"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include "Logger.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Connection
{
private:
	int client_fd;
	ServerConfig &server_config;
	bool keep_alive;
	bool data_chunked;
	time_t last_activity;
	int timeout;
	std::string buffer;

	bool isRequestComplete();
	void closeConnection();
	void manageClientActivity();
	void parseBuffer(std::string &buf);
	void setBufferChunks(const std::string &body);
	ServerConfig getServerConfig(HttpRequest &request);

	std::string readRequestData();

public:
	Connection(int fd, ServerConfig &server_config);
	// Connection &operator=(const Connection &other);
	~Connection();

	void handleRequest();
	bool isTimedOut();
	bool isClosed();
	int getTimeout();
	int getBodySize();
};
#endif
