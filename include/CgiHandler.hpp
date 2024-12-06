#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "SignalHandler.hpp"

class CgiHandler
{
private:
	std::string method;
	std::string content_type;
	std::string content_length;
	std::string query_string;
	std::string script_name;
	std::string body;

	int pipe_in[2];
	int pipe_out[2];
	pid_t pid;

	void setupChildDup();
	void setupEnv();

public:
	CgiHandler();
	~CgiHandler();

	std::string execute();

	void setMethod(const std::string &method);
	void setBody(const std::string &body);
	void setContentType(const std::string &content_type);
	void setContentLength(const std::string &content_length);
	void setQueryString(const std::string &query_string);
	void setScriptName(const std::string &script_name);
};

#endif