#include "EnginePCH.h"
#pragma hdrstop


#ifdef ENGINE_DEPRECATED
using namespace Perry;

IMPLEMENT_REFLECT_OBJECT(Level)
{
}
FINISH_REFLECT()

void Level::OnLoadInternal()
{
	auto parentType = entt::resolve<System>();

	for (auto&& [id, metaType] : entt::resolve())
	{
		if (!metaType.is_class())
			continue;

		bool isSystem = false;
		for (auto&& [id, IttPType] : metaType.base())
		{
			if (parentType.id() == IttPType.id())
			{
				isSystem = true;
				break;
			}
		}

		if (!isSystem)
			continue;

		auto newSystem = metaType.construct();
		int systemPriority = newSystem.get("m_priority"_hs).cast<int>();

		if (m_SystemInstances.empty())
		{
			m_SystemInstances.insert(m_SystemInstances.begin(), newSystem);
		}
		else
		{
			[[maybe_unused]] bool insertedSystem = false;
			for (size_t i = 0; i < m_SystemInstances.size(); i++)
			{
				int p = m_SystemInstances[i].get("m_priority"_hs).cast<int>();

				if (systemPriority >= p || i == m_SystemInstances.size() - 1)
				{
					m_SystemInstances.insert(m_SystemInstances.begin() + i, newSystem);
					insertedSystem = true;
					break;
				}
			}

			assert(insertedSystem);
		}
	}

	for (auto& systemAny : m_SystemInstances)
	{
		System* systemInstance = systemAny.try_cast<System>();
		systemInstance->Registry = &registry;
		systemInstance->Init(registry);
	}

	registry.ctx().emplace_as<const float&>("DeltaTime"_hs, GetEngine().GetDeltaTime());

	OnLoad();
}

entt::registry& Level::GetRegistry()
{
	return registry;
}

Level::~Level()
{
	for (auto& system : m_SystemInstances)
	{
		System* sb = system.try_cast<System>();
		sb->ShutDown(registry);
	}

	registry.clear();
	registry = {};
}

void Level::OnLoad()
{
	// Always create a camera Entity
	/*Perry::Entity e = AddEntity();
	e.AddComponent<Perry::CameraComponent>();
	e.AddComponent<FlyingMovementComponent>();
	e.SetName("Camera");*/
}

void Level::UpdateSystems(float deltaTime)
{
	for (size_t i = 0; i < m_SystemInstances.size(); i++)
	{
		auto& system = m_SystemInstances[i];

		System* sb = system.try_cast<System>();
		if ((!GetEngine().GetGame().IsGamePaused() || sb->RunInEditorMode) && sb->IsEnabled)
		{
			sb->Update(registry);
		}
	}
}

Entity Level::AddEntity(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
	Entity el = Entity::Create(registry,position,rotation, scale);

	return el; // NOLINT(clang-diagnostic-return-stack-address)
}

void Perry::Level::RemoveEntity(Entity& e)
{
	e.Delete();
}

#endif