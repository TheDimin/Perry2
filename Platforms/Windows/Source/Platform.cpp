#include "pch.h"
#include <stdlib.h>




PROCESS_INFORMATION SteamProccess = { 0 };

void Platform::Init()
{
#ifdef NO_PLATFORM
	SteamErrMsg msg{};

	auto clientID = Perry::GetLaunchParameters().GetString("CLIENT_ID", "");
	if (!clientID.empty())
	{
		std::string envP = std::format("steam_master_ipc_name_override={}", clientID.data());
		if (_putenv(envP.data()) != 0) {

			ERROR(LOG_PLATFORM, "Failed to set steam_master_ipc_name_override!");
			return;
		}
	}


	auto result = SteamAPI_InitEx(&msg);
#define DEV
#ifdef DEV
	if (result == k_ESteamAPIInitResult_NoSteamClient)
	{
		// We allow launching multiple steam clients, but if it not enabled yet, do that now.

		const char* steamPath = "C:\\Program Files (x86)\\Steam\\steam.exe";


		// Prepare process startup information
		STARTUPINFO si = { 0 };
		si.cb = sizeof(STARTUPINFO);

		char commandLine[1024];
		snprintf(commandLine, sizeof(commandLine), "\"%s\" -master_ipc_name_override %s -console -login %s %s", steamPath, clientID.data(),
			Perry::GetLaunchParameters().GetString("STEAM_USERNAME", "").data(),
			Perry::GetLaunchParameters().GetString("STEAM_PASSWORD", "").data()
		);


		// Launch Steam
		if (CreateProcess(
			nullptr,              // Application path
			commandLine,                // Command line arguments
			nullptr,                // Process security attributes
			nullptr,                // Thread security attributes
			FALSE,                  // Inherit handles
			0,                      // Creation flags
			nullptr,                // Environment (use parent's)
			nullptr,                // Current directory
			&si,                    // Startup information
			&SteamProccess          // Process information
		)) {
			
			WARN(LOG_PLATFORM, "Launching new instance of steam waiting 2 seconds ");
			Sleep(2000);
		}
		else {
			ERROR(LOG_PLATFORM, "Failed to launch Steam. Error: %d", GetLastError());
		}

		
		// Close process and thread handles
		CloseHandle(SteamProccess.hProcess);
		CloseHandle(SteamProccess.hThread);

		//Yeah we just call init again, but after it ran exit this function ! (Be aware this can cause infinite loop)
		Init();
		return;
	}
#endif

	if (result != k_ESteamAPIInitResult_OK)
	{
		ERROR(LOG_PLATFORM, "%s", msg);
		return;
	}

	localUser = new User();
#endif
}

Platform::~Platform()
{
#ifdef NO_PLATFORM
	SteamAPI_Shutdown();

	delete localUser;
#endif
}

void Platform::Update(float delta)
{
#ifdef NO_PLATFORM
	SteamAPI_RunCallbacks();
#endif
}