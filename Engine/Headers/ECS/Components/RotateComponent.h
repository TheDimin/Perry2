#pragma once

namespace Perry
{
	struct RotateComponent
	{
		RotateComponent();
		RotateComponent(float speed, const glm::vec3& dir);

		glm::vec3 m_Axis;
		float m_Speed = 0.f;
		REFLECT()
	};

	class RotateSystem : public System
	{
	public:
		void Update(entt::registry& reg) override;
		REFLECT()
	};
}