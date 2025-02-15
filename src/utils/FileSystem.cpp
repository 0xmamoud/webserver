#include "../../include/FileSystem.hpp"
#include "../../include/Utility.hpp"
#include "../../include/Logger.hpp"

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
		std::string fullPath = path.length() - 1 == '/' ? path + fileName : path + "/" + fileName;
		if (isDirectory(fullPath))
			continue;
		files.push_back(fileName);
	}

	closedir(dir);

	return files;
}

std::string FileSystem::getContentType(const std::string &path)
{
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos)
		return "text/plain";

	std::string extension = path.substr(pos);
	std::map<std::string, std::string>::const_iterator it = content_types.find(extension);
	if (it == content_types.end())
		return "text/plain";

	return it->second;
}

std::string FileSystem::getFileContent(const std::string &path)
{
	bool binary = isBinaryFile(path.substr(path.find_last_of(".")));
	std::ifstream file(path.c_str(), binary ? std::ios::binary : std::ios::in);
	if (!file.is_open())
		throw std::runtime_error("Failed to open file: " + path);
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	return content;
}

void FileSystem::createFile(const std::string &path, const std::string &content)
{
	bool binary = isBinaryFile(path.substr(path.find_last_of(".")));
	Logger::log(Logger::DEBUG, "Creating file: " + path);
	Logger::log(Logger::DEBUG, "isBinary: " + toString(binary));

	std::ofstream file(path.c_str(), binary ? std::ios::binary : std::ios::out);
	if (!file.is_open())
		throw std::runtime_error("Failed to create file: " + path);

	if (binary)
	{
		// Write raw data (bytes)
		file.write(content.data(), content.size());
	}
	else
	{
		// Write as text
		file << content;
	}

	file.close();
}

void FileSystem::deleteFile(const std::string &path)
{
	if (unlink(path.c_str()) != 0)
		throw std::runtime_error("Failed to delete file: " + path);
}

// void FileSystem::deleteDirectory(const std::string &path)
// {
// 	DIR *dir = opendir(path.c_str());
// 	if (dir == NULL)
// 		throw std::runtime_error("Failed to delete directory: " + path);
// 	std::cout << "Deleting directory: " << path << std::endl;

// 	struct dirent *entry;
// 	while ((entry = readdir(dir)) != NULL)
// 	{
// 		std::string fileName(entry->d_name);
// 		if (fileName == "." || fileName == "..")
// 			continue;
// 		std::string fullPath = (path[path.length() - 1] == '/') ? path + fileName : path + "/" + fileName;
// 		if (isDirectory(fullPath))
// 			deleteDirectory(fullPath);
// 		else
// 		{
// 			try
// 			{
// 				std::cout << "Deleting file: " << fullPath << std::endl;
// 				deleteFile(fullPath);
// 			}
// 			catch (const std::exception &e)
// 			{
// 				closedir(dir);
// 				throw std::runtime_error("Failed to delete directory: " + path);
// 			}
// 		}
// 	}

// 	closedir(dir);
// 	if (rmdir(path.c_str()) != 0)
// 		throw std::runtime_error("Failed to delete directory: " + path);
// 	std::cout << "Directory deleted: " << path << std::endl;
// }

bool FileSystem::isBinaryFile(const std::string &extension)
{

	std::set<std::string> text_types;
	text_types.insert("text/html");
	text_types.insert("text/css");
	text_types.insert("application/javascript");
	text_types.insert("application/json");
	text_types.insert("application/xml");
	text_types.insert("text/plain");

	std::map<std::string, std::string>::const_iterator it = content_types.find(extension);
	if (it == content_types.end())
		return false;

	return text_types.find(it->second) == text_types.end();
}

std::string FileSystem::getRandomeFileName()
{
	std::ostringstream oss;
	oss << time(0);

	std::string fine_name = "upload_" + oss.str();
	return fine_name;
}

std::string FileSystem::getFileExtension(const std::string &filename)
{
	size_t pos = filename.find_last_of(".");
	if (pos == std::string::npos)
		return "";

	std::string file_extension = filename.substr(pos);

	std::map<std::string, std::string>::const_iterator it = content_types.find(file_extension);
	if (it == content_types.end())
		return "";

	return file_extension;
}