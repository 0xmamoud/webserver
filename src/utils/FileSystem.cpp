#include "../../include/FileSystem.hpp"

const std::map<std::string, std::string> FileSystem::content_types = FileSystem::initializeContentTypes();

std::map<std::string, std::string> FileSystem::initializeContentTypes()
{
	std::map<std::string, std::string> types;
	types[".html"] = "text/html";
	types[".css"] = "text/css";
	types[".js"] = "application/javascript";
	types[".png"] = "image/png";
	types[".jpg"] = "image/jpeg";
	types[".jpeg"] = "image/jpeg";
	types[".gif"] = "image/gif";
	types[".svg"] = "image/svg+xml";
	types[".json"] = "application/json";
	types[".xml"] = "application/xml";
	types[".pdf"] = "application/pdf";
	types[".txt"] = "text/plain";
	return types;
}
FileSystem::FileSystem() {}

FileSystem::~FileSystem() {}

bool FileSystem::isDirectory(const std::string &path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) != 0)
		return false;

	return (sb.st_mode & S_IFDIR) != 0;
}

bool FileSystem::isFile(const std::string &path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) != 0)
		return false;

	return (sb.st_mode & S_IFREG) != 0;
}

bool FileSystem::isFileExists(const std::string &path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) != 0)
		return false;

	return true;
}

bool FileSystem::isDirectoryExists(const std::string &path)
{
	struct stat sb;
	if (stat(path.c_str(), &sb) != 0)
		return false;

	return true;
}

bool FileSystem::isFileReadable(const std::string &path)
{
	return access(path.c_str(), R_OK) == 0;
}

std::vector<std::string> FileSystem::readDirectory(const std::string &path)
{
	std::vector<std::string> files;

	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		return files;

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string fileName(entry->d_name);
		if (fileName == "." || fileName == "..")
			continue;
		files.push_back(fileName);
	}

	return files;
}

std::string FileSystem::getContentType(const std::string &path)
{
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos)
		return "application/octet-stream";

	std::string extension = path.substr(pos);
	std::map<std::string, std::string>::const_iterator it = content_types.find(extension);
	if (it == content_types.end())
		return "application/octet-stream";

	return it->second;
}

std::string FileSystem::getFileContent(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
		throw std::runtime_error("Invalid file");

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	return content;
}