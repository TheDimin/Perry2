#include "pch.h"

using namespace Perry;

void LoggerSystem::Clear(bool clearConsole)
{
	m_MemLog.clear();
	if (clearConsole)
		system("cls");
}

void LoggerSystem::SetColor(ELogLevel level)
{
	static const std::unordered_map<ELogLevel, int> m_ColorLookup{
		{ELogLevel::INFO, 8}, {ELogLevel::LOG, 7}, {ELogLevel::WARN, 6}, {ELogLevel::EERROR, 4}, {ELogLevel::ASSERT, 4}, {ELogLevel::ALL, 7} };

	const auto foundColor = m_ColorLookup.find(level);
	ASSERT(LOG_LOGGING, foundColor != m_ColorLookup.end());

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, foundColor->second);
}

void Perry::AssertWindow(const char* Category, const char* func, int line, const char* file, const char* message)
{
	std::ostringstream messageStream;
	messageStream << "Assertion failed ! \n";
	messageStream << file << "->" << func << "() [" << line << "]\n";
	messageStream << message;

	MessageBoxA(NULL,
		messageStream.str().c_str(),
		("[" + std::string(Category) + "] Assertion Failed").c_str(),
		MB_ICONERROR | MB_OK);
}