#include "EditorPCH.h"
#pragma hdrstop

Perry::ToolManager::ToolManager()
{
	for (auto&& [id, metaType] : entt::resolve())
	{
		if (!metaType.is_class())
			continue;

		if (!metaType.prop(Reflect::T_ReflectedTool))
			continue;

		auto tool = metaType.construct();
		ToolBase* toolInstance = tool.try_cast<ToolBase>();
		ASSERT_MSG(LOG_REGION, toolInstance, "ATTEMPTED to initialize a invalid tool!");

		toolInstance->Init();

		toolInstance->ToolName = metaType.prop(p_DisplayName).value().try_cast<std::string>()->c_str();


		INFO(LOG_TOOL, "FOUND reflected tool: %s", toolInstance->ToolName);

		Tools.emplace_back(tool);
	}
}

void Perry::ToolManager::DrawImgui()
{
	for (auto& metaTool : Tools)
	{
		ToolBase* tool = metaTool.try_cast<ToolBase>();
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu(tool->Category))
			{
				tool->IsOpen = ImGui::MenuItem(tool->ToolName);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (tool->IsOpen)
		{
			if(tool->DrawDefaultWindow)
			{
				if (ImGui::Begin(tool->ToolName, &tool->IsOpen))
				{
					tool->Draw();
					ImGui::End();
				}
			}
			else
			{
				tool->Draw();
			}
		}
	}
}