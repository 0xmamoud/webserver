#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include "HttpRequest.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "CgiHandler.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "FileSystem.hpp"
#include <algorithm>

class HttpResponse
{
private:
	const HttpRequest request;
	const ServerConfig server_config;
	std::string http_version;
	std::string status_code;
	std::string status_message;
	std::string content_type;
	std::string date;
	std::string keep_alive;
	std::string content_length;
	std::string body;
	std::string path;
	std::string getErrorPage(int error_code);
	bool is_cgi_timeouted;

	void handleGET();
	void handlePOST();
	void handleDELETE();
	void handleCGI();
	void generateResponse();
	void directoryResponseTemplate(const std::vector<std::string> &files);
	void generateHeader(const std::string &status_code, const std::string &status_message, const std::string &content_type);

	bool isCGI();
	bool isMethodAllowed(const std::string &method);
	bool parsePath();
	bool pathAutorization(const std::string &path);
	bool getFullPath(const std::string &method);
	bool isSameServerName();

public:
	HttpResponse(const HttpRequest &request, const ServerConfig &server_config);
	~HttpResponse();

	void sendResponse(int client_fd);
};
#endif
