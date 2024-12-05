#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>

void handleGetRequest()
{
	const char *queryString = getenv("QUERY_STRING");
	if (queryString == nullptr)
	{
		std::cout << "Content-Type: text/plain\r\n\r\n";
		std::cout << "No query string provided.\n";
		return;
	}

	std::cout << "Content-Type: text/plain\r\n\r\n";
	std::cout << "GET Request Received:\n";
	std::cout << "Query String: " << queryString << "\n";
}

void handlePostRequest()
{
	const char *contentLengthStr = getenv("CONTENT_LENGTH");
	if (contentLengthStr == nullptr)
	{
		std::cout << "Content-Type: text/plain\r\n\r\n";
		std::cout << "No content length provided.\n";
		return;
	}

	int contentLength = std::atoi(contentLengthStr);
	if (contentLength <= 0)
	{
		std::cout << "Content-Type: text/plain\r\n\r\n";
		std::cout << "Invalid content length.\n";
		return;
	}

	std::string body(contentLength, '\0');
	std::cin.read(&body[0], contentLength);

	std::cout << "Content-Type: text/plain\r\n\r\n";
	std::cout << "POST Request Received:\n";
	std::cout << "Body: " << body << "\n";
}

int main()
{
	const char *requestMethod = getenv("REQUEST_METHOD");
	std::cout << "HTTP/1.1 200 OK\r\n";
	if (requestMethod == nullptr)
	{
		std::cout << "Content-Type: text/plain\r\n\r\n";
		std::cout << "No request method provided.\n";
		return 1;
	}

	if (std::strcmp(requestMethod, "GET") == 0)
	{
		handleGetRequest();
	}
	else if (std::strcmp(requestMethod, "POST") == 0)
	{
		handlePostRequest();
	}
	else
	{
		std::cout << "Content-Type: text/plain\r\n\r\n";
		std::cout << "Unsupported request method: " << requestMethod << "\n";
	}

	return 0;
}
