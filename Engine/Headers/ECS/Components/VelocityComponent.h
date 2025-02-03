#pragma once

namespace Perry
{
	struct VelocityComponent
	{
		VelocityComponent(float Friction = 1.f, const glm::vec3& vel = glm::vec3()) :
			m_Friction(Friction), m_Velocity(vel)
		{
		}

		float m_Friction;

		glm::vec3 m_Velocity = glm::vec3(0.f);

		REFLECT()
	};

	class ApplyVelocitySystem : public Perry::System
	{
	public:
		bool drawVelocity = true;

		ApplyVelocitySystem();
		void Update(entt::registry& reg) override;

	protected:
		GraphInfo GetGraphBinding() override;

	private:
		REFLECT()
	};
}
