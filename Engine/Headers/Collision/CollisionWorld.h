#pragma once

//TODO replace this with jolt
#ifdef ENGINE_DEPRECATED
namespace Perry
{
	static const glm::vec2 AABB_NORMALS[] = {
		glm::vec2(-1.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f), glm::vec2(0.0f, 1.0f)
	};

	using CollisionPair = std::pair<CollisionComponent*, CollisionComponent*>;

	struct Collision {
		CollisionPair pair;

		float overlap = 0.0f;
		glm::vec2 normal = glm::vec3(0.0f);
	};

	class CollisionWorld : public System
	{
	public:
		CollisionWorld() : System() { ExecutionPriority = 0; }

		struct Cell
		{
			//Storing the collision pairs inside the Cell, allows us to multithread based on each cell
			std::vector<CollisionPair> pairs{};
			std::vector<CollisionComponent*> ids{};
		};
	protected:
		void Init(entt::registry& reg) override;
	public:
		void Update(entt::registry& reg) override;
		GraphInfo GetGraphBinding() override;
		friend class CollisionComponent;
		friend class Level;

	private:
		bool AABB(const CollisionComponent& a, const CollisionComponent& b, glm::vec2& out_normal, float& out_overlap) const;  // aabb function

		Cell& GetCell(const glm::vec3&);
		std::unordered_map<glm::ivec2, Cell> grid{};
		static const ::std::unordered_map<CollisionLayer, CollisionLayer> CollisionMatrix;

		std::set<CollisionPair> m_CollisionPairsLastFrame;

		std::vector<Collision> m_Collisions;

		//TODO (Damian) Any value other then 1 does not work as intended !
		float CellSize = 5;

		float m_Skin = 0.01f;

		REFLECT()
	};
}  // namespace Perry

#endif
