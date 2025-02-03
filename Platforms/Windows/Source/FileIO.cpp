#include "pch.h"
#pragma hdrstop
#include "FileIO.h"
#include <commdlg.h>
#include <shlobj_core.h>
#include "Log.h"
using namespace Perry;

static std::unordered_map<DirectoryType, std::string> filePaths{};

void FileIO::Init()
{
	char exeFilePath[MAX_PATH];
	[[maybe_unused]] DWORD status = GetModuleFileName(NULL, exeFilePath, MAX_PATH);
	assert(status != 0);

	{
		PWSTR path_tmp;
		HRESULT folderPathResult = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
		if (folderPathResult != S_OK)
			ASSERT_MSG(LOG_FILEIO,
				false,
				"Failed to get known folder path for \"FOLDERID_RoamingAppData\". Return code: %i",
				int(folderPathResult));

		// Check if the known roaming directory does infact exist.
		std::string roamingFolderPath = std::filesystem::path(path_tmp).string();
		if (!std::filesystem::exists(roamingFolderPath))
		{
			ERROR(LOG_FILEIO,
				"Failed to find Roaming directory: %s. Trying to create new directory...",
				roamingFolderPath.c_str());
			std::filesystem::create_directory(roamingFolderPath);
		}

		// Create a custom ball folder in the known roaming directory if it doesn't exist.
		roamingFolderPath += "/P2/";
		if (!std::filesystem::exists(roamingFolderPath))
		{
			INFO(LOG_FILEIO, "Creating \"P2\" folder in known directory: ", roamingFolderPath.c_str());
			std::filesystem::create_directory(roamingFolderPath);
		}

		// Files that should be stored in a known directory (like the roaming directory)
		filePaths.insert({ DirectoryType::Save, std::string(roamingFolderPath + "Save/") });
	}

	const auto applicationPath = std::filesystem::path(exeFilePath).parent_path().string();

	// Files that should be stored in the same directory as the .exe
	filePaths.insert({ DirectoryType::Resource, applicationPath + "/Resources/" });
	filePaths.insert({ DirectoryType::Shaders, applicationPath + "/Resources/Shaders/" });
	filePaths.insert({ DirectoryType::Log, applicationPath + "Log/" });
}
std::string FileIO::WorkingDirectory()
{
	return std::filesystem::current_path().string();
}

std::string FileIO::GetFileName(const std::string& path)
{
	auto end = path.find_last_of('/');
	if (end == std::string::npos)
		end = path.find_last_of('\\');
	auto fpath = path.substr(end);

	return fpath;
}


bool FileIO::Exist(DirectoryType type, const std::string& targetFile)
{
	return std::filesystem::exists(GetPath(type, targetFile));
}

bool FileIO::Write(DirectoryType type, const std::string& relativePath, const std::string& data, bool appendData)
{
	if (!HasWriteAccess(type))
	{
		ERROR(LOG_FILEIO, "DirectoryType '%i' is not savable", type);
		return false;
	}

	const std::string filePath = GetPath(type, relativePath);

	if (!std::filesystem::exists(std::filesystem::path(filePath).parent_path()))
	{
		std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
	}

	// Open file. Assign the right flags, based on whether to append or truncate the data.
	std::fstream file{};

	std::ios_base::openmode mode = std::fstream::out;
	mode |= appendData ? std::fstream::app : std::fstream::trunc;

	file.open(filePath, mode);

	if (!file.is_open())
	{
		ERROR(LOG_FILEIO, "Failed to open '%s' for writing.\n", filePath.c_str());
		return false;
	}

	file << data;
	file.close();
	return true;
}

bool FileIO::WriteBinary(DirectoryType type, const std::string& relativePath, const void* data, size_t size, bool appendData)
{
	ASSERT_MSG(LOG_FILEIO, data, "Called FileIO::write with a invalid data pointer");

	if (!HasWriteAccess(type))
	{
		ERROR(LOG_FILEIO, "DirectoryType '%i' is not savable", type);
		return false;
	}

	std::string filePath = GetPath(type, relativePath);

	if (!std::filesystem::exists(std::filesystem::path(filePath).parent_path()))
	{
		std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
	}

	// Open file. Assign the right flags, based on whether to append or truncate the data.
	std::fstream file{};

	std::ios_base::openmode mode = std::fstream::out | std::fstream::binary;

	file.open(filePath, mode);

	if (!file.is_open())
	{
		ERROR(LOG_FILEIO, "Failed to open '%s' for writing.\n", filePath.c_str());
		return false;
	}

	file.write(static_cast<const char*>(data), size);
	file.close();
	return true;
}

std::string FileIO::Read(DirectoryType type, const std::string& relativePath)
{
	// Confirm that the file exists
	if (!Exist(type, relativePath))
	{
		ERROR(LOG_FILEIO, "Attempted to read from '%s' which doesnt exist !", GetPath(type, relativePath).c_str());
		return "";
	}

	auto p = GetPath(type, relativePath);
	// Open and read file
	std::fstream file(p, std::fstream::in);
	if (!file.good())
	{
		ERROR(LOG_FILEIO, "Failed to open '%s' for reading.\n", GetPath(type, relativePath).c_str());
		return "";
	}

	// Read entire file
	std::stringstream ss;
	ss << file.rdbuf();
	std::string data = ss.str();
	file.close();
	return data;
}

bool FileIO::ReadBinary(DirectoryType type, const std::string& relativePath, void* targetBuffer,
	std::streamsize targetBufferSize)
{
	ASSERT_MSG(LOG_FILEIO, targetBuffer, "Called FileIO::Read with a invalid targetbuffer");
	// Confirm that the file exists
	if (!Exist(type, relativePath))
	{
		ASSERT_MSG(LOG_FILEIO, false, "ReadFromFile failed, '%s' does not exist.", relativePath.c_str());
		return false;
	}

	std::ifstream file(GetPath(type, relativePath), std::ios::binary);
	if (!file.is_open())
	{
		ERROR(LOG_FILEIO, "file '%s' not found!", relativePath.c_str());
		return false;
	}

	std::streamsize size = std::filesystem::file_size(GetPath(type, relativePath));

	if (targetBufferSize > size)
	{
		ERROR(LOG_FILEIO, "Target buffer is to big to read the file into");
		return false;
	}

	file.read(static_cast<char*>(targetBuffer), targetBufferSize);
	file.close();

	return true;
}

bool FileIO::Delete(DirectoryType type, const std::string& relativePath)
{
	if (!Exist(type, relativePath))
		return false;

	return std::filesystem::remove(GetPath(type, relativePath));
}

size_t FileIO::GetSize(DirectoryType type, const std::string& relativePath)
{
	return std::filesystem::file_size(GetPath(type, relativePath));
}

std::string FileIO::GetPath(DirectoryType type)
{
	const auto rootPath = filePaths.find(type);
	assert(rootPath != filePaths.end()); // If this asserts it needs to be implemented in initialize most likely !

	return rootPath->second;
}

std::string FileIO::GetPath(DirectoryType type, const std::string& relativePath)
{
	return std::filesystem::path(GetPath(type)).append(relativePath).string();
}

bool FileIO::HasWriteAccess(DirectoryType type)
{
	return type == DirectoryType::Save || type == DirectoryType::Log || type == DirectoryType::Resource;
}

std::string FileIO::OpenDialog(const char* filter)
{
	OPENFILENAMEA ofn; // common dialog box structure
	CHAR szFile[260] = { 0 };

	// Init OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);

	throw;
	//ofn.hwndOwner = glfwGetWin32Window(GetEngine().GetWindow().GetHandle());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	std::string resPath = GetPath(DirectoryType::Resource);
	ofn.lpstrInitialDir = resPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		std::filesystem::path filepath = std::filesystem::path(ofn.lpstrFile).lexically_relative(resPath);

		return filepath.string();
	}
	return std::string();
}

std::vector<std::string> FileIO::GetFilesInDirectory(DirectoryType directoryType, const std::string& parentDirectory)
{
	std::vector<std::string> paths;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(GetPath(directoryType, parentDirectory)))
	{
		paths.push_back(entry.path().string());
	}

	return paths;
}

std::vector<std::string> FileIO::GetFilesInDirectory(DirectoryType directoryType, const std::string& parentDirectory, const std::string& ext)
{
	std::vector<std::string> paths;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(GetPath(directoryType, parentDirectory)))
	{
		if (entry.path().extension() == ext)
			paths.push_back(entry.path().lexically_relative(GetPath(directoryType)).string());
	}

	return paths;
}

std::vector<std::string> FileIO::GetFilesInDirectory(DirectoryType directoryType, const std::string& parentDirectory,
	const std::vector<std::string> ext)
{
	std::vector<std::string> paths;
	for (const auto& extinstance : ext)
	{
		auto partPaths = GetFilesInDirectory(directoryType, parentDirectory, extinstance);
		for (auto partPath : partPaths)
		{
			paths.emplace_back(partPath);
		}
	}
	return paths;
}

std::string FileIO::SaveDialog(const char* filter)
{
	OPENFILENAMEA ofn; // common dialog box structure
	CHAR szFile[260] = { 0 };

	// Init OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);

	throw;
	//ofn.hwndOwner = glfwGetWin32Window(GetEngine().GetWindow().GetHandle());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	std::string resPath = GetPath(DirectoryType::Resource);
	ofn.lpstrInitialDir = resPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		std::filesystem::path filepath = std::filesystem::path(ofn.lpstrFile).lexically_relative(resPath);

		return filepath.string();
	}
	return std::string();
}