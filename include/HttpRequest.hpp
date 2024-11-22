#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Config.hpp"
#include <string>

class HttpRequest
{
private:
	const std::string request;
	const ServerConfig server_config;
	std::string method;
	std::string uri;
	std::string queryString;
	std::string http_version;
	std::string host;
	std::string mime_type;
	std::string body;
	std::string connection;
	std::string content_length;

	void parse();

public:
	HttpRequest(const std::string &request, const ServerConfig &server_config);
	~HttpRequest();

	std::string getMethod() const;
	std::string getUri() const;
	std::string getHttpVersion() const;
	std::string getHost() const;
	std::string getMimeType() const;
	std::string getBody() const;
	std::string getConnection() const;
	std::string getContentLength() const;
	std::string getQueryString() const;
};

#endif
