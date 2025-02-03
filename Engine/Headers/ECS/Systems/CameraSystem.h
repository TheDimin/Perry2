#pragma once

namespace Perry
{
	class CameraSystem : public System
	{
	public:
		CameraSystem() : System() {};
		void Init(entt::registry& reg) override;
		void Update(entt::registry& reg) override;

		static void UpdateViewProjectionMatrix(Perry::Entity e, CameraComponent& camera, const TransformComponent& transform);
		REFLECT()

	protected:
		GraphInfo GetGraphBinding() override;
	};
}