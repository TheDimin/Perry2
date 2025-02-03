#include "EnginePCH.h"
#pragma hdrstop

#include <windows.h>

#include "../../Editor/Headers/ActionManager.h"
#include "../../Editor/Headers/Tools/ToolManager.h"
#include "../../Editor/Headers/Tools/ResouceViewer.h"
#include "../../Editor/Headers/Editor.h"
namespace Perry
{
	PROCESS_INFORMATION pi;

	IMPLEMENT_REFLECT_OBJECT(EngineManager)
	{
		meta PROP_SCRIPTCALLABLE PROP_DISPLAYNAME("Engine");

		meta.func<&EngineManager::GetDeltaTime>("F_Delta"_hs) PROP_SCRIPTCALLABLE PROP_DISPLAYNAME("DeltaTime");
	}
	FINISH_REFLECT();

	/*Networking::Networking* EngineManager::GetNetwork() const
	{
		return Network.get();
	}*/

	GameBase& EngineManager::GetGame() const
	{
		return *m_GameInstance;
	}

	Renderer& EngineManager::GetRenderer() const
	{
		return *m_Renderer;
	}

	Window& EngineManager::GetWindow() const
	{
		return *m_Window;
	}

	AudioSystem& EngineManager::GetAudio() const
	{
		return *m_AudioSystem;
	}

	void EngineManager::Quit()
	{
		m_RequestClose = true;
	}

	const float& EngineManager::GetDeltaTime() const
	{
		return m_DeltaTime;
	}


	Input& EngineManager::GetInput() const
	{
		return *m_Input;
	}

	EngineManager::EngineManager() : m_AudioSystem(new AudioSystem)
	{
	}

	void EngineManager::DrawImgui()
	{
#ifndef NO_IMGUI
		GetEditor().DrawImgui();
		m_Renderer->ImguiDraw();
#endif
	}

	void LaunchCurrentExe(const std::vector<std::string>& launchOptions, PROCESS_INFORMATION& pi) {
		// Get the path to the current executable
		char exePath[MAX_PATH];
		if (GetModuleFileName(NULL, exePath, MAX_PATH) == 0) {
			std::cerr << "Failed to get the current executable path." << std::endl;
			return;
		}

		// Construct the command line arguments
		std::string commandLine = "\""; // Start with a quote for the path
		commandLine += exePath;
		commandLine += "\""; // End the quote for the path

		// Append launch options to the command
		for (const auto& option : launchOptions) {
			commandLine += " ";
			commandLine += option;
		}

		// Prepare to launch the process
		STARTUPINFO si = { sizeof(STARTUPINFO) };

		// Create the new process
		if (!CreateProcess(
			NULL,                      // Application name (NULL since it's included in the command line)
			&commandLine[0],           // Command line
			NULL,                      // Process handle not inheritable
			NULL,                      // Thread handle not inheritable
			FALSE,                     // Set handle inheritance to FALSE
			0,                         // No creation flags
			NULL,                      // Use parent's environment block
			NULL,                      // Use parent's starting directory
			&si,                       // Pointer to STARTUPINFO structure
			&pi                        // Pointer to PROCESS_INFORMATION structure
		)) {
			std::cerr << "Failed to launch the current executable. Error: " << GetLastError() << std::endl;
			return;
		}

		std::cout << "Current executable launched successfully." << std::endl;
	}


	void EngineManager::Init()
	{
		PROFILER_MARK("EngineInit")
			Fiber fiber = Perry::ThreadToFiber(nullptr);

		FileIO::Init();

		//::MetaInit();

		//Network = std::make_unique<Networking::Networking>();
		m_Window = std::make_unique<Window>();

		m_Renderer = std::make_unique<Renderer>(m_Window.get());

		m_Input = std::unique_ptr<Input>(new Input()); // Private constructor thats why we do it this way
		m_AudioSystem = std::make_unique<AudioSystem>();
		Platform = std::make_unique<class Platform>();

		m_Renderer->PlatformInit();
		m_Input->Init();
		static_cast<ISubSystem*>(m_AudioSystem.get())->Init();

		Platform->Init();

		m_GameInstance->Init();

		if (Perry::GetLaunchParameters().Contains("LAUNCH_CLIENTS"))
		{
			for (int i = 0; i < 1; ++i)
			{
				std::vector<std::string> launchOptions{};

				for (auto& launchOption : GetLaunchParameters().View())
				{
					if (launchOption.first == "LAUNCH_CLIENTS" || launchOption.first == "Host")
						continue;

					launchOptions.push_back("-" + launchOption.first + "=" + launchOption.second);
				}

				launchOptions.push_back("CLIENT_ID=PerryClient" + std::to_string(i));
				launchOptions.push_back("STEAM_USERNAME=PerryDev1");
				launchOptions.push_back("STEAM_PASSWORD=" + Perry::GetLaunchParameters().GetString("STEAM_PASSWORD", ""));

				//TODO auto connect to server
				//The fact we save the steam password in the launch parameters is a security risk, but its a dummy account anyway
				//TODO, can we use steam anynoumous api for it
				LaunchCurrentExe(launchOptions, pi);
			}
		}
	}

	EngineManager& GetEngine()
	{
		static EngineManager* engine = new EngineManager(); // This line creates the Engine
		return *engine;
	}

	void EngineManager::Run()
	{
		assert(m_GameInstance);
		Init();

		bool destroy = false;

		using clock = std::chrono::system_clock;
		static auto t0 = ::std::chrono::time_point_cast<::std::chrono::milliseconds>((clock::now())).time_since_epoch();

		while (!destroy && !m_RequestClose)
		{
			PROFILER_MARK("EngineFrame")
				// printf("update called \n");
				m_Renderer->StartFrame();
			m_Input->Update();

			auto t1 = ::std::chrono::time_point_cast<::std::chrono::milliseconds>((clock::now())).time_since_epoch();
			m_DeltaTime = (t1 - t0).count() * .001f;
			t0 = t1;

			destroy = m_Renderer->Update(m_DeltaTime);

			m_GameInstance->Update(m_DeltaTime);
			Platform->Update(m_DeltaTime);

			static_cast<ISubSystem*>(m_AudioSystem.get())->Update(m_DeltaTime);
			DrawImgui();

			m_Renderer->Render();
			ImHexMemTracker::Dump();
		}

		m_GameInstance->ShutDown();

		TerminateProcess(pi.hProcess, 0);

		// Close process and thread handles to avoid resource leaks
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

	}
} // namespace Perry
