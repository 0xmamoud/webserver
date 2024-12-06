#include "../../include/Connection.hpp"

Connection::Connection(int fd, ServerConfig &server_config) : client_fd(fd), server_config(server_config), keep_alive(false), data_chunked(false), last_activity(time(0)), timeout(5) {};

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

	this->parseBuffer(buf, bytes_read);
	this->manageClientActivity();

	if (!this->isRequestComplete())
		return;

	try
	{
		HttpRequest request(this->buffer);
		ServerConfig server_config = this->getServerConfig(request);
		HttpResponse response(request, this->server_config);
		response.sendResponse(client_fd);
		this->buffer.clear();
	}
	catch (const std::exception &e)
	{
		Logger::log(Logger::ERROR, e.what());
		close(this->client_fd);
		this->client_fd = -1;
		return;
	}
};

void Connection::parseBuffer(char *buf, int bytes_read)
{
	std::string buffer(buf, bytes_read);
	std::string header = buffer.substr(0, buffer.find("\r\n\r\n"));
	if (header.empty())
		return;
	if (header.find("Transfer-Encoding: chunked") != std::string::npos)
		this->data_chunked = true;

	if (!this->data_chunked)
	{
		this->buffer += buffer;
		return;
	}

	std::string body = buffer.substr(buffer.find("\r\n\r\n") + 4);
	if (this->buffer.empty() && body.empty())
	{

		this->data_chunked = false;
		this->buffer += buffer;
		return;
	}
	else if (this->buffer.empty() && !body.empty())
		this->buffer += header;

	if (body.find("0\r\n\r\n") == std::string::npos)
		this->data_chunked = false;
	this->setBufferChunks(body);
};

void Connection::setBufferChunks(const std::string &body)
{
	std::vector<std::string> chunks = split(body, "\r\n");
	for (size_t i = 0; i < chunks.size(); i++)
	{
		if (chunks[i].empty())
			continue;
		if (chunks[i].find("0") != std::string::npos)
		{
			this->data_chunked = false;
			break;
		}
		if (i % 2 == 0)
			continue;
		this->buffer += chunks[i];
	}
};

bool Connection::isRequestComplete()
{
	return !this->data_chunked;
};

bool Connection::isTimedOut()
{
	return time(0) - last_activity > timeout;
};

bool Connection::isClosed()
{
	return client_fd < 0;
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

ServerConfig Connection::getServerConfig(HttpRequest &request)
{

	std::string uri = request.getUri();
	if (uri.empty())
		return this->server_config;

	std::map<std::string, LocationConfig>::iterator it = this->server_config.locations.find(uri);
	if (it == this->server_config.locations.end())
		return this->server_config;
	if (it->second.redirect.empty())
		return this->server_config;

	size_t slash_pos = it->second.redirect.find('/');
	std::string new_uri = (slash_pos != std::string::npos) ? it->second.redirect.substr(slash_pos) : "/";
	request.setUri(new_uri);
	return it->second.servers[it->second.redirect];
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
