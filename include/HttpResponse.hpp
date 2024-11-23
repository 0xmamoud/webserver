#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include "HttpRequest.hpp"
#include "Config.hpp"
#include <sys/types.h>
#include <sys/socket.h>

class HttpResponse
{
private:
	const HttpRequest request;
	const ServerConfig server_config;
	std::string http_version;
	std::string status_code;
	std::string status_message;
	std::string content_type;
	std::string content_length;
	std::string date;
	std::string keep_alive;
	std::string body;

	void generateResponse();

public:
	HttpResponse(const HttpRequest &request, const ServerConfig &server_config);
	~HttpResponse();

	void sendResponse(int client_fd);
};
#endif
