#include "../../include/HttpResponse.hpp"

HttpResponse::HttpResponse(const HttpRequest &request, const ServerConfig &server_config) : request(request), server_config(server_config)
{
	this->generateResponse();
};

HttpResponse::~HttpResponse() {};

void HttpResponse::generateResponse()
{
	this->http_version = "HTTP/1.1";
};
