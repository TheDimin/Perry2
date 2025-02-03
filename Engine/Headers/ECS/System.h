#pragma once
namespace Perry
{
	class Level;
	class Region;
	class EngineManager;

	struct GraphInfo
	{
		GraphInfo(const std::vector<entt::id_type>&& read, const std::vector<entt::id_type>&& write, bool sync = false) :
			Read(read), Write(write), Sync(sync) {};
		std::vector<entt::id_type> Read{};
		std::vector<entt::id_type> Write{};
		std::vector<entt::id_type> DependsOn{};
		bool Sync = false;
	};

	class System
	{
	public:
		System() = default;
		virtual ~System() = default;
		void Enable() { IsEnabled = true; };
		void Disable() { IsEnabled = false; };
		virtual void Update(entt::registry& reg) {};

		const entt::meta_type& getType()const { return typeInfo; }
	protected:
		friend Level;
		friend Region;
		friend EngineManager;
		friend class Renderer;
		virtual	void Init(entt::registry& reg) { };
		virtual	void ShutDown(entt::registry& reg) {};
		virtual GraphInfo GetGraphBinding() { return GraphInfo({}, {}, true); } //This should not have a default empty function... but easier while we develop this.
		virtual void DrawImGUI() const {};


		float GetDeltaTime() const;
		const Region* GetRegion() const;
		entt::registry& GetRegistry() const { return *Registry; }

		//TODO: implement system finding here attached to current reg.
		template<typename T>
		static T* GetSystem() { return nullptr; }

		//The execution priority of the system. Lower number will execute first.
		int ExecutionPriority = 0;
		//The user has specified that this system is active and should execute.
		bool IsEnabled = true;
		//Should this system run even if the game is paused?
		bool RunInEditorMode = false;
	private:
		entt::meta_type typeInfo;
		entt::registry* Registry = nullptr;

		REFLECT()
	};
}