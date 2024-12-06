#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <signal.h>
#include <iostream>
#include <string>
#include "Logger.hpp"
#include <unistd.h>

class SignalHandler
{
public:
	static bool is_interrupted;
	static void handleStop(int signal);
	static void timeoutHandler(int signal);
};

#endif