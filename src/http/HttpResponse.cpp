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
		return this->generate405();
	}
	catch (const std::exception &e)
	{
		Logger::log(Logger::ERROR, e.what());
		this->generate500();
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
		return false;

	this->path = it_location->second.root + uri;
	if (FileSystem::isDirectory(this->path))
	{
		std::map<std::string, LocationConfig>::const_iterator it_location = this->server_config.locations.find(this->path);
		if (it_location == this->server_config.locations.end())
			return false;

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
		this->generate404();
	}
	else
	{
		if (!FileSystem::isFileExists(this->path))
		{
			this->generate404();
			return false;
		}

		if (!FileSystem::isFileReadable(this->path))
		{
			this->generate403();
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

bool HttpResponse::isMethodAllowed(const LocationConfig &location)
{
	(void)location;
	return true;
}

void HttpResponse::generate200()
{
	this->status_code = "200";
	this->status_message = "OK";
}

void HttpResponse::generate403()
{
	this->status_code = "403";
	this->status_message = "Forbidden";
}

void HttpResponse::generate404()
{
	this->status_code = "404";
	this->status_message = "Not Found";
}

void HttpResponse::generate405()
{
	this->status_code = "405";
	this->status_message = "Method Not Allowed";
}

void HttpResponse::generate413()
{
	this->status_code = "413";
	this->status_message = "Payload Too Large";
}

void HttpResponse::generate500()
{
	this->status_code = "500";
	this->status_message = "Internal Server Error";
}

void HttpResponse::sendResponse(int client_fd)
{
	(void)client_fd;
}