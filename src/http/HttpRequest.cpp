#include "../../include/HttpRequest.hpp"

HttpRequest::HttpRequest(const std::string &request, const ServerConfig &server_config) : request(request), server_config(server_config)
{
	this->parse();
}

HttpRequest::~HttpRequest() {}
