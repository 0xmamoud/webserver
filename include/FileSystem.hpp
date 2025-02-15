#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>

class FileSystem
{
private:
	FileSystem();
	~FileSystem();

	static const std::map<std::string, std::string> content_types;
	static std::map<std::string, std::string> initializeContentTypes();

	static bool isBinaryFile(const std::string &extension);

public:
	static bool isDirectory(const std::string &path);
	static bool isFile(const std::string &path);
	static bool isFileExists(const std::string &path);
	static bool isDirectoryExists(const std::string &path);
	static bool isFileReadable(const std::string &path);

	static void createFile(const std::string &path, const std::string &content);
	static void deleteFile(const std::string &path);
	// static void deleteDirectory(const std::string &path);

	static std::vector<std::string> readDirectory(const std::string &path);
	static std::string getContentType(const std::string &path);
	static std::string getFileContent(const std::string &path);
	static std::string getRandomeFileName();
	static std::string getFileExtension(const std::string &filename);
};
#endif
