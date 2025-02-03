#include "EnginePCH.h"
#pragma hdrstop

#include "PathFinding/PathFindingDemoLevel.h"

#include "Debug.h"
#include "Transform.h"
#include "Graphics/Renderer.h"
#include "Collision/CollisionLayerSetup.h"
#include "ECS/Entity.h"
#include "ECS/Components/FlyingMovementComponent.h"
#include "ECS/Components/TransformComponent.h"

#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/CollisionComponent.h"

static entt::entity FollowedEntity;
static entt::entity CameraEntity;

void Perry::PathFindingDemoLevel::OnLoad()
{
	//Always create a camera Entity
	{
		Perry::Entity e = AddEntity();
		e.AddComponent<CameraComponent>();
		e.AddComponent<FlyingMovementComponent>();
		Transform::SetRotation(e.FindComponent<TransformComponent>(), glm::vec3(11.84f, 10, 13.628f));
		CameraEntity = e.GetID();
	}
	//"Player"
	{
		Entity e = AddEntity(glm::vec3(0, 0, -1.0f), glm::vec3(0.0f, 180.0f, 0.0f));
		e.LoadModel("Models/Duck/Duck.gltf");
		FollowedEntity = e.GetID();
	}
	//Static collision Example
	{
		Entity e = AddEntity(glm::vec3(5, 5, -1.0f), glm::vec3(0.0f, 180.0f, 0.0f));
		e.LoadModel("Models/Duck/Duck.gltf");
		e.AddComponent<CollisionComponent>(glm::vec2(1, 1), LAYER_STATIC);
	}

	//Call this after all static objects are loaded
	nm.Generate(glm::vec2(10, 10), glm::vec2(2, 2));
}

void Perry::PathFindingDemoLevel::Update(float deltaTime)
{
	Level::Update(deltaTime);

	//Call this to recalculate the path
	entt::registry& reg = GetRegistry();
	auto& FollowedTransform = reg.get<TransformComponent>(FollowedEntity);

	m_path = nm.CalculatePath(Transform::GetPosition(FollowedTransform), glm::vec3(2, 2, 0));

	nm.Update();
	//TODO parrent camera to Followed Transform .. Was not a thing before
	//CameraTransform.m_Position.x = FollowedTransform.m_Position.x;
	//CameraTransform.m_Position.y = FollowedTransform.m_Position.y;

	auto dn = nm.FindClosestNode(Transform::GetPosition(FollowedTransform));

	glm::vec2 d = nm.GetNodeSize() - glm::vec2(0.5f);

	Debug::DrawBox(glm::vec3(m_path[0]->pos, 0), glm::vec3(d, 0), glm::vec3(0, 0, 1));
	Debug::DrawBox(glm::vec3((m_path[m_path.size() - 1]->pos), 0), glm::vec3(d, 0), glm::vec3(0, 0, 1));

	Debug::DrawBox(glm::vec3(dn->pos, 0), glm::vec3(d, 0), glm::vec3(0, 0, 1));

	if (!m_path.empty())
		for (int i = 0; i < m_path.size() - 1; ++i)
			Debug::DrawLine(glm::vec3(m_path[i]->pos, 0), glm::vec3(m_path[i + 1]->pos, 0), glm::vec3(1, 1, 1));
}
