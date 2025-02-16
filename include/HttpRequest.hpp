#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Config.hpp"
#include "Utility.hpp"
#include <string>

class HttpRequest
{
private:
	const std::string request;
	std::string method;
	std::string uri;
	std::string queryString;
	std::string http_version;
	std::string host;
	std::string Connection;
	std::string content_type;
	std::string body;
	int content_length;

	void parse();
	std::string getHeader() const;
	void parseStartLine(const std::string &header);
	void parseHost(const std::string &header);
	void parseConnection(const std::string &header);
	void parseContentType(const std::string &header);
	void parseBody();

public:
	HttpRequest(const std::string &request);
	~HttpRequest();

	std::string getMethod() const;
	std::string getUri() const;
	std::string getHttpVersion() const;
	std::string getHost() const;
	std::string getConnection() const;
	std::string getContentType() const;
	std::string getBody() const;
	std::string getQueryString() const;
	int getContentLength() const;

	void setUri(const std::string &uri);
	void setHost(const std::string &host);
};

#endif
