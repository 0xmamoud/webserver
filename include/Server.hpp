#ifndef SERVER_HPP
#define SERVER_HPP

#include "Epoll.hpp"
#include "Connection.hpp"
#include "ConfigParser.hpp"
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "Logger.hpp"
#include <sstream>
#include <utility>

class Server
{
private:
	std::map<int, ServerConfig> server_configs;
	std::map<int, Connection *> connections;

	int createServerSocket(int port);
	bool isServerSocket(int fd);
	void handleNewConnection(int server_fd, Epoll &epoll);
	int makeNonBlocking(int fd);
	void closeConnection();

public:
	Server(const Config &config);
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();

	void run();
};

#endif