#include "EnginePCH.h"
#pragma hdrstop

namespace Perry
{
	IMPLEMENT_REFLECT_COMPONENT(OrthoCameraComponent)
	{

	}
	FINISH_REFLECT();

	IMPLEMENT_REFLECT_COMPONENT(CameraComponent)
	{
		meta.data<&Perry::CameraComponent::m_FOV>("FOV"_hs)
			PROP_DISPLAYNAME("FOV")
			PROP_MINMAX(0.f, 180.f);

		meta.data<&Perry::CameraComponent::m_NearClip>("m_NearClip"_hs)
			PROP_DISPLAYNAME("NearClip");

		meta.data<&Perry::CameraComponent::m_FarClip>("m_FarClip"_hs)
			PROP_DISPLAYNAME("FarClip");
	}
	FINISH_REFLECT();


	IMPLEMENT_REFLECT_SYSTEM(CameraSystem)
	{
	}
	FINISH_REFLECT();

	GraphInfo CameraSystem::GetGraphBinding()
	{
		return GraphInfo({ TYPE_TO_ID(OrthoCameraComponent), TYPE_TO_ID(TransformComponent) },
			{ TYPE_TO_ID(CameraComponent) });
	}
	glm::ivec2 WorldToScreenPoint(const Entity& e, const glm::vec3& worldPoint)
	{
		auto& cam = e.FindComponent<CameraComponent>();
		return WorldToScreenPoint(e, cam, worldPoint);
	}

	glm::ivec2 WorldToScreenPoint(const Entity& e, const CameraComponent& cam, const glm::vec3& worldPoint)
	{
		glm::mat4 pvMatrix = cam.m_PVMatrix;
		glm::vec4 clipSpace = pvMatrix * glm::vec4(worldPoint, 1.0f);

		// Normalize clip space coordinates
		clipSpace /= clipSpace.w;

		// Convert to screen space coordinates
		glm::ivec2 screenSize = GetEngine().GetWindow().GetSize(); // Replace with your screen size retrieval logic
		glm::vec2 screenSpace = glm::vec2((clipSpace.x + 1.0f) * 0.5f, (1.0f - clipSpace.y) * 0.5f);
		screenSpace *= glm::vec2(screenSize);

		return screenSpace;
	}

	glm::vec3 ScreenToWorldPoint(const Entity& e, const CameraComponent& cam, float screenDepth)
	{
		glm::vec2 mouse = Perry::GetInput().GetMousePosition();

		entt::entity eID = e.GetID();

		auto& camTransform = e.GetRegistry().get<TransformComponent>(e);

		// Screen space to NDC (Normalized Device Coordinates)
		float x = (2.0f * mouse.x) / GetEngine().GetWindow().GetWindowWidth() - 1.0f;
		float y = 1.0f - (2.0f * mouse.y) / GetEngine().GetWindow().GetWindowHeight();
		float z = 1.0f; // Assuming a normalized depth range of 0 to 1, where 1 is the far plane.

		// Map the z from screen depth to the normalized depth range if needed.
		// This step may vary depending on how the depth is handled in your engine.
		// For Unity compatibility, z needs to be mapped based on the camera's near and far plane.
		// Here, we assume screenDepth is the distance from the camera in world units.
		// This calculation can be more complex based on your camera setup and requires adjustment.
		z = (screenDepth - cam.m_NearClip) / (cam.m_FarClip - cam.m_NearClip);

		// Convert to clip space coordinates
		glm::vec4 rayClip = glm::vec4(x, y, z, 1.0f);

		// Convert clip space coordinates to eye space
		glm::vec4 rayEye = glm::inverse(cam.m_ProjectionMatrix) * rayClip;
		rayEye.z = -1.0f; // For a perspective projection, we want to look into the scene.
		rayEye.w = 0.0f;

		// Convert eye space to world space
		glm::vec3 rayWorld = glm::vec3(glm::inverse(cam.m_ViewMatrix) * rayEye);
		rayWorld = glm::normalize(rayWorld); // Normalize to ensure it's a direction vector.

		// Calculate the world position based on the ray direction and the desired screen depth.
		// This is a simplified version; the actual calculation should account for the camera position and the specific
		// depth.
		glm::vec3 worldPosition = Transform::GetPosition(camTransform) + rayWorld * screenDepth;

		return worldPosition;
	}

	static Perry::Entity activeCamera = {};
	void Camera::SetCamera(const Entity& e)
	{
		activeCamera = Perry::Entity(&e.GetRegistry(), e.GetID());
	}

	Perry::Entity& Camera::GetCamera()
	{
		if (activeCamera.Valid())
			if (!activeCamera.GetRegistry().try_get<CameraComponent>(activeCamera.GetID()))
			{
				activeCamera = {};
			}

		if (!activeCamera.Valid())
		{
			for (auto& region : World::GetWorld().ActiveRegions())
			{
				bool found = false;
				for (auto e : region->Registry.view<CameraComponent>())
				{
					SetCamera({ &region->Registry, e });
					found = true;
					break;
				}

				if (found)
					break;
			}
		}

		// ASSERT_MSG(LOG_GAMEOBJECTS, activeCamera.Valid(), "Camera doesn't seem to be set..");
		return activeCamera;
	}

	CameraComponent& Camera::GetCameraComponent()
	{
		return activeCamera.FindComponent<CameraComponent>();
	}

	void OnCameraCreated(entt::registry& reg, entt::entity e)
	{
		auto&& [camera, transform] = reg.get<CameraComponent, TransformComponent>(e);
		CameraSystem::UpdateViewProjectionMatrix(Perry::Entity(&reg, e), camera, transform);
	}

	void CameraSystem::Init(entt::registry& reg)
	{
		ExecutionPriority = -100;
		RunInEditorMode = true;
		reg.on_construct<CameraComponent>().connect<&OnCameraCreated>();
	}

	void CameraSystem::Update(entt::registry& reg)
	{
		for (auto&& [ent, camera, transform] : reg.view<CameraComponent, TransformComponent>().each())
		{
			UpdateViewProjectionMatrix(Perry::Entity(&reg, ent), camera, transform);
		}
	}

	void CameraSystem::UpdateViewProjectionMatrix(Perry::Entity e, CameraComponent& camera, const TransformComponent& transform)
	{
		const Window& w = GetEngine().GetWindow();

		if (w.IsMinimized()) return;

		const float width = static_cast<float>(w.GetWindowWidth());
		const float height = static_cast<float>(w.GetWindowHeight());
		const float aspectRatio = width / height;

		if (e.GetRegistry().all_of<OrthoCameraComponent>(e))
		{
			const float halfWidth = camera.m_FOV;
			const float halfHeight = camera.m_FOV / aspectRatio;

			camera.m_ProjectionMatrix = glm::ortho(
				-halfWidth, halfWidth,
				-halfHeight, halfHeight,
				-camera.m_FarClip, camera.m_FarClip);
		}
		else
		{
			camera.m_ProjectionMatrix = ::glm::perspective(::glm::radians(camera.m_FOV),
				aspectRatio, camera.m_NearClip, camera.m_FarClip);
		}

		const glm::vec3 CameraWorldPos = Transform::GetPosition(transform);

		camera.m_ViewMatrix = glm::lookAt(CameraWorldPos, CameraWorldPos + Transform::GetForwardVector(transform), glm::vec3(0, 1, 0));

		camera.m_PVMatrix = glm::mat4(1.0f);
		camera.m_PVMatrix = camera.m_ProjectionMatrix * camera.m_ViewMatrix;
	}

}