#pragma once

namespace Perry
{
	class System;
}
namespace Perry
{
	/// <summary>
	/// Our world is divided into regions, These can update and be loaded independed of each other.
	///	Coordinates are always local to a region to allow bigger worlds.
	///	As such each chunk has it own registry, and own systems. and can be updated at different intervals
	/// </summary>
	class Region final
	{
	public:
		Region();
		//Temp constructor
		Region(const glm::ivec2& location, const glm::ivec2& size, const std::string& name);
		Region(const Region& l) = delete;
		Region& operator=(const Region& other) = delete;
		Region& operator=(Region&& other) noexcept = delete;
		Region(Region&& other) noexcept;
		virtual ~Region();

		entt::registry& GetRegistry();

		template<class SYSTEM>
		SYSTEM* GetSystem();

		friend class EngineManager;
		friend class EditorManager;
		friend class World;

		// TODO right now every region builds its own execution graph, this can be created globablly executed locally
		void OnLoadInternal();
		entt::registry Registry;

		entt::adjacency_matrix<entt::directed_tag> JobGraph;

		std::vector<std::unique_ptr<System>> SystemInstances = std::vector<std::unique_ptr<System>>();

		const float& GetRegionDeltaTime();

		void Save(const std::string& worldName, nlohmann::json& jobject);
		void Load(const std::string& worldName, const nlohmann::json& jobject);

		//Bounds including overlap
		glm::vec3 GetRegionMaxBounds() const;
		//Bounds of what this region is in charge of
		glm::vec3 GetRegionAuthorityBounds() const;

		glm::ivec2 Location;
		glm::ivec2 Size;
		float overlapExtend = 50;
		std::string Name;
		bool IsLoaded = true;
	private:
		void WorldUpdate(float GlobalDeltaTime);
		void UpdateSystems();
		void BeginExecute(unsigned long long vertex);

		std::chrono::time_point<std::chrono::system_clock> LastUpdateTime;
		float RegionDeltaTime = 0;
		typedef std::function<void(entt::registry&)> Event;
		Concurrency::concurrent_queue<Event> Events;
		//REFLECT()
	};

	template<typename SYSTEM>
	SYSTEM* Region::GetSystem()
	{
		for (auto& systemInstance : SystemInstances)
		{
			if (dynamic_cast<SYSTEM*>(systemInstance.get()) != nullptr)
			{
				return dynamic_cast<SYSTEM*>(systemInstance.get());
			}
		}

		//ASSERT_MSG(LOG_GENERIC, false, "Failed to find system !");

		return nullptr;
	}
} // namespace Perry