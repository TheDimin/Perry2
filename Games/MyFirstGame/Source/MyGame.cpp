#include "EnginePCH.h"
#include "MyGame.h"


void MyGame::Init()
{
	auto* world = Perry::World::CreateWorld();
	Perry::World::SetWorld(world);

	auto model = Perry::Entity::Create(world->Regions[0]->Registry);

	model.LoadModel("Models/Avocado/Avocado.gltf");
	GameBase::Init();
}

void MyGame::Update(float DeltaTime)
{
	GameBase::Update(DeltaTime);
}

void MyGame::ShutDown() {}