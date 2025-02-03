#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;
float System::GetDeltaTime() const
{
	ASSERT_MSG(LOG_SYSTEM, Registry, "Registry is not set, Did init not call System::Init?");
	float dt = Registry->ctx().get<float>("DeltaTime"_hs);

	return dt;
}
const Region* System::GetRegion() const
{
	return Registry->ctx().get<Region*>("Owner"_hs);
}

IMPLEMENT_REFLECT_OBJECT(System)
{
	meta.data<&System::IsEnabled>("m_Enabled"_hs)
		PROP_DISPLAYNAME("Enabled");
	meta.data<&System::RunInEditorMode>("m_runInEditor"_hs)
		PROP_DISPLAYNAME("RunInEditor")
		PROP_DESCRIPTION("This system wil execute all the time, even when the game is paused.")
		PROP_READONLY;

	meta.data<&System::ExecutionPriority>("m_priority"_hs)
		PROP_DISPLAYNAME("priority")
		PROP_DESCRIPTION("Priority of execution of the system, Can only be changed from code.")
		PROP_READONLY;
}
FINISH_REFLECT()