#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <ostream>
#include <sstream>
#include <ctime>
#include <algorithm>

std::vector<std::string> split(const std::string &str, const std::string &delim);
std::string trim(const std::string &str, const std::string &whitespace);
std::string getDate();
size_t convertToOctets(const std::string &str);
std::string toString(int number);

#endif