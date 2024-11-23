#include "../../include/Connection.hpp"

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
	memset(buf, 0, server_config.body_size);
	int bytes_read = recv(client_fd, buf, server_config.body_size, 0);

	if (bytes_read < 0)
	{
		perror("recv");
		Logger::log(Logger::ERROR, "Failed to read from socket");
		return;
	}
	else if (bytes_read == 0)
	{
		Logger::log(Logger::INFO, "Connection closed by client");
		close(this->client_fd);
		this->client_fd = -1;
		return;
	}

	this->buffer.append(buf, bytes_read);
	this->manageClientActivity();

	try
	{
		HttpRequest request(this->buffer);
		// HttpResponse response(request);
		// response.sendResponse(client_fd);
	}
	catch (const std::exception &e)
	{
		Logger::log(Logger::ERROR, e.what());
		close(this->client_fd);
		this->client_fd = -1;
		return;
	}
};

bool Connection::isRequestComplete()
{
	return buffer.find("\r\n\r\n") != std::string::npos;
};

bool Connection::isTimedOut()
{
	return time(0) - last_activity > timeout;
};

void Connection::manageClientActivity()
{
	this->last_activity = time(0);
	std::string header = this->buffer.substr(0, buffer.find("\r\n\r\n"));
	if (header.empty())
		return;
	if (header.find("Connection: keep-alive") != std::string::npos)
	{
		this->keep_alive = true;
		return;
	}
	this->keep_alive = false;
};

void Connection::closeConnection()
{
	if (this->keep_alive)
		return;
	close(this->client_fd);
	this->client_fd = -1;
};

int Connection::getTimeout()
{
	return timeout;
};

int Connection::getBodySize()
{
	return server_config.body_size;
};
