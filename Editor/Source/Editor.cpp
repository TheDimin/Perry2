#include "EditorPCH.h"
#pragma hdrstop

class RtsLevel;
using namespace Perry;

//#define ShowInternal

EditorManager::EditorManager() : ToolManager()
{
	Init();
}

void EditorManager::DrawImgui()
{
	ToolManager.DrawImgui();
	ActionManager.Draw();

	if (GetSelectedEntity().Valid())
	{
		Transform::SetRegistery(&SelectedEntity.GetRegistry());
		DrawGizmo();
	}
	else
	{
		SetSelectedEntity({ nullptr,entt::null });
	}

	//DrawMenuBar();

		ImGui::Begin("Statistics");
		ImGui::Text("DeltaTime:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.21f, 1.0f, 0.42f, 1.0f), "%.3g", GetEngine().m_DeltaTime*100);
		ImGui::End();


/*

	if (s.showSystems)
		DrawSystems();

	if (s.showWarnings)
		DrawWarnings();

	if (s.showTextureDebug)
		DrawTextureDebug();

	if (s.audio.show)
		DrawAudioSettings();
	if (s.showEngineSpeed)
		DrawSpeedSettings();

	resourceViewer.Draw();*/
}

void EditorManager::DrawGizmo()
{
	if (SelectedEntity.Valid())
	{
		auto camEntity = Camera::GetCamera();

		ImGuizmo::Enable(true);

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
		TransformComponent& transform = SelectedEntity.FindComponent<TransformComponent>();
		glm::mat4 data = Perry::Transform::GetMatrix(transform);

		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

		ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always, ImVec2(1.f, 0));
		ImGui::SetNextWindowSize(ImVec2(210, 60));

		ImGui::Begin("##ImGuizmo", NULL, ImGuiWindowFlags_NoDecoration);
		{
			ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
			if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
				mCurrentGizmoOperation = ImGuizmo::SCALE;

			if (mCurrentGizmoOperation != ImGuizmo::SCALE)
			{
				if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
					mCurrentGizmoMode = ImGuizmo::LOCAL;
				ImGui::SameLine();
				if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
					mCurrentGizmoMode = ImGuizmo::WORLD;
			}

			ImGuizmo::DrawGrid(&camEntity.FindComponent<CameraComponent>().m_ViewMatrix[0][0], &camEntity.FindComponent<CameraComponent>().m_ProjectionMatrix[0][0], &glm::identity<glm::mat4>()[0][0], 10);

		//	ImGuizmo::Manipulate(glm::value_ptr(camComponent.m_ViewMatrix), glm::value_ptr(camComponent.m_ProjectionMatrix),
			//	mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(data));


			if (ImGuizmo::IsUsing())
			{
				switch (mCurrentGizmoOperation) {
				case ImGuizmo::SCALE:
					Transform::SetScale(transform, Transform::GetScale(data));
					break;
				case ImGuizmo::ROTATE:
					Transform::SetRotation(transform, Transform::GetRotation(data));
					break;
				case ImGuizmo::TRANSLATE:
					Transform::SetPosition(transform, Transform::GetPosition(data));
					break;
				default:
					break;
				}
			}
		}
		ImGui::End();
	}
}
void EditorManager::DrawSystems()
{
	/*
	[[maybe_unused]] auto& window = GetEngine().GetWindow();
	ImGui::SetNextWindowPos(ImVec2(0, 20.f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_Always);
	if (ImGui::Begin("Systems", &GetEngine().GetImguiSettings().showSystems, ImGuiWindowFlags_NoScrollbar))
	{
		static std::string nameFilter = "";
		static ImVec2 systemList{ -1,150 };
		static ImVec2 systemVariableList{ 0,500 };

		ImGui::InputText("SearchName", &nameFilter);

		ImGui::PushItemWidth(-1);
		ImGui::BeginListBox("##Empty", ImVec2(0, systemList.y - 5));

		for (auto& system : GetEngine().GetLevel().m_SystemInstances)
		{
			std::string label = system.type().prop(p_DisplayName).value().cast<std::string>(); //+ std::to_string(reinterpret_cast<unsigned int>(system.get()));

			if (!containsIgnoreCase(label, nameFilter))
				continue;

			if (ImGui::Selectable(label.data(), (system.data()) == selectedSystem.system))
			{
				selectedSystem.system = system.data();
				selectedSystem.type = system.type();
			}
		}

		ImGui::EndListBox();


		ImVec2 max = AddImVec(AddImVec(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowWidth(), 5)), ImGui::GetCursorPos());
		ImGui::SplitterBehavior(ImRect(AddImVec(ImGui::GetCursorPos(), ImGui::GetWindowPos()), max),
			ImGui::GetID("list"),
			ImGuiAxis_Y,
			&systemList.y, &systemVariableList.y,
			50, 50, 5,
			1);
		ImGui::Dummy(ImVec2(0, 5));

		if (ImGui::BeginChild("systemVariableList", systemVariableList))
		{
			if (selectedSystem.system != nullptr)
			{
				entt::meta_any systemInstance = selectedSystem.type.from_void(selectedSystem.system);

				Inspector::InspectClass(systemInstance, 0, false);
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
		*/
}

void EditorManager::DrawAudioSettings()
{
	/*
	  auto buses = GetAudio().GetAllBuses();

	  if (buses.empty())
		  return;

	  auto& audioSettings = GetEngine().GetImguiSettings().audioSettings;
	  auto& audioShow = GetEngine().GetImguiSettings().audio;
	  ImGui::Begin("Audio Settings", &audioShow.show);

	  /*
		if (ImGui::Button("Save"))
		{
			Serializer::SaveAudio("Config/audio_settings.json");
		}
		*/
		/*for (auto bus : buses)
		{
			auto busName = GetAudio().GetBusPath(bus);
			if (ImGui::CollapsingHeader(busName.c_str()))
			{
				if (ImGui::Checkbox(("Mute " + busName).c_str(), &audioSettings[busName].first))
					GetAudio().ToggleMute(bus, audioSettings[busName].first);

				if (ImGui::SliderFloat(("Volume " + busName).c_str(), &audioSettings[busName].second, 0.0f, 1.0f, "%.3f"))
					GetAudio().SetVolume(bus, audioSettings[busName].second);
			}
		}

		ImGui::End();
	  */
}

void EditorManager::SetImguiTheme()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	//style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	//style.Colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	style.GrabRounding = style.FrameRounding = 2.3f;
	style.GrabRounding = style.FrameRounding = 2.3f;
}

void EditorManager::Init()
{
	SetImguiTheme();
}

EditorManager& Perry::GetEditor()
{
	static EditorManager* editor = new EditorManager(); // This line creates the Engine
	return *editor;
}