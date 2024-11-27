#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include "HttpRequest.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "FileSystem.hpp"

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
	std::string path;

	void generateResponse();

	void handleGET();
	void handlePOST();
	void handleDELETE();
	void handleCGI();

	void generate200();
	void generate403();
	void generate404();
	void generate405();
	void generate413();
	void generate500();

	bool isCGI();
	bool isMethodAllowed(const LocationConfig &location);
	bool parsePath();
	bool pathAutorization(const std::string &path);

public:
	HttpResponse(const HttpRequest &request, const ServerConfig &server_config);
	~HttpResponse();

	void sendResponse(int client_fd);
};
#endif
