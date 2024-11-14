#include "../../include/Server.hpp"

Server::Server(const Config &config) : config(config)
{
	for (std::vector<ServerConfig>::const_iterator it = config.servers.begin(); it != config.servers.end(); ++it)
	{
		int socket_fd = createServerSocket(it->port);
		if (socket_fd != -1)
			server_sockets.push_back(socket_fd);
	}
}

Server::Server(const Server &other) { *this = other; }

Server &Server::operator=(const Server &other)
{
	(void)other;
	return *this;
}

Server::~Server()
{
	for (std::vector<int>::iterator it = server_sockets.begin(); it != server_sockets.end(); ++it)
		close(*it);
};

int Server::createServerSocket(int port)
{
	int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socket_fd == -1)
	{
		perror("socket");
		return -1;
	}

	int optval = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		perror("setsockopt");
		return -1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind");
		return -1;
	}

	if (listen(socket_fd, SOMAXCONN) < 0)
	{
		perror("listen");
		return -1;
	}
	std::cout << "Server listening on port " << port << std::endl;

	return socket_fd;
}

void Server::run()
{
	// TODO implement epoll management
	std::cout << "Ca marche!" << std::endl;
}