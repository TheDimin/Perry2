#pragma once

namespace Perry
{
	class ToolBase;
	class ToolManager
	{
		friend class EditorManager;
	private:
		ToolManager();

		void DrawImgui();

		std::vector<entt::meta_any> Tools;
	};
} // namespace Perry