#include "../../include/HttpResponse.hpp"

HttpResponse::HttpResponse(const HttpRequest &request, const ServerConfig &server_config) : request(request), server_config(server_config), is_cgi_timeouted(false)
{

	this->generateResponse();
};

HttpResponse::~HttpResponse() {};

void HttpResponse::generateResponse()
{
	try
	{
		std::string method = this->request.getMethod();

		if (!this->isMethodAllowed(method))
		{
			this->body = this->getErrorPage(405);
			this->generateHeader("405", "Method Not Allowed", "text/html");
			return;
		}

		if (this->request.getContentLength() > this->server_config.body_size)
		{
			this->body = this->getErrorPage(413);
			this->generateHeader("413", "Payload Too Large", "text/html");
			return;
		}

		if (this->server_config.server_name != this->request.getHost())
		{
			this->body = this->getErrorPage(400);
			this->generateHeader("400", "Bad Request", "text/html");
			return;
		}

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
bool HttpResponse::isCGI()
{
	std::string uri = this->request.getUri();
	std::map<std::string, LocationConfig>::const_iterator it_location = this->server_config.locations.find(uri);
	if (it_location == this->server_config.locations.end())
		return false;

	return !it_location->second.cgi_bin.empty();
}

void HttpResponse::handleCGI()
{
	try
	{
		std::ostringstream oss;
		if (!this->getFullPath("CGI"))
			return;

		CgiHandler cgi_handler;
		cgi_handler.setMethod(this->request.getMethod());
		cgi_handler.setBody(this->request.getBody());
		cgi_handler.setContentType(this->request.getContentType());
		oss << this->request.getContentLength();
		cgi_handler.setContentLength(oss.str());
		cgi_handler.setQueryString(this->request.getQueryString());
		cgi_handler.setScriptName(this->path);

		this->body = cgi_handler.execute();
	}
	catch (const std::exception &e)
	{
		this->is_cgi_timeouted = true;
		throw std::runtime_error(e.what());
	}
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
	if (!this->getFullPath("POST"))
		return;

	std::string content_type = this->request.getContentType();
	std::string file_extension = content_type.substr(content_type.find_last_of("/") + 1);
	std::string file_name = FileSystem::getRandomeFileName() + "." + file_extension;
	this->path += (this->path[this->path.length() - 1] == '/' ? "" : "/") + file_name;
	FileSystem::createFile(this->path, this->request.getBody());

	this->generateHeader("200", "OK", "application/json");
}

void HttpResponse::handleDELETE()
{
	if (!this->getFullPath("DELETE"))
		return;

	FileSystem::deleteFile(this->path);

	this->generateHeader("200", "OK", "application/json");
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

	this->path = it_location->second.root;
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
		{
			this->body = this->getErrorPage(403);
			this->generateHeader("403", "Forbidden", "text/html");
			return false;
		}
	}

	if (!this->pathAutorization(this->path))
		return false;

	return true;
}

bool HttpResponse::getFullPath(const std::string &method)
{
	std::string uri = this->request.getUri();
	std::map<std::string, LocationConfig>::const_iterator it_location = this->server_config.locations.find(uri);
	if (it_location == this->server_config.locations.end())
	{
		this->body = this->getErrorPage(404);
		this->generateHeader("404", "Not Found", "text/html");
		return false;
	}

	if (method == "POST")
		this->path = it_location->second.upload_path;
	else if (method == "DELETE")
	{
		this->path = it_location->second.root;
		if (!FileSystem::getFileExtension(this->path + uri).empty())
			this->path += uri;

		if (FileSystem::isDirectory(this->path))
		{
			this->body = this->getErrorPage(403);
			this->generateHeader("403", "Forbidden", "text/html");
			return false;
		}
	}
	else if (method == "CGI")
	{

		this->path = it_location->second.cgi_path;
		if (this->path.length() > 0 && this->path[this->path.length() - 1] != '/')
			this->path += '/';
		this->path += it_location->second.cgi_bin;
	}
	else
		this->path = it_location->second.root;

	if (this->path.empty())
	{
		this->body = this->getErrorPage(404);
		this->generateHeader("404", "Not Found", "text/html");
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

bool HttpResponse::isMethodAllowed(const std::string &method)
{
	std::map<std::string, LocationConfig>::const_iterator it_location = this->server_config.locations.find(this->request.getUri());
	if (it_location == this->server_config.locations.end())
		return true;

	std::vector<std::string> methods = it_location->second.methods;
	return std::find(methods.begin(), methods.end(), method) != methods.end();
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
	if (this->isCGI() && !this->is_cgi_timeouted)
	{
		send(client_fd, this->body.c_str(), this->body.length(), 0);
		if (this->keep_alive != "keep-alive")
			close(client_fd);
		return;
	}
	std::string response = this->http_version + " " + this->status_code + " " + this->status_message + "\r\n";
	response += "Date: " + this->date + "\r\n";
	response += "Server: webserv/1.0\r\n";
	response += "Content-Type: " + this->content_type + "\r\n";
	response += "Content-Length: " + this->content_length + "\r\n";
	response += "Connection: " + this->keep_alive + "\r\n\r\n";
	response += this->body;

	send(client_fd, response.c_str(), response.length(), 0);
}
