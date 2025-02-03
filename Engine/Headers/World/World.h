#pragma once

namespace Perry
{
	class Region;
	class World final
	{
	public:
		World();

		void InitSystems();

		void Update(float deltaTime);

		std::vector<Region*>& ActiveRegions();

		Region* CreateRegion(const glm::ivec2& location, const glm::ivec2& size,const std::string& name);

		void Save(const std::string& path);
		void Load(const std::string& path);
		// Global API
	public:
		static World* CreateWorld();
		static World& GetWorld();
		static void SetWorld(World* newWorld)
		{
			delete ActiveWorld;

			ActiveWorld = newWorld;
		}
		
		std::vector<Region*> Regions{};
	private:
		
		static inline World* ActiveWorld = nullptr;

		//REFLECT();
	};
} // namespace Perry