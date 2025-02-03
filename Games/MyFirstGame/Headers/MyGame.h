#pragma once
#include "GameBase.h"


class MyGame :public Perry::GameBase
{
public:
	MyGame() : GameBase() {};
	void Init() override;
	void Update(float DeltaTime) override;
	void ShutDown() override;
};