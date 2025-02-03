#include "EnginePCH.h"

using namespace  Perry;

static void BeginCollideEvent(CollisionComponent& col, CollisionComponent& col2)
{
	printf("BeginCollide event called \n");
}

static void MultipleBeginCollideEvents(CollisionComponent& col, CollisionComponent& col2)
{
	printf("An other binding to BeginCollide \n");
}

static void OnCollideEvent(CollisionComponent& col, CollisionComponent& col2)
{
	printf("OnCollide \n");
}

static void EndCollideEvent(CollisionComponent& col, CollisionComponent& col2)
{
	printf("OnCollide End\n");
}


void CollisionExampleLevel::OnLoad()
{

#ifndef BUBBLES
	//Always create a camera Entity
	{
		Perry::Entity e = AddEntity(glm::vec3(0.5f, 3.f, -1.f),glm::vec3(90.f,0.f,0.f));
		e.AddComponent<CameraComponent>();
		e.AddComponent<FlyingMovementComponent>();
		e.SetName("Camera");
	}
	//"Player"
	{
		Entity e = AddEntity(glm::vec3(0, 0, -1.0f), glm::vec3(0.0f, 180.0f, 0.0f),glm::vec3(0.005f));
		e.LoadModel("Models/Duck/Duck.gltf");
		e.AddComponent<CollisionComponent>(glm::vec2(1), LAYER_1);
		e.AddComponent<VelocityComponent>();
		e.SetName("Player");
	}
	//Static collision Example
	{
		Entity e = AddEntity(glm::vec3(1, 0, -1.0f), glm::vec3(0.0f, 180.0f, 0.0f),glm::vec3(0.005f));
		e.LoadModel("Models/Duck/Duck.gltf");
		e.SetName("Responder");

		auto& coll = e.AddComponent<CollisionComponent>(glm::vec2(1), LAYER_1);
		coll.m_IsStatic = true;

		coll.m_OnCollideStart.connect<&BeginCollideEvent>();
		coll.m_OnCollideStart.connect<&MultipleBeginCollideEvents>();

		coll.m_OnCollide.connect<&OnCollideEvent>();

		coll.m_OnCollideEnd.connect<&EndCollideEvent>();
	}
#endif

}