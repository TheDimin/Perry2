#pragma once

namespace Perry
{
	class TransformComponent;
	class ChunkInfoComponent
	{
	public:
		ChunkInfoComponent() = default;

		glm::ivec2 ChunkId;
		REFLECT()
	};

	/// <summary>
	/// When you want todo quaries in the world but want to reduce the amount of objects to loop over.
	///	You ideally interact with the ChunkInfoGenerator. Long term you should be able to query chunks quickly on
	///specific objects.
	/// </summary>
	class ChunkInfoGenerator : public Perry::System
	{
	public:
		ChunkInfoGenerator() : System() {}
		[[nodiscard]] const std::set<entt::entity>& GetChunk(const Perry::TransformComponent& transform) const;
		std::vector<const std::set<entt::entity>*> GetNeighbourHood(const Perry::TransformComponent& transform,
															  float maxDistance) const;

		void AddToChunk(entt::entity, const Perry::TransformComponent& transform);
		void RemoveFromChunk(entt::entity, const Perry::TransformComponent& transform);
		ChunkInfoGenerator(const ChunkInfoGenerator& other) :
			Perry::System(other), ChunkInfoObserver(), ChunkLookup(other.ChunkLookup)
		{
		}
		ChunkInfoGenerator(ChunkInfoGenerator&& other) noexcept :
			Perry::System(std::move(other)), ChunkInfoObserver(), ChunkLookup(std::move(other.ChunkLookup))
		{
		}
		ChunkInfoGenerator& operator=(const ChunkInfoGenerator& other)
		{
			if (this == &other)
				return *this;
			Perry::System::operator=(other);
			ChunkLookup = other.ChunkLookup;
			return *this;
		}
		ChunkInfoGenerator& operator=(ChunkInfoGenerator&& other) noexcept
		{
			if (this == &other)
				return *this;
			Perry::System::operator=(std::move(other));
			ChunkLookup = std::move(other.ChunkLookup);
			return *this;
		}

		static constexpr float ChunkSize = 2.5f;

	protected:
		void Update(entt::registry& reg) override;
		void Init(entt::registry& reg) override;
		void ShutDown(entt::registry& reg) override;

		void OnChunkInfoCreated(entt::registry& reg, entt::entity e);
		void OnChunkInfoDestroyed(entt::registry& reg, entt::entity e);

		GraphInfo GetGraphBinding() override;

		entt::observer ChunkInfoObserver{};
		// For now we store all the entities in their chunks, long term we have to come with a different mechanism
		std::unordered_map<glm::ivec2, std::set<entt::entity>> ChunkLookup;

		REFLECT()
	};
} // namespace Perry