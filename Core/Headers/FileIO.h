#pragma once

namespace Perry
{
#undef DeleteFile

	/// <summary>
	/// Directory type, each type can be mapped to a different file location, based on platform
	///	Not all directory types support writing !
	/// </summary>
	enum class DirectoryType
	{
		Resource, // Engine Resources, Cross platform content!
		Shaders, // Shader file location (Platform specific).
		Save, // Platform specific file location
		Log, // Console log file location
	};

	class FileIO
	{
	public:
		FileIO() = delete;
		/// <summary>
		/// Initializes the FileIO
		/// </summary>
		static void Init();

		static std::string WorkingDirectory();

		/// <summary>
		/// Checks if <targetFile> exists in the directory type
		/// </summary>
		/// <param name="type">The directory type we are looking for</param>
		/// <param name="targetFile">Relative file path we are going to check</param>
		/// <returns></returns>
		static bool Exist(DirectoryType type, const std::string& targetFile);

		/// <summary>
		/// Gets file name with extension
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		static std::string GetFileName(const std::string& path);

		/// <summary>
		/// Writes a string to the given file.
		///	Note that you cannot write to every DirectoryType
		/// </summary>
		/// <param name="type">The directoryType we try to write to</param>
		/// <param name="relativePath">The filepath where we will save this file</param>
		/// <param name="data">The data we will be writing to the file</param>
		/// <param name="appendData">If this is true we append the data to end of the file, otherwise overwrite the
		/// whole file.</param> <returns>If we wrote to the file successfully</returns>
		static bool Write(DirectoryType type, const std::string& relativePath, const std::string& data,
			bool appendData = false);
		/// <summary>
		/// Write binary data to a file
		/// </summary>
		/// <param name="type">The directoryType we try to write to</param>
		/// <param name="relativePath">The filepath where we will save this file</param>
		/// <param name="data">The memory pointer where we read the data from</param>
		/// <param name="size">How much bytes we will write to disk (Data ,Data+Size)</param>
		/// <returns>If we wrote to the file successfully</returns>
		static bool WriteBinary(DirectoryType type, const std::string& relativePath, const void* data, size_t size, bool appendData = false);

		/// <summary>
		/// Reads a file into a string
		/// </summary>
		/// <param name="type">The directoryType we try to write to</param>
		/// <param name="relativePath">The filepath where we will save this file</param>
		/// <returns>the read data, empty string if reading failed</returns>
		static std::string Read(DirectoryType type, const std::string& relativePath);
		/// <summary>
		/// Reads a file into the supplied targetBuffer.
		/// </summary>
		/// <param name="type">The directoryType we try to write to</param>
		/// <param name="relativePath">The filepath where we will save this file</param>
		/// <param name="targetBuffer">The buffer the file will be read into</param>
		/// <param name="targetBufferSize">The max amount of bytes we can write to the target buffer</param>
		/// <returns>If we read to the file successfully</returns>
		static bool ReadBinary(DirectoryType type, const std::string& relativePath, void* targetBuffer,
			std::streamsize targetBufferSize);

		/// <summary>
		/// Deletes the given file, Note that not all directory types support deleting
		/// </summary>
		/// <param name="type">The directoryType we try to write to</param>
		/// <param name="relativePath">The file we want to delete</param>
		/// <returns>If we deleted the file Successfully</returns>
		static bool Delete(DirectoryType type, const std::string& relativePath);

		/// <summary>
		/// Get the size of the file
		/// </summary>
		/// <param name="type"></param>
		/// <param name="relativePath"></param>
		/// <returns>size of file in bytes</returns>
		static size_t GetSize(DirectoryType type, const std::string& relativePath);

		/// <summary>
		/// Get the absolute path for the directory type
		/// </summary>
		/// <param name="type">The directory type for which we want the absolute path</param>
		/// <returns>The absolute file path as string</returns>
		static std::string GetPath(DirectoryType type);
		/// <summary>
		/// Get the absolute path for the directory type with the appended relative path
		/// </summary>
		/// <param name="type">The directory type for which we want the absolute path</param>
		/// <param name="relativePath">A generic filepath we append to the given string</param>
		/// <returns>The absolute file path as string</returns>
		static std::string GetPath(DirectoryType type, const std::string& relativePath);

		/// <summary>
		/// Can this directory be written to
		/// </summary>
		/// <param name="type">The directory type to check for</param>
		/// <returns>True if we can write to it</returns>
		static bool HasWriteAccess(DirectoryType type);


		/// <summary>
		/// Opens a dialog window (On Windows) to select a file with a given filter
		/// </summary>
		/// <param name="std::string filter:">Specifies an extension filter so only those type of files show in the dialog</param>
		/// <returns>Returns a relative to the "Resources" folder file path to the selceted file; or empty string if it fails</returns>
		static std::string OpenDialog(const char* filter);


		/// <summary>
		/// Opens a dialog window (On Windows) to save a file of the specified filter
		/// </summary>
		/// <param name="std::string filter:">Specifies an extension filter so only those type of files show in the dialog</param>
		/// <returns>Returns a relative to the "Resources" folder file path to the (to be) saved file; or empty string if it fails</returns>
		static std::string SaveDialog(const char* filter);

		static std::vector<std::string> GetFilesInDirectory(DirectoryType directoryType, const std::string& parentDirectory);
		static std::vector<std::string> GetFilesInDirectory(DirectoryType directoryType, const std::string& parentDirectory, const std::string& ext);
		static std::vector<std::string> GetFilesInDirectory(DirectoryType directoryType, const std::string& parentDirectory, const std::vector<std::string> ext);


	private:

		static std::string m_ResourcesPath;
	};
}