#include "UnitTestsGame.h"

#include "Engine.h"
#include "AssetManager.h"
#include "UnitTestsLevel.h"
#include "imgui.h"

#include "Audio/AudioExampleLevel.h"
#include "LevelLoading/LevelLoadingExampleLevel.h"
#include "PathFinding/PathFindingDemoLevel.h"

#include "catch.hpp"

void UnitTestsGame::Init()
{ 
    Perry::GetEngine().LoadLevel<UnitTestsLevel>();

	const char* outArg[]{ "UnitTests.exe", "--reporter", "JUnit::out=result-junit.xml", "--reporter", "console::out=-::colour-mode=ansi" };

	auto s = Catch::Session();
	s.run(5, outArg);

	// TODO: Create Listener and display tests with ImGUI

}

void UnitTestsGame::Update(float DeltaTime) {
  ImGui::Begin("Levels");
  // HardCoded levels to show
  if (ImGui::Button("MyLevel")) 
	  Perry::GetEngine().LoadLevel<UnitTestsLevel>();

  if (ImGui::Button("AudioExample"))
      Perry::GetEngine().LoadLevel<Perry::AudioExampleLevel>();

  if (ImGui::Button("LevelLoadingExample"))
      Perry::GetEngine().LoadLevel<Perry::LevelLoadingExampleLevel>();

  if (ImGui::Button("NavigationExample"))
      Perry::GetEngine().LoadLevel<Perry::PathFindingDemoLevel>();

  ImGui::End();
}

void UnitTestsGame::ShutDown() {}