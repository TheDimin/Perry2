#pragma once

namespace Perry
{
	namespace Profiler
	{
		struct ProfilerScope
		{
			static uint32_t GetColorCode(const char* category);
			ProfilerScope(const char* name, const char* category = nullptr)
			{
				PerformanceAPI_BeginEvent(name, NULL, GetColorCode(category));
			}

			~ProfilerScope() { PerformanceAPI_EndEvent(); }
		};
	} // namespace Profiler
}


//TODO lookup table for event name to color
#define PROFILER_MARK(name) Perry::Profiler::ProfilerScope scope = {name,nullptr};
#define PROFILER_MARKC(name,category) Perry::Profiler::ProfilerScope scope = {name,category};

#define PROFILER_NAME_THREAD(name) PerformanceAPI_SetCurrentThreadName(name);

#define PROFILER_FIBER_REGISTER(ID,name) PerformanceAPI_RegisterFiber((uint64_t)(uintptr_t)ID);
#define PROFILER_FIBER_REMOVE(ID) PerformanceAPI_RemoveFiber((uint64_t)(uintptr_t)ID);

#define PROFILER_FIBER_SWITCH(oldID, newID) PerformanceAPI_BeginFiberSwitch((uint64_t)(uintptr_t)oldID, (uint64_t)(uintptr_t)newID);
#define PROFILER_FIBER_RETURN(oldID) PerformanceAPI_EndFiberSwitch((uint64_t)(uintptr_t)oldID);