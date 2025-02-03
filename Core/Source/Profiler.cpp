#include "Core.h"
#pragma hdrstop

uint32_t Perry::Profiler::ProfilerScope::GetColorCode(const char* category)
{
	if (category == nullptr)
		return PERFORMANCEAPI_MAKE_COLOR(0,124,124);

	if (std::string(category) == std::string("Graphics"))
		return 0xFF0000;

	if (std::string(category) == std::string("Engine"))
		return 0x00FF00;


	return 0xFF00FF;
}