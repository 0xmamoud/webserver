#include "../../include/Connection.hpp"

Connection::Connection(int fd, ServerConfig &server_config) : client_fd(fd), server_config(server_config), keep_alive(false), data_chunked(false), last_activity(time(0)), timeout(5) {};

Connection::~Connection()
{
	if (client_fd > 0)
		close(client_fd);

	this->buffer.clear();
};

void Connection::handleRequest()
{

	std::string buf = this->readRequestData();

	this->parseBuffer(buf);
	this->manageClientActivity();

	if (!this->isRequestComplete())
		return;

	try
	{
		HttpRequest request(this->buffer);
		ServerConfig new_server_config = this->getServerConfig(request);
		HttpResponse response(request, new_server_config);
		response.sendResponse(client_fd);
		this->buffer.clear();
		this->closeConnection();
	}
	catch (const std::exception &e)
	{
		Logger::log(Logger::ERROR, e.what());
		close(this->client_fd);
		this->client_fd = -1;
		return;
	}
};

std::string Connection::readRequestData()
{
	const int CHUNK_SIZE = 1;
	std::vector<char> buffer;
	char chunk[CHUNK_SIZE];
	int bytes_read = 0;

	while ((bytes_read = recv(client_fd, chunk, CHUNK_SIZE, 0)) > 0)
	{
		buffer.insert(buffer.end(), chunk, chunk + bytes_read);
		if (bytes_read < CHUNK_SIZE)
			break;
	}
	return std::string(buffer.begin(), buffer.end());
}

void Connection::parseBuffer(std::string &buf)
{
	std::string header = buf.substr(0, buf.find("\r\n\r\n"));
	if (header.empty())
		return;
	if (header.find("Transfer-Encoding: chunked") != std::string::npos)
		this->data_chunked = true;

	if (!this->data_chunked)
	{
		this->buffer.append(buf);
		buf.clear();
		return;
	}

	std::string body = buf.substr(buf.find("\r\n\r\n") + 4);
	if (this->buffer.empty() && body.empty())
	{

		this->data_chunked = false;
		this->buffer += buf;
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
	std::string header = this->buffer.substr(0, this->buffer.find("\r\n\r\n"));
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
	request.setHost(it->second.servers[it->second.redirect].server_name);
	ServerConfig new_server_config = it->second.servers[it->second.redirect];

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
