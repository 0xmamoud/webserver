#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Config.hpp"
#include <string>

class HttpRequest
{
private:
	std::string request;
	std::string method;
	std::string uri;
	std::string http_version;
	std::string host;
	std::string mime_type;
	std::string body;
	std::string connection;
	std::string content_length;
	ServerConfig server_config;

public:
	HttpRequest(std::string request, ServerConfig server_config);
	~HttpRequest();

	void parse();

	std::string getMethod() const;
	std::string getUri() const;
	std::string getHttpVersion() const;
	std::string getHost() const;
	std::string getMimeType() const;
	std::string getBody() const;
	std::string getConnection() const;
	std::string getContentLength() const;
};

#endif
