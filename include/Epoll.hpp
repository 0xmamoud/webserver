#ifndef EPOLL_HPP
#define EPOLL_HPP

#include <sys/epoll.h>
#include <vector>

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

	void add(int fd, uint32_t events);
	int wait(std::vector<struct epoll_event> &events);
};
#endif