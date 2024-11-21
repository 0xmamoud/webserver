#include "../../include/Server.hpp"

Server::Server(const Config &config)
{
	for (std::vector<ServerConfig>::const_iterator it = config.servers.begin(); it != config.servers.end(); ++it)
	{
		int socket_fd = createServerSocket(it->port);
		if (socket_fd < 0)
			continue;
		server_configs[socket_fd] = *it;
	}
}

Server::Server(const Server &other) { *this = other; }

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		this->server_configs = other.server_configs;
	}
	return *this;
}

Server::~Server()
{
	for (std::map<int, ServerConfig>::iterator it = server_configs.begin(); it != server_configs.end(); ++it)
	{
		close(it->first);
	}
};

int Server::createServerSocket(int port)
{
	std::ostringstream oss;

	oss << port;

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

	Logger::log(Logger::INFO, "Server listening on port " + oss.str());

	return socket_fd;
}

void Server::run()
{
	Epoll epoll;

	for (std::map<int, ServerConfig>::iterator it = server_configs.begin(); it != server_configs.end(); ++it)
	{
		if (epoll.add(it->first, EPOLLIN) < 0)
			throw std::runtime_error("Failed to add server socket to epoll");
	}

	struct epoll_event events[10];
	std::map<int, Connection> connections;
	while (true)
	{
		int nfds = epoll.wait(events, 10, -1);
		for (int i = 0; i < nfds; i++)
		{
			if (events[i].events & EPOLLIN)
			{
				int fd = events[i].data.fd;
				if (isServerSocket(fd))
					handleNewConnection(fd, epoll, connections);
				else
				{
					std::map<int, Connection>::iterator it = connections.find(fd);
					if (it != connections.end())
					{
						it->second.handleRequest();
					}
				}
			}
		}
		closeConnection(connections);
	}
}

bool Server::isServerSocket(int fd)
{
	for (std::map<int, ServerConfig>::iterator it = server_configs.begin(); it != server_configs.end(); ++it)
	{
		if (it->first == fd)
			return true;
	}
	return false;
}

void Server::handleNewConnection(int server_fd, Epoll &epoll, std::map<int, Connection> &connections)
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

	ServerConfig &server_config = server_configs[server_fd];
	connections.insert(std::make_pair(client_fd, Connection(client_fd, server_config)));
	Logger::log(Logger::INFO, "New connection accepted");
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

void Server::closeConnection(std::map<int, Connection> &connections)
{
	for (std::map<int, Connection>::iterator it = connections.begin(); it != connections.end();)
	{
		std::cout << "timeout modif" << it->second.getTimeout() << std::endl;
		std::cout << "body size modif" << it->second.getBodySize() << std::endl;
		if (it->second.isTimedOut())
		{
			connections.erase(it++);
		}
		else
			++it;
	}
}