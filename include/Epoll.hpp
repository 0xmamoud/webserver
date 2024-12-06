#ifndef EPOLL_HPP
#define EPOLL_HPP
#include <stdexcept>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "SignalHandler.hpp"

class Epoll
{
private:
	int epoll_fd;
	std::vector<struct epoll_event> events;

public:
	Epoll();
	Epoll(const Epoll &other);
	Epoll &operator=(const Epoll &other);
	~Epoll();

	int add(int fd, uint32_t events);
	int wait(struct epoll_event *events, int maxevents, int timeout);
};
#endif