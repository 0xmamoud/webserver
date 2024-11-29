#include "../../include/HttpResponse.hpp"

HttpResponse::HttpResponse(const HttpRequest &request, const ServerConfig &server_config) : request(request), server_config(server_config)
{

	this->generateResponse();
};

HttpResponse::~HttpResponse() {};

void HttpResponse::generateResponse()
{
	try
	{
		std::string method = this->request.getMethod();

		if (this->isCGI())
			return this->handleCGI();
		if (method == "GET")
			return this->handleGET();
		if (method == "POST")
			return this->handlePOST();
		if (method == "DELETE")
			return this->handleDELETE();

		this->body = this->getErrorPage(405);
		this->generateHeader("405", "Method Not Allowed", "text/html");
	}
	catch (const std::exception &e)
	{
		Logger::log(Logger::ERROR, e.what());
		this->body = this->getErrorPage(500);
		if (this->body.empty())
			this->body = "<html><head><title>500 Internal Server Error</title></head><body><h1>500 Internal Server Error</h1></body></html>";
		this->generateHeader("500", "Internal Server Error", "text/html");
	}
}

void HttpResponse::handleCGI()
{
	Logger::log(Logger::DEBUG, "CGI handling");
}

void HttpResponse::handleGET()
{
	if (!this->parsePath())
		return;

	if (FileSystem::isDirectory(this->path))
	{
		std::vector<std::string> files = FileSystem::readDirectory(this->path);
		if (files.size() == 0)
		{
			this->body = this->getErrorPage(404);
			this->generateHeader("404", "Not Found", "text/html");
			return;
		}
		this->directoryResponseTemplate(files);
		this->generateHeader("200", "OK", FileSystem::getContentType(this->path));
		this->content_type = "text/html";
		return;
	}

	this->body = FileSystem::getFileContent(this->path);
	this->generateHeader("200", "OK", FileSystem::getContentType(this->path));
}

void HttpResponse::handlePOST()
{
	Logger::log(Logger::DEBUG, "POST handling");
}

void HttpResponse::handleDELETE()
{
	Logger::log(Logger::DEBUG, "DELETE handling");
}

bool HttpResponse::parsePath()
{
	std::string uri = this->request.getUri();
	std::map<std::string, LocationConfig>::const_iterator it_location = this->server_config.locations.find(uri);
	if (it_location == this->server_config.locations.end())
	{
		this->body = this->getErrorPage(404);
		this->generateHeader("404", "Not Found", "text/html");
		return false;
	}

	this->path = it_location->second.root + uri;
	if (FileSystem::isDirectory(this->path))
	{
		if (this->path[this->path.length() - 1] != '/')
			this->path += '/';

		bool index_file = !it_location->second.index.empty();
		if (index_file)
			this->path += it_location->second.index;
		else if (!index_file && it_location->second.autoindex)
		{
			std::vector<std::string> files = FileSystem::readDirectory(this->path);
			if (files.size() == 1)
				this->path += files[0];
		}
		else
			return false;
	}

	if (!this->pathAutorization(this->path))
		return false;

	return true;
}

bool HttpResponse::pathAutorization(const std::string &path)
{
	if (FileSystem::isDirectory(path))
	{
		if (FileSystem::isDirectoryExists(path))
			return true;
		this->body = this->getErrorPage(404);
		this->generateHeader("404", "Not Found", "text/html");
	}
	else
	{
		if (!FileSystem::isFileExists(this->path))
		{
			this->body = this->getErrorPage(404);
			this->generateHeader("404", "Not Found", "text/html");
			return false;
		}

		if (!FileSystem::isFileReadable(this->path))
		{
			this->body = this->getErrorPage(403);
			this->generateHeader("403", "Forbidden", "text/html");
			return false;
		}
		return true;
	}

	return false;
}

bool HttpResponse::isCGI()
{
	return false;
}

void HttpResponse::generateHeader(const std::string &status_code, const std::string &status_message, const std::string &content_type)
{
	std::ostringstream oss;

	this->http_version = "HTTP/1.1";
	this->status_code = status_code;
	this->status_message = status_message;
	this->date = getDate();
	this->keep_alive = status_code == "200" ? this->request.getConnection() : "close";
	oss << this->body.length();
	this->content_type = content_type;
	this->content_length = oss.str();
}

void HttpResponse::directoryResponseTemplate(const std::vector<std::string> &files)
{
	this->body = "<html><head><title>Directory listing</title></head><body><h1>Directory listing</h1><ul>";
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		this->body += "<li><a href=\"" + *it + "\">" + *it + "</a></li>";
	}
	this->body += "</ul></body></html>";
	this->content_type = "text/html";
}

std::string HttpResponse::getErrorPage(int error_code)
{
	std::map<int, std::string>::const_iterator it = this->server_config.error_pages.find(error_code);
	if (it == this->server_config.error_pages.end())
		return "";

	std::string file_content = FileSystem::getFileContent(it->second);
	if (file_content.empty())
		return "";
	return file_content;
}

void HttpResponse::sendResponse(int client_fd)
{
	std::string response = this->http_version + " " + this->status_code + " " + this->status_message + "\r\n";
	response += "Date: " + this->date + "\r\n";
	response += "Server: webserv/1.0\r\n";
	response += "Content-Type: " + this->content_type + "\r\n";
	response += "Content-Length: " + this->content_length + "\r\n";
	response += "Connection: " + this->keep_alive + "\r\n\r\n";
	response += this->body;

	// Logger::log(Logger::DEBUG, "Response: " + response);

	send(client_fd, response.c_str(), response.length(), 0);
}
