#pragma once
#include "GameBase.h"


class UnitTestsGame :public Perry::GameBase
{
public:
	void Init() override;
	void Update(float DeltaTime) override;
	void ShutDown() override;
};