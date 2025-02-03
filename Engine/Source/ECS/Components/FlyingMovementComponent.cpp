#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_COMPONENT(FlyingMovementComponent)
{
	meta.data<&FlyingMovementComponent::m_MovementSpeed>("M_Speed"_hs) PROP_DISPLAYNAME("MovementSpeed")
		PROP_DRAGSPEED(0.5f) PROP_MINMAX(0.f, 15.f);

	meta.data<&FlyingMovementComponent::m_RotationSpeed>("M_rotate"_hs) PROP_DISPLAYNAME("RotationSpeed")
		PROP_DRAGSPEED(0.5f) PROP_MINMAX(0.f, 15.f);

	meta.data<&FlyingMovementComponent::m_Yaw>("M_yaw"_hs) PROP_DISPLAYNAME("Yaw") PROP_DRAGSPEED(0.1f)
		PROP_NOT_SERIALIZABLE;

	meta.data<&FlyingMovementComponent::m_Pitch>("M_pitch"_hs) PROP_DISPLAYNAME("Pitch") PROP_DRAGSPEED(0.1f)
		PROP_NOT_SERIALIZABLE;
}
FINISH_REFLECT()

IMPLEMENT_REFLECT_SYSTEM(FlyMovementSystem)
{
	meta.data<&FlyMovementSystem::m_InvertMouse>("m_invertMosue"_hs) PROP_DISPLAYNAME("Invert Mouse");

	meta.data<&FlyMovementSystem::m_MouseSensitivity>("m_MouseSensitivity"_hs) PROP_DISPLAYNAME("Mouse Sensitivity")
		PROP_DRAGSPEED(0.1f) PROP_MINMAX(0.f, 10000.f);
}
FINISH_REFLECT()

GraphInfo FlyMovementSystem::GetGraphBinding()
{
	return GraphInfo(
	{ TYPE_TO_ID(FlyingMovementComponent),TYPE_TO_ID(MeshComponent) },
	{ TYPE_TO_ID(TransformComponent) }
	);
}

void FlyMovementSystem::Init(entt::registry& reg)
{
	RunInEditorMode = true;
}

void FlyMovementSystem::Update(entt::registry& reg)
{
	float delta = GetDeltaTime();
	glm::vec3 posInput = glm::vec3(
		GetInput().GetAxis("cameraFront"),
		GetInput().GetAxis("cameraUp"),
		GetInput().GetAxis("cameraSide")) * glm::vec3(delta);

	const glm::vec2 RotateInput = glm::vec2(GetInput().GetAxis("RotateCameraX"), GetInput().GetAxis("RotateCameraY"))*delta;
	const bool camRightClick = GetInput().GetAction("CameraRightClick");
	const glm::vec2 currentMousePos = GetInput().GetMousePosition();

	float camSpeedChange =GetInput().GetAxis("CameraSpeed") * delta;

	static glm::vec2 mousepos = glm::vec2();
	glm::vec2 offset = glm::vec2(RotateInput.x, -1 * RotateInput.y);

	if (abs(glm::length(offset)) < 0.001f && camRightClick)
	{
		if (m_InvertMouse)
			offset = -(mousepos - currentMousePos);
		else
			offset = (mousepos - currentMousePos);
	}
	mousepos = currentMousePos;


	auto t = reg.view< FlyingMovementComponent, TransformComponent>();

	for (auto&& [entity, Fly, trans] : t.each())
	{
		Fly.m_MovementSpeed += camSpeedChange;

		Fly.m_MovementSpeed = glm::clamp(Fly.m_MovementSpeed,0.1f,15.f);
		//Move
		{
			glm::vec3 addP =
				Transform::GetLocalForwardVector(trans) * (Fly.m_MovementSpeed * posInput.x) +
				Transform::GetLocalUpVector(trans) * (Fly.m_MovementSpeed * posInput.y) +
				Transform::GetLocalRightVector(trans) * (Fly.m_MovementSpeed * posInput.z);
			Transform::AddTranslation(trans, addP);
		}

		//Rotate
		{
			if (abs(glm::length(offset)) < 0.001f) continue;

			Fly.m_Yaw -= offset.x * Fly.m_RotationSpeed * (m_MouseSensitivity / 100.f);
			Fly.m_Pitch -= offset.y * Fly.m_RotationSpeed * (m_MouseSensitivity / 100.f);

			Fly.m_Yaw = fmod(Fly.m_Yaw, glm::radians(360.f));
			Fly.m_Pitch = glm::clamp(Fly.m_Pitch, glm::radians(-90.f), glm::radians(90.f));

			Transform::SetRotation(trans, glm::quat(glm::vec3(Fly.m_Pitch, Fly.m_Yaw, 0.0f)));
		}
	}
}


