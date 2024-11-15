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
	Epoll epoll;

	for (std::vector<int>::iterator it = server_sockets.begin(); it != server_sockets.end(); ++it)
	{
		if (epoll.add(*it, EPOLLIN) < 0)
			throw std::runtime_error("Failed to add server socket to epoll");
	}

	struct epoll_event events[10];
	while (true)
	{
		int nfds = epoll.wait(events, 10, -1);
		for (int i = 0; i < nfds; i++)
		{
			if (events[i].events & EPOLLIN)
			{
				int fd = events[i].data.fd;
				if (isServerSocket(fd))
					handleNewConnection(fd, epoll);
				else
				{
					// Connection connection(fd);
					// connection.handleRequest();
					std::cout << "Handling request on fd " << fd << std::endl;
					close(fd);
				}
			}
		}
	}
}

bool Server::isServerSocket(int fd)
{
	for (std::vector<int>::iterator it = server_sockets.begin(); it != server_sockets.end(); ++it)
	{
		if (*it == fd)
			return true;
	}
	return false;
}

void Server::handleNewConnection(int server_fd, Epoll &epoll)
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd < 0)
	{
		perror("accept");
		return;
	}

	if (makeNonBlocking(client_fd) < 0)
	{
		close(client_fd);
		return;
	}
	epoll.add(client_fd, EPOLLIN);
}

int Server::makeNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
	{
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK | O_CLOEXEC;
	if (fcntl(fd, F_SETFL, flags) < 0)
	{
		perror("fcntl");
		return -1;
	}
	return 1;
}