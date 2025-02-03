#pragma once
#ifdef ENGINE_DEPRECATED

namespace Perry
{
	struct Entity;

	class Level
	{
	public:
		Level() = default;
		Level(const Level& l) = delete;
		Level(Level&& l) = default;
		virtual ~Level();

		//[[deprecated("Will move to using 'Prefabs', user should place objects in world, which will delegate it to chunks.")]]
		Entity AddEntity(const glm::vec3& position = glm::vec3(0.f), const glm::vec3& rotation = glm::vec3(0.f), const glm::vec3& scale = glm::vec3(1.f));

		void RemoveEntity(Entity& e);

		inline bool IsLevelSaved() const { return m_IsLevelSaved; }
		inline const std::string& GetLevelFile() { return m_LevelFile; }
		inline const std::string& GetLevelFolder() { return m_LevelFolder; }

		entt::registry& GetRegistry();

		template <class SYSTEM>
		SYSTEM* GetSystem();

	protected:
		friend class EngineManager;
		friend class EditorManager;

		void OnLoadInternal();
		entt::registry registry{};

		virtual void OnLoad();
		void UpdateSystems(float deltaTime);
		virtual void Update(float deltaTime) {};
		virtual void OnUnload() {};

		REFLECT()

	private:
		template <class System>
		System& RegisterSystem();

		std::vector<entt::meta_any> m_SystemInstances = std::vector<entt::meta_any>();

		std::string m_LevelFolder{};
		std::string m_LevelFile{};
		bool m_IsLevelSaved = false;
	};

	template <typename System>
	System& Level::RegisterSystem()
	{
		m_SystemInstances.emplace_back(std::make_unique<System>());

		return *m_SystemInstances.back();
	}

	template <typename SYSTEM>
	SYSTEM* Level::GetSystem()
	{
		entt::meta_type systemType = entt::resolve<SYSTEM>();
		for (auto& systemInstance : m_SystemInstances)
		{
			if (systemInstance.type().id() == systemType.id())
				return systemInstance.try_cast<SYSTEM>();
		}
		return nullptr;
	}

}  // namespace Perry

#endif