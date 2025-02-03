#include "EnginePCH.h"
#pragma hdrstop

#include "GameBase.h"

#include "World/World.h"

void Perry::GameBase::Init()
{
}

void Perry::GameBase::Update(float deltaTime)
{
	PROFILER_MARK("GameLoop")
	ElapsedSeconds += deltaTime;
	World::GetWorld().Update(deltaTime);
}

void Perry::GameBase::ShutDown()
{
}