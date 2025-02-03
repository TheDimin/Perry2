#pragma once

namespace Perry
{
	class FollowComponent
	{
	public:
		FollowComponent();
		FollowComponent(entt::entity FollowingEntity, glm::vec3 followOffset = glm::vec3(0.f, 3.f, 4.f));

		entt::entity following;

		float m_Smoothing = 0.5f;
		glm::vec3 m_PosOffset = glm::vec3(0.0f, 0.0f, 0.0f);

	private:
		friend class FollowSystem;

		glm::vec3 m_SmoothPosition{};

		REFLECT()
	};

	class FollowSystem : public Perry::System
	{
	public:
		void Update(entt::registry& reg) override;

	protected:
		GraphInfo GetGraphBinding() override;

	public:
		REFLECT()
	};
}
