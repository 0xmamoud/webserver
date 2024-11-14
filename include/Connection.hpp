#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>

class Connection
{
private:
	int client_fd;

public:
	Connection(int fd);
	Connection(const Connection &other);
	Connection &operator=(const Connection &other);
	~Connection();

	void handleRequest();
};
#endif