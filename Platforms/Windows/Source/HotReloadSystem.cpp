#include "pch.h"
#include "Engine.h"
#include "ECS/Systems/MetaSystemDefines.h"
#include "ECS/Systems/MetaReflectSystem.h"
#pragma hdrstop

#ifdef LivePP

#include "LivePP/API/LPP_API_x64_CPP.h"

HotReloadSystem::HotReloadSystem()
{
	//Nothing should run before this...
	ExecutionPriority = INT_MAX;
	RunInEditorMode = true;
}


static lpp::LppSynchronizedAgent lppAgent;
static bool InitOnce = false;

void HotReloadSystem::Init(entt::registry& reg)
{
	System::Init(reg);
	if (InitOnce) return;

	//Cursed filepath, how do we plan on fixing this? test..
	lppAgent = lpp::LppCreateSynchronizedAgent(L"C:/Projects/Y2022B-Y2-PR-Platypus/Platforms/Windows/External/LivePP");
	lppAgent.EnableModule(lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr);
	InitOnce = true;

	if (!Perry::GetInput().ContainsAction("EDITOR_RELOAD"))
	{
		Perry::GetInput().CreateAction("EDITOR_RELOAD").Key(Perry::KEY_F11);
	}
}

void HotReloadSystem::Update(entt::registry& reg)
{
	if (Perry::GetInput().GetAction("EDITOR_RELOAD"))
	{
		for (auto& shader : Perry::ResourceManager<Perry::Shader>::GetAll())
		{
			shader->Rebuild();
		}

		for (auto& shader : Perry::ResourceManager<Perry::ComputeShader>::GetAll())
		{
			shader->Rebuild();
		}

		//lppAgent.ScheduleReload();
	}

	// listen to hot-reload and hot-restart requests
	if (lppAgent.WantsReload())
	{
		// client code can do whatever it wants here, e.g. synchronize across several threads, the network, etc.
		// ...
		//printf("Wants reload\n");
		lppAgent.CompileAndReloadChanges(lpp::LPP_RELOAD_BEHAVIOUR_WAIT_UNTIL_CHANGES_ARE_APPLIED);
	}
	////
	if (lppAgent.WantsRestart())
	{
		// client code can do whatever it wants here, e.g. finish logging, abandon threads, etc.
		// ...
		lppAgent.Restart(lpp::LPP_RESTART_BEHAVIOUR_INSTANT_TERMINATION, 0u);
	}
}

void HotReloadSystem::ShutDown(entt::registry& reg)
{
	//System::ShutDown(reg);
	lpp::LppDestroySynchronizedAgent(&lppAgent);
}

IMPLEMENT_REFLECT_SYSTEM(HotReloadSystem)
{

}
FINISH_REFLECT();
#endif