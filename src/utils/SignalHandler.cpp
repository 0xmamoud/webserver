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