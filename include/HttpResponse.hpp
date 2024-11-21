#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include "HttpRequest.hpp"

class HttpResponse
{
private:
	const HttpRequest request;
	std::string http_version;
	std::string status_code;
	std::string status_message;
	std::string content_type;
	std::string content_length;
	std::string date;
	std::string keep_alive;
	std::string body;

public:
	HttpResponse(const HttpRequest &request);
	~HttpResponse();

	void generateResponse();
};
#endif
