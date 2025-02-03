#include "EnginePCH.h"
#pragma hdrstop

#include "Engine.h"
#include "Audio/AudioSystem.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/FlyingMovementComponent.h"
#include "ECS/Entity.h"


#ifdef PlaySound
#undef PlaySound
#endif

using namespace Perry;

void AudioExampleLevel::OnLoad()
{
	Level::OnLoad();
	
	Entity e = AddEntity();
	e.AddComponent<CameraComponent>();
	e.AddComponent<FlyingMovementComponent>();
	e.SetName("Camera");

	AudioSystem& audio = GetEngine().GetAudio();

	masterBank = audio.LoadBank("Master.bank");
	stringBank = audio.LoadBank("Master.strings.bank");
	SFXBank = audio.LoadBank("SFX.bank");

	auto l = audio.GetEvent("event:/Ambience/Country");

	l->start();
	l->setParameterByNameWithLabel("Hour", "Evening");

	audioEffect = audio.PlaySound("640148main_APU Shutdown.ogg");
}

void AudioExampleLevel::OnUnload()
{
	Level::OnUnload();

	masterBank->unload();
	stringBank->unload();
	SFXBank->unload();
	audioEffect->release();
}