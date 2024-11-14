#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>

class HttpResponse
{
private:
	int status_code;
	std::string body;

public:
	HttpResponse();
	HttpResponse(const HttpResponse &other);
	HttpResponse &operator=(const HttpResponse &other);
	~HttpResponse();

	std::string stringify();
};
#endif
