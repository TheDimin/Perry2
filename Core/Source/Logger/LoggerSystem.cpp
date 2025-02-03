#include "Core.h"
#pragma hdrstop

using namespace Perry;

std::string currentSaveFilePath = "";

namespace LoggerInternal
{
	LoggerSystem& GetLogger()
	{
		static LoggerSystem m_Logger{ false };

		return m_Logger;
	}
} // namespace LoggerInternal

LoggerSystem::LoggerSystem(const bool allowWritingToFile) : m_AllowWritingToFile(allowWritingToFile)
{

	//TEST FOR savign settings.

	//entt::meta_any obj = entt::meta_any(*this);
	//ObjectSerializer::SaveObject(obj,"Settings/Logger");

	//if(FileIO::Exist(DirectoryType::Save,"Settings/Logger.json"))
	//{
	//	entt::meta_any obj = entt::meta_any(this);
	//	ObjectSerializer::LoadObject(obj,"Settings/Logger");
	//}

	Init();
}

LoggerSystem::~LoggerSystem()
{
	Save();
	//entt::meta_any obj = entt::meta_any(*this);
	//ObjectSerializer::SaveObject(obj,"Settings/Logger");
}

void LoggerSystem::Init()
{
	// Find a valid log file index
	if (m_AllowWritingToFile)
		AllowFileWriting();

	std::string source = "";// Perry::LaunchParameters::GetString("LogInfo", "");
	if (!source.empty())
	{
		std::stringstream ss(source);
		std::string category;
		while (!ss.eof())
		{
			getline(ss, category, ',');
			SetLogCategory(ELogLevel::INFO, category.c_str(), true);
		}
	}
}

bool LoggerSystem::IsBlocked(ELogLevel Level, const char* Category)
{
	GLOBAL_SCOPE_LOCK()
		if ((m_BlockedLevelMask & Level) != ELogLevel::NONE)
			return true; // LogLevel is blocked

	auto categoryMask = m_BlockedCategories.find(Category);

	// If catogory doesn't exists create it and mute verbose
	if (categoryMask == m_BlockedCategories.end())
	{
		m_BlockedCategories.insert({ Category, ELogLevel::INFO });
		categoryMask = m_BlockedCategories.find(Category);
	}

	if ((categoryMask->second & Level) != ELogLevel::NONE)
		return true; // Category is blocked.

	return false;
}



void LoggerSystem::GenerateLogEntry(ELogLevel Level, const char* Category, const char* filePath, int lineNumber,
	const char* text)
{
	// Yes we have to do these checks in both functions,
	// At gen time it can can call this one directly if no formatting parameters are present
	if (IsBlocked(Level, Category))
		return;

	{
		GLOBAL_SCOPE_LOCK()

			SetColor(Level);

		LogMessage(("[" + GetTimeStampString() + "]").c_str());
		LogMessage(("[" + std::string(Category) + "]").c_str());
		LogMessage(("[" + FileIO::GetFileName(filePath) + ":" + std::to_string(lineNumber) + "]").c_str());
		LogMessage((LogLevelToString(Level) + " ").c_str());

		LogMessage(text);
		SetColor(ELogLevel::ALL); // we use all as a "reset" value here
		LogMessage("\n");
	}

	if (m_MemLog.size() > m_MaxMemLogSize)
	{
		// We cannot log assertions of fileio.... something horrible went wrong and we can't write..
		if (Level == ELogLevel::ASSERT && Category == LOG_FILEIO)
			return;

		Save();
	}
}

void LoggerSystem::Save()
{
	if (m_MemLog.empty() || !m_AllowWritingToFile)
		return;

	// We need to copy then clear as writeToFile will also log to memlog
	std::string memCopy = m_MemLog;
	m_MemLog.resize(0, '\0');

	ASSERT_MSG(LOG_LOGGING, !currentSaveFilePath.empty(), "Logger.Initialize has not been called");

	FileIO::Write(DirectoryType::Log, currentSaveFilePath, memCopy, true);

	// We don't want this to be logged to the text file.
	// Note that changing this may cause it to still not be written to the text file...
	INFO(LOG_LOGGING, "Saving logfile to disk [%s]...", FileIO::GetPath(DirectoryType::Log, currentSaveFilePath).c_str());
}

void LoggerSystem::LogMessage(const char* message)
{
	printf("%s", message);

	// Buildup Cache.. will be saved to disk later
	m_MemLog += std::string(message);
}

std::string LoggerSystem::GetTimeStampString()
{
	// Yes this implementation looks identical to pc.... LocalTime_s parameters are swapped around... thats why its not
	// the same...

	auto currentTime = (std::chrono::system_clock::now());
	auto startupTime = (std::chrono::system_clock::now());
	auto timeDiff = std::chrono::duration<float>(currentTime - startupTime).count();
	std::ostringstream timeStream;

	//+1 when timeDiff = 60 we want to showcase 1 minute, otherwise it only happens at 61.. idk why
	int minutes = static_cast<int>((timeDiff + 1) / 60);
	int seconds = static_cast<int>(std::fmod(timeDiff, 60.0f));

	// Use ostringstream to format the string
	if (minutes > 0)
		timeStream << std::setfill('0') << minutes << "m ";

	timeStream << std::setfill('0') << std::setw(2) << seconds << "s";

	return timeStream.str();
}

const std::string& LoggerSystem::LogLevelToString(ELogLevel logLevel)
{
	static const std::unordered_map<ELogLevel, std::string> TEXT_MAP{
		{ELogLevel::INFO, "[INFO]"}, {ELogLevel::LOG, "[LOG]"}, {ELogLevel::WARN, "[WARN]"}, {ELogLevel::EERROR, "[ERROR]"}, {ELogLevel::ASSERT, "[ASSERT]"} };

	const auto result = TEXT_MAP.find(logLevel);
	ASSERT(LOG_LOGGING, result != TEXT_MAP.end());

	return result->second;
}

void LoggerSystem::SetLogCategory(ELogLevel level, const char* category, bool enabled)
{
	auto categoryBlock = m_BlockedCategories.find(category);
	if (categoryBlock == m_BlockedCategories.end())
	{
		m_BlockedCategories.insert({ category, ELogLevel::NONE });
		categoryBlock = m_BlockedCategories.find(category);
	}

	// Idealy we auto disable/enable lower priority categories, but math for it is funky
	if (enabled)
		for (ELogLevel i = level; (level <= categoryBlock->second) && categoryBlock->second != ELogLevel::NONE;
			i = static_cast<ELogLevel>(i << 1))
			categoryBlock->second = static_cast<ELogLevel>(categoryBlock->second & ~i);
	else
		for (ELogLevel i = level; i > ELogLevel::NONE; i = static_cast<ELogLevel>(i >> 1))
			categoryBlock->second = static_cast<ELogLevel>(categoryBlock->second | i);
}

void LoggerSystem::SetLogLevel(ELogLevel level, bool enabled)
{
	if (enabled)
		for (ELogLevel i = level; (level <= m_BlockedLevelMask) && m_BlockedLevelMask != ELogLevel::NONE;
			i = static_cast<ELogLevel>(i << 1))
			m_BlockedLevelMask = static_cast<ELogLevel>(m_BlockedLevelMask & ~i);
	else
		for (ELogLevel i = level; i > ELogLevel::NONE; i = static_cast<ELogLevel>(i >> 1))
			m_BlockedLevelMask = static_cast<ELogLevel>(m_BlockedLevelMask | i);
}

const std::string& LoggerSystem::GetLogCache()
{
	return m_MemLog;
}

void LoggerSystem::AllowFileWriting()
{
	if (!currentSaveFilePath.empty())
	{
		WARN(LOG_LOGGING, "Attempted to allow writing to file while this is already");
		return;
	}

	for (unsigned int i = 0; i < UINT_MAX; ++i)
	{
		if (!FileIO::Exist(DirectoryType::Log, (std::to_string(i) + ".log")))
		{
			currentSaveFilePath = std::to_string(i) + ".log";
			break;
		}
	}
	m_AllowWritingToFile = true;

	// Check if we should write current data to file...
	if (m_MemLog.size() > m_MaxMemLogSize)
	{
		Save();
	}
}


IMPLEMENT_REFLECT_OBJECT(LoggerSystem)
{
	meta.data<&LoggerSystem::m_BlockedCategories>("BlockedCategory"_hs)
		PROP_DISPLAYNAME("Blocked Categories");

	meta.data<&LoggerSystem::m_BlockedLevelMask>("BlockedLevelMask"_hs)
		PROP_DISPLAYNAME("Blocked Mask");

	meta.data<&LoggerSystem::m_MaxMemLogSize>("MaxMemLogSize"_hs)
		PROP_DISPLAYNAME("Max Memory LogFile Size");
}
FINISH_REFLECT()

IMPLEMENT_REFLECT_ENUM(ELogLevel)
{
	meta.data<ELogLevel::NONE>("LOG_NONE"_hs)
		PROP_DISPLAYNAME("NONE");
	meta.data<ELogLevel::INFO>("LOG_INFO"_hs)
		PROP_DISPLAYNAME("INFO");
	meta.data<ELogLevel::LOG>("LOG_LOG"_hs)
		PROP_DISPLAYNAME("LOG");
	meta.data<ELogLevel::WARN>("LOG_WARN"_hs)
		PROP_DISPLAYNAME("WARN");
	meta.data<ELogLevel::EERROR>("LOG_ERROR"_hs)
		PROP_DISPLAYNAME("ERROR");
	meta.data<ELogLevel::ASSERT>("LOG_ASSERT"_hs)
		PROP_DISPLAYNAME("ASSERT");
	meta.data<ELogLevel::ALL>("LOG_ALL"_hs)
		PROP_DISPLAYNAME("ALL");
}
FINISH_REFLECT()