#pragma once

namespace Perry
{
	namespace Editor
	{
		class ActionManager;
	}

	// Class That is used for Editor/Debugging
	// It should inject itself into the Engine in debug builds
	class EditorManager
	{
	public:
		EditorManager();

		const Perry::Entity& GetSelectedEntity() const { return SelectedEntity; }
		bool HasEntitySelected() const { return SelectedEntity.Valid(); }
		void SetSelectedEntity(Perry::Entity e) { SelectedEntity = e; }

		entt::registry* SelectedRegistry = nullptr;
	private:
		Perry::Entity SelectedEntity;

	private:
		friend class EngineManager;

		void DrawImgui();
		void DrawGizmo();
		void DrawSystems();
		void DrawAudioSettings();

		void SetImguiTheme();

		void Init();

		struct SelectedSystemInfo
		{
			void* system = nullptr;
			entt::meta_type type{};
		};
		SelectedSystemInfo selectedSystem;

	private:
		ResourceViewer resourceViewer;
		ToolManager ToolManager;
		Editor::ActionManager ActionManager{};
	};

#ifndef NO_IMGUI
	extern EditorManager& GetEditor();
#endif

} // namespace Perry