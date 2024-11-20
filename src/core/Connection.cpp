#include "../../include/Connection.hpp"

Connection::Connection() : client_fd(-1), server_config(ServerConfig())
{
}

Connection::Connection(int fd, ServerConfig &server_config) : client_fd(fd), server_config(server_config), keep_alive(false), last_activity(time(0)), timeout(5) {};

Connection &Connection::operator=(const Connection &other)
{
	if (this != &other)
	{
		this->client_fd = other.client_fd;
		this->server_config = other.server_config;
		this->keep_alive = other.keep_alive;
		this->last_activity = other.last_activity;
		this->timeout = other.timeout;
		this->buffer = other.buffer;
	}
	return *this;
};

Connection::~Connection()
{
	if (client_fd > 0)
		close(client_fd);
};

void Connection::handleRequest()
{
	char buf[server_config.body_size];
	int bytes_read = recv(client_fd, buf, server_config.body_size, 0);

	if (bytes_read < 0)
	{
		perror("recv");
		return;
	}

	timeout = 2;
	server_config.body_size = 1500;

	Logger::log(Logger::DEBUG, buf);
};

bool Connection::isTimedOut()
{
	return time(0) - last_activity > timeout;
};

int Connection::getTimeout()
{
	return timeout;
};

int Connection::getBodySize()
{
	return server_config.body_size;
};
