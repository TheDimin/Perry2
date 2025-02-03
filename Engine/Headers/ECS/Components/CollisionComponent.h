#pragma once

enum CollisionLayer : unsigned short;
namespace Perry
{
	class CollisionComponent
	{
		static constexpr auto in_place_delete = true;
	public:
		CollisionComponent();
		CollisionComponent(const glm::vec2& size, CollisionLayer l);
		CollisionComponent(const glm::vec2& size, const std::string& tag = "", CollisionLayer l =  CollisionLayer(1));

		entt::sink<entt::sigh<void(CollisionComponent&, CollisionComponent&)>>  m_OnCollide{ m_OnCollideSignal };
		entt::sink<entt::sigh<void(CollisionComponent&, CollisionComponent&)>>  m_OnCollideStart{ m_OnCollideStartSignal };
		entt::sink<entt::sigh<void(CollisionComponent&, CollisionComponent&)>>  m_OnCollideEnd{ m_OnCollideEndSignal };

		std::string GetTag() const;
		CollisionLayer GetLayer()const;
		glm::vec2 GetSize() const;

		CollisionLayer m_CollisionLayer;
		std::string m_Tag;

		bool m_AutoResize = true;

		bool m_IsStatic = false;

		bool m_TriggerBox = false;

		glm::vec2 m_Offset = glm::vec2(0, 0);
		glm::vec2 m_Size = glm::vec2(0, 0);

	private:
		friend class CollisionWorld;

		//Read only data copied from transform
		glm::vec2 centerPoint{};

		entt::sigh<void(CollisionComponent&, CollisionComponent&)> m_OnCollideStartSignal{};
		entt::sigh<void(CollisionComponent&, CollisionComponent&)> m_OnCollideSignal{};
		entt::sigh<void(CollisionComponent&, CollisionComponent&)> m_OnCollideEndSignal{};

		REFLECT()
	};
}