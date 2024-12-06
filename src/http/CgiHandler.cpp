#include "../../include/CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

std::string CgiHandler::execute()
{
	this->setupSignalTimeout();

	if (pipe(this->pipe_in) == -1 || pipe(this->pipe_out) == -1)
		throw std::runtime_error("Failed to create pipe");

	this->pid = fork();
	if (this->pid == -1)
		throw std::runtime_error("Failed to fork process");

	if (this->pid == 0)
	{
		alarm(10);
		this->setupChildDup();
		this->setupEnv();

		if (execl(this->script_name.c_str(), this->script_name.c_str(), NULL) == -1)
			throw std::runtime_error("Failed to execute CGI script");
	}

	close(this->pipe_in[0]);
	close(this->pipe_out[1]);

	if (this->method == "POST" && this->body.length() > 0)
		write(this->pipe_in[1], this->body.c_str(), this->body.length());
	close(this->pipe_in[1]);

	std::string response;
	char buffer[4096];
	int read_bytes;

	while ((read_bytes = read(this->pipe_out[0], buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[read_bytes] = '\0';
		response += buffer;
	}
	close(this->pipe_out[0]);

	int status;
	waitpid(this->pid, &status, 0);

	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
	{
		throw std::runtime_error("CGI script timed out after 10 seconds");
	}

	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		throw std::runtime_error("CGI script exited with non-zero status");

	return response;
}

void CgiHandler::setupSignalTimeout()
{
	struct sigaction sa;
	sa.sa_handler = SignalHandler::timeoutHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGALRM, &sa, NULL);
}

void CgiHandler::setupChildDup()
{
	close(this->pipe_in[1]);
	close(this->pipe_out[0]);

	if (this->method == "POST")
	{
		if (dup2(this->pipe_in[0], STDIN_FILENO) < 0)
			throw std::runtime_error("Failed to duplicate file descriptor");
	}
	close(this->pipe_in[0]);

	if (dup2(this->pipe_out[1], STDOUT_FILENO) < 0)
		throw std::runtime_error("Failed to duplicate file descriptor");
	close(this->pipe_out[1]);
}

void CgiHandler::setupEnv()
{
	setenv("REQUEST_METHOD", this->method.c_str(), 1);
	setenv("CONTENT_TYPE", this->content_type.c_str(), 1);
	setenv("CONTENT_LENGTH", this->content_length.c_str(), 1);
	setenv("QUERY_STRING", this->query_string.c_str(), 1);
	setenv("SCRIPT_NAME", this->script_name.c_str(), 1);
}

void CgiHandler::setMethod(const std::string &method)
{
	this->method = method;
}

void CgiHandler::setBody(const std::string &body)
{
	this->body = body;
}

void CgiHandler::setContentType(const std::string &content_type)
{
	this->content_type = content_type;
}

void CgiHandler::setContentLength(const std::string &content_length)
{
	this->content_length = content_length;
}

void CgiHandler::setQueryString(const std::string &query_string)
{
	this->query_string = query_string;
}

void CgiHandler::setScriptName(const std::string &script_name)
{
	this->script_name = script_name;
}
