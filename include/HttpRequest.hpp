#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>

class HttpRequest
{
private:
	std::string request_data;

public:
	HttpRequest();
	HttpRequest(const HttpRequest &other);
	HttpRequest &operator=(const HttpRequest &other);
	~HttpRequest();

	void parseRequest();
};

#endif
