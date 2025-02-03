#pragma once

namespace Perry
{
	class CameraComponent;
	class TransformComponent;

	class CameraComponent
	{
	public:
		CameraComponent() = default;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_PVMatrix;

		float m_FOV = 80.0f;

		float m_NearClip = 0.1f;
		float m_FarClip = 10000.0f;

		REFLECT()
	};

	struct OrthoCameraComponent
	{
		REFLECT()
	};



	glm::ivec2 WorldToScreenPoint(const struct Entity& e, const glm::vec3& worldPoint);
	glm::ivec2 WorldToScreenPoint(const struct Entity& e, const CameraComponent& cam, const glm::vec3& worldPoint);
	glm::vec3 ScreenToWorldPoint(const struct Entity& e, const CameraComponent& cam, float screenDepth = 1);

	class Camera
	{
	public:
		static void SetCamera(const Entity& e);

		static Perry::Entity& GetCamera();
		static CameraComponent& GetCameraComponent();
	};
}