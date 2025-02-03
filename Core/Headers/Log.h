#pragma once

namespace LoggerInternal
{
	Perry::LoggerSystem& GetLogger();
}

constexpr const char* LOG_GENERIC = "GENERIC";
constexpr const char* LOG_LOGGING = "LOGGING";
constexpr const char* LOG_FILEIO = "FILEIO";
constexpr const char* LOG_GAMEOBJECTS = "GAME-OBJECTS";
constexpr const char* LOG_SERIALIZER = "SERIALIZER";
constexpr const char* LOG_INPUT = "INPUT";
constexpr const char* LOG_GRAPHICS = "GRAPHICS";
constexpr const char* LOG_RESOURCE = "RESOURCES";
constexpr const char* LOG_LAUNCHPARAM = "LAUNCH-PARAMS";
constexpr const char* LOG_AUDIO = "AUDIO";
constexpr const char* LOG_PHYSICS = "PHYSICS";
constexpr const char* LOG_ULTRALIGHT = "ULTRALIGHT"; // used for logs from ultralight Libary
constexpr const char* LOG_UI = "UI"; // Used for our UI code/system
constexpr const char* LOG_SYSTEM = "SYSTEM";
constexpr const char* LOG_ARCHIVE = "ARCHIVE";
constexpr const char* LOG_REGION = "REGION";
constexpr const char* LOG_TOOL = "TOOL";
constexpr const char* LOG_WORLD = "WORLD";
constexpr const char* LOG_FIBER = "FIBER";
constexpr const char* LOG_LUA = "LUA";
constexpr const char* LOG_PLATFORM = "STEAM"; //COULD BE DIFFERENT DEPENDING ON PLATFORM
constexpr const char* LOG_NETWORKING = "STEAM-SOCKET";

constexpr const char* LOG_RTS = "RTS";

#undef ERROR

#define ASSERT(LOG_CATEGORY, CONDITION) ASSERT_MSG(LOG_CATEGORY, CONDITION, "")


#ifdef ENABLE_LOGGING
#define LOG_CLEAR() LoggerInternal::GetLogger().Clear()
#define LOG_ENABLE(LOG_CATEGORY, LOG_LEVEL) LoggerInternal::GetLogger().SetLogCategory(LOG_LEVEL, LOG_CATEGORY, true)
#define LOG_DISABLE(LOG_CATEGORY, LOG_LEVEL) LoggerInternal::GetLogger().SetLogCategory(LOG_LEVEL, LOG_CATEGORY, false)
#define LOG_LEVEL_ENABLE(LOG_LEVEL) LoggerInternal::GetLogger().SetLogLevel(LOG_LEVEL, true)
#define LOG_LEVEL_DISABLE(LOG_LEVEL) LoggerInternal::GetLogger().SetLogLevel(LOG_LEVEL, false)

#define INFO(LOG_CATEGORY, ...)                   \
	LoggerInternal::GetLogger().GenerateLogEntry( \
		Perry::Logger::ELogLevel::INFO, LOG_CATEGORY, __FILE__, __LINE__, __VA_ARGS__)
#define LOG(LOG_CATEGORY, ...)                    \
	LoggerInternal::GetLogger().GenerateLogEntry( \
		Perry::Logger::ELogLevel::LOG, LOG_CATEGORY, __FILE__, __LINE__, __VA_ARGS__)
#define WARN(LOG_CATEGORY, ...)                   \
	LoggerInternal::GetLogger().GenerateLogEntry( \
		Perry::Logger::ELogLevel::WARN, LOG_CATEGORY, __FILE__, __LINE__, __VA_ARGS__)
#define ERROR(LOG_CATEGORY, ...)                  \
	do {LoggerInternal::GetLogger().GenerateLogEntry( \
		Perry::Logger::ELogLevel::EERROR, LOG_CATEGORY, __FILE__, __LINE__, __VA_ARGS__);\
	if constexpr (sizeof(#__VA_ARGS__) > 1)                                                       \
			{                                                                                             \
				Perry::AssertWindow(LOG_CATEGORY, __func__, __LINE__, __FILE__, __VA_ARGS__);              \
			}                                                                                             \
			else                                                                                          \
			{                                                                                             \
				Perry::AssertWindow(                                                                       \
					LOG_CATEGORY, __func__, __LINE__, __FILE__, "Condition failed, no extra info given"); \
			}\
	__debugbreak();} while(0)

#ifndef PLATFORM_WINDOWS
#define ASSERT_MSG(LOG_CATEGORY, CONDITION, ...)                                                          \
	do                                                                                                    \
	{                                                                                                     \
		if (!(CONDITION))                                                                                 \
		{                                                                                                 \
			if constexpr (sizeof(#__VA_ARGS__) > 1)                                                       \
			{                                                                                             \
				LoggerInternal::GetLogger().GenerateLogEntry(                                             \
					Perry::Logger::ELogLevel::ASSERT, LOG_CATEGORY, __FILE__, __LINE__, __VA_ARGS__);     \
			}                                                                                             \
			else                                                                                          \
			{                                                                                             \
				LoggerInternal::GetLogger().GenerateLogEntry(Perry::Logger::ELogLevel::ASSERT,            \
															 LOG_CATEGORY,                                \
															 __FILE__,                                    \
															 __LINE__,                                    \
															 "Condition failed, no extra info given");    \
			}                                                                                             \
			if (LOG_CATEGORY != LOG_FILEIO) /*Only save if its not fileio crashing*/                      \
				LoggerInternal::GetLogger().Save();                                                       \
			if constexpr (sizeof(#__VA_ARGS__) > 1)                                                       \
			{                                                                                             \
				Perry::AssertWindow(LOG_CATEGORY, __func__, __LINE__, __FILE__, __VA_ARGS__);              \
			}                                                                                             \
			else                                                                                          \
			{                                                                                             \
				Perry::AssertWindow(                                                                       \
					LOG_CATEGORY, __func__, __LINE__, __FILE__, "Condition failed, no extra info given"); \
			}\
		}                                                                                                 \
	} while (0)
#else

#define ASSERT_MSG(LOG_CATEGORY, CONDITION, ...)                                                          \
	do                                                                                                    \
	{                                                                                                     \
		if (!(CONDITION))                                                                                 \
		{                                                                                                 \
			if constexpr (sizeof(#__VA_ARGS__) > 1)                                                       \
			{                                                                                             \
				LoggerInternal::GetLogger().GenerateLogEntry(                                             \
					Perry::Logger::ELogLevel::ASSERT, LOG_CATEGORY, __FILE__, __LINE__, __VA_ARGS__);     \
			}                                                                                             \
			else                                                                                          \
			{                                                                                             \
				LoggerInternal::GetLogger().GenerateLogEntry(Perry::Logger::ELogLevel::ASSERT,            \
															 LOG_CATEGORY,                                \
															 __FILE__,                                    \
															 __LINE__,                                    \
															 "Condition failed, no extra info given");    \
			}                                                                                             \
			if (LOG_CATEGORY != LOG_FILEIO) /*Only save if its not fileio crashing*/                      \
				LoggerInternal::GetLogger().Save();                                                       \
			if constexpr (sizeof(#__VA_ARGS__) > 1)                                                       \
			{                                                                                             \
				Perry::AssertWindow(LOG_CATEGORY, __func__, __LINE__, __FILE__, __VA_ARGS__);              \
			}                                                                                             \
			else                                                                                          \
			{                                                                                             \
				Perry::AssertWindow(                                                                       \
					LOG_CATEGORY, __func__, __LINE__, __FILE__, "Condition failed, no extra info given"); \
			}\
			__debugbreak();\
		}                                                                                                 \
	} while (0)
#endif


#else
#define LOG_ENABLE(LOG_CATEGORY, LOG_LEVEL) \
	do                                      \
	{                                       \
		(void)sizeof(LOG_CATEGORY);         \
		(void)sizeof(LOG_LEVEL);            \
	} while (0)
#define LOG_DISABLE(LOG_CATEGORY, LOG_LEVEL) \
	do                                       \
	{                                        \
		(void)sizeof(LOG_CATEGORY);          \
		(void)sizeof(LOG_LEVEL);             \
	} while (0)
#define LOG_LEVEL_ENABLE(LOG_LEVEL) \
	do                              \
	{                               \
		(void)sizeof(LOG_LEVEL);    \
	} while (0)
#define LOG_LEVEL_DISABLE(LOG_LEVEL) \
	do                               \
	{                                \
		(void)sizeof(LOG_LEVEL);     \
	} while (0)

#define INFO(LOG_CATEGORY, ...)     \
	do                              \
	{                               \
		(void)sizeof(LOG_CATEGORY); \
		(void)sizeof(__VA_ARGS__);  \
	} while (0)
#define LOG(LOG_CATEGORY, ...)      \
	do                              \
	{                               \
		(void)sizeof(LOG_CATEGORY); \
		(void)sizeof(__VA_ARGS__);  \
	} while (0)
#define WARN(LOG_CATEGORY, ...)     \
	do                              \
	{                               \
		(void)sizeof(LOG_CATEGORY); \
		(void)sizeof(__VA_ARGS__);  \
	} while (0)
#define ERROR(LOG_CATEGORY, ...)    \
	do                              \
	{                               \
		(void)sizeof(LOG_CATEGORY); \
		(void)sizeof(__VA_ARGS__);  \
	} while (0)

#define ASSERT_MSG(LOG_CATEGORY, CONDITION, ...) \
	do                                           \
	{                                            \
		(void)sizeof(LOG_CATEGORY);              \
		(void)sizeof(CONDITION);                 \
		(void)sizeof(__VA_ARGS__);               \
	} while (0)
#define LOG_CLEAR() \
	do              \
	{               \
	} while (0)
#endif