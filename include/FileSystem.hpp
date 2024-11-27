#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>

class FileSystem
{
private:
	FileSystem();
	~FileSystem();

	static const std::map<std::string, std::string> content_types;
	static std::map<std::string, std::string> initializeContentTypes();

public:
	static bool isDirectory(const std::string &path);
	static bool isFile(const std::string &path);
	static bool isFileExists(const std::string &path);
	static bool isDirectoryExists(const std::string &path);
	static bool isFileReadable(const std::string &path);

	static std::vector<std::string> readDirectory(const std::string &path);
	static std::string getContentType(const std::string &path);
	static std::string getFileContent(const std::string &path);
};
#endif
