#include "../../include/SignalHandler.hpp"

bool SignalHandler::is_interrupted = false;

void SignalHandler::handleStop(int signal)
{
	if (signal == SIGINT)
	{
		Logger::log(Logger::INFO, "Shuting dawn server...");
		is_interrupted = true;
	}
}

void SignalHandler::timeoutHandler(int signal)
{
	if (signal == SIGALRM)
		throw std::runtime_error("Timeout signal received cutting process...");
}