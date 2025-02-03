#pragma once


#define IMPLEMENT_REFLECT_SUBSYSTEM(ClassName)

namespace Perry
{
	class EngineManager;
	class ISubSystem
	{
	protected:
		friend EngineManager;
		virtual ~ISubSystem() = default;

		virtual void Init() = 0;
		virtual void Update(float DeltaTime) = 0;
		virtual void ShutDown() = 0;
	};
}
