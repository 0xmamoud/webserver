#include "../../include/HttpRequest.hpp"

HttpRequest::HttpRequest(const std::string &request) : request(request)
{
	this->parse();
}

HttpRequest::~HttpRequest() {}

void HttpRequest::parse()
{
	std::string header = this->getHeader();
	this->parseStartLine(header);
	this->parseHost(header);
	this->parseConnection(header);
	this->parseContentType(header);
	this->parseBody();
	this->content_length = this->request.length();
}

std::string HttpRequest::getHeader() const
{
	std::string header = this->request.substr(0, this->request.find("\r\n\r\n"));
	if (header.empty())
		throw std::runtime_error("unknown protocol please provide a valid HTTP request");
	return header;
}

void HttpRequest::parseStartLine(const std::string &header)
{
	std::string startLine = header.substr(0, header.find("\r\n"));
	if (startLine.empty())
		throw std::runtime_error("unknown protocol please provide a valid HTTP request");

	std::vector<std::string> tokens = split(startLine, " ");
	if (tokens.size() != 3)
		throw std::runtime_error("unknown protocol please provide a valid HTTP request");

	this->method = tokens[0];
	this->uri = tokens[1].substr(0, tokens[1].find("?"));
	size_t start = tokens[1].find("?");
	if (start != std::string::npos)
	{
		this->queryString = tokens[1].substr(start + 1);
	}
	else
	{
		this->queryString = "";
	}
	this->http_version = trim(tokens[2], " \t\n\r\f\v");
	if (this->http_version.empty() || this->http_version != "HTTP/1.1")
		throw std::runtime_error("Only HTTP/1.1 is supported");
}

void HttpRequest::parseHost(const std::string &header)
{
	size_t start = header.find("Host: ");
	if (start == std::string::npos)
		throw std::runtime_error("Host header is missing");

	start += 6;
	size_t end = header.find("\r\n", start);
	if (end == std::string::npos)
		throw std::runtime_error("Host header is missing");

	this->host = header.substr(start, end - start);
}

void HttpRequest::parseConnection(const std::string &header)
{
	size_t start = header.find("Connection: ");
	if (start == std::string::npos)
	{
		this->Connection = "close";
		return;
	}

	start += 12;
	size_t end = header.find("\r\n", start);
	if (end == std::string::npos)
	{
		this->Connection = "close";
		return;
	}

	this->Connection = header.substr(start, end - start);
}

void HttpRequest::parseContentType(const std::string &header)
{
	size_t start = header.find("Content-Type: ");
	if (start == std::string::npos)
	{
		this->content_type = "";
		return;
	}

	start += 8;
	size_t end = header.find("\r\n", start);
	if (end == std::string::npos)
	{
		this->content_type = "";
		return;
	}

	this->content_type = header.substr(start, end - start);
}

void HttpRequest::parseBody()
{
	size_t start = this->request.find("\r\n\r\n");
	if (start == std::string::npos)
	{
		this->body = "";
		return;
	}

	start += 4;
	this->body = trim(this->request.substr(start), " \t\n\r\f\v");
}

// --------------------
// Getters Functions
// --------------------
std::string HttpRequest::getMethod() const
{
	return this->method;
}

std::string HttpRequest::getUri() const
{
	return this->uri;
}

std::string HttpRequest::getHttpVersion() const
{
	return this->http_version;
}

std::string HttpRequest::getHost() const
{
	return this->host;
}

std::string HttpRequest::getContentType() const
{
	return this->content_type;
}

std::string HttpRequest::getBody() const
{
	return this->body;
}

std::string HttpRequest::getQueryString() const
{
	return this->queryString;
}

std::string HttpRequest::getConnection() const
{
	return this->Connection;
}

void HttpRequest::setUri(const std::string &uri)
{
	this->uri = uri;
}

int HttpRequest::getContentLength() const
{
	return this->content_length;
}