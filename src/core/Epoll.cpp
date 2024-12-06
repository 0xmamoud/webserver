#include "../../include/Epoll.hpp"

Epoll::Epoll()
{
	this->epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		perror("epoll_create1");
		throw std::runtime_error("Failed to create epoll instance");
	}
}

Epoll::Epoll(const Epoll &other)
{
	*this = other;
}

Epoll &Epoll::operator=(const Epoll &other)
{
	this->epoll_fd = other.epoll_fd;
	return *this;
}

Epoll::~Epoll()
{
	close(this->epoll_fd);
}

int Epoll::add(int fd, uint32_t events)
{
	struct epoll_event event;
	event.events = events;
	event.data.fd = fd;

	if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
	{
		perror("epoll_ctl");
		return -1;
	}
	return 1;
}

int Epoll::wait(struct epoll_event *events, int maxevents, int timeout)
{
	int nfds = epoll_wait(this->epoll_fd, events, maxevents, timeout);
	if (nfds < 0)
	{
		if (errno == EINTR)
			return nfds;

		if (SignalHandler::is_interrupted)
			return nfds;

		perror("epoll_wait");
		throw std::runtime_error("Failed to wait for events");
	}
	return nfds;
}
