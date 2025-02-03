#include "EditorPCH.h"
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_TOOL(ImguiDemoTool)
{
}
FINISH_REFLECT();

ImguiDemoTool::ImguiDemoTool() : ToolBase("misc")
{
	DrawDefaultWindow = false;
}

void Perry::ImguiDemoTool::Draw()
{
	ImGui::ShowDemoWindow(&IsOpen);
}