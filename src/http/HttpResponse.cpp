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
	this->path = this->request.getUri();
	if (this->isDirectory(this->path))
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
			std::vector<std::string> files = this->readDirectory(this->path);
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
	if (this->isDirectory(path))
	{
		if (this->isDirectoryExists(path))
			return true;
		this->generate404();
	}
	else
	{
		if (!this->isFileExists(this->path))
		{
			this->generate404();
			return false;
		}

		if (!this->isFileReadable(this->path))
		{
			this->generate403();
			return false;
		}
		return true;
	}

	return false;
}
