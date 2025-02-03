#pragma once
//#undef LivePP
#ifdef LivePP
#include "ECS/System.h"
//Yeah including this here is not ideal, but theoretically no one should access this anyway...


namespace lpp
{
	struct LppSynchronizedAgent;
}

class HotReloadSystem : public Perry::System
{
public:
	HotReloadSystem();
	void Init(entt::registry& reg) override;
	void Update(entt::registry& reg) override;
	void ShutDown(entt::registry& reg) override;

private:
	REFLECT()
};
#endif