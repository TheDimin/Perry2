#pragma once

namespace Perry
{
	class AudioSystem;
	class Renderer;
	class GameBase;
	class Window;
	class Input;
	namespace Networking
	{
		class Networking;
	}

	class EngineManager
	{
	public:
		Networking::Networking* GetNetwork() const;
		GameBase& GetGame() const;
		Renderer& GetRenderer() const;
		Window& GetWindow() const;
		Input& GetInput() const;
		AudioSystem& GetAudio() const;

		template <typename T>
		T* GetSubSystem()
		{
			for (auto& sub : SubSystems)
			{
				if (auto cast = dynamic_cast<T*>(sub))
				{
					return cast;
				}
			}
			ERROR("SUB_SYSTEM", "Failed to find subsystem");
			return nullptr;
		}

		void Quit();
		const float& GetDeltaTime() const;

		// Init Functions
		template<typename Game>
		void SetGame();

		void Run();
	private:
		friend class EditorManager;
		friend EngineManager& GetEngine();
		EngineManager();

		void DrawImgui();
		void Init();

		//std::unique_ptr<Networking::Networking> Network;
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Renderer> m_Renderer;
		std::unique_ptr<GameBase> m_GameInstance;
		std::unique_ptr<AudioSystem> m_AudioSystem;
		std::unique_ptr<Input> m_Input;
		std::unique_ptr<Platform> Platform;
		std::vector<Perry::ISubSystem*> SubSystems;


		float m_DeltaTime = 0;
		bool m_RequestClose = false;

		REFLECT()
	};




	template<typename Game>
	inline void EngineManager::SetGame()
	{
		m_GameInstance = ::std::make_unique<Game>();
	}

	// extern EngineManager& Engine;
	EngineManager& GetEngine();

	inline Input& GetInput()
	{
		return GetEngine().GetInput();
	}
	inline AudioSystem& GetAudio()
	{
		return GetEngine().GetAudio();
	}
} // namespace Perry