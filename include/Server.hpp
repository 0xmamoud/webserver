#ifndef SERVER_HPP
#define SERVER_HPP

#include "Epoll.hpp"
#include "Connection.hpp"
#include "ConfigParser.hpp"
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
class Server
{
private:
	Config config;
	std::vector<int> server_sockets;
	int createServerSocket(int port);

public:
	Server(const Config &config);
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();

	void run();
};

#endif