#include "EditorPCH.h"
#pragma hdrstop

using namespace Perry::Editor;

IMPLEMENT_REFLECT_OBJECT(IAction)
{

}
FINISH_REFLECT()

ActionManager::ActionManager()
{
	for (auto&& [typeID, metaType] : entt::resolve())
	{
		if (!metaType.is_class())
			continue;

		if (!metaType.prop(T_ReflectedAction))
			continue;

		ActionTypes.emplace_back(metaType);
	}
}
IAction* ActionManager::Create()
{
	return nullptr;
}


void ActionManager::Draw()
{
	if (!Active)
		return;

	ImGui::OpenPopup("ActionList");

	if (ImGui::BeginPopup("ActionList"))
	{
		std::string Search{};

		std::string lll = "";
		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) ImGui::SetKeyboardFocusHere(0);

		ImGui::InputText("SearchName", &Search);

		for (auto& actionType : ActionTypes)
		{
			auto name = actionType.prop(p_DisplayName).value().cast<std::string>();

			auto actionID = actionType.id();

			if (ImGui::Selectable(name.c_str(), false))
			{
				ActivatedActions.emplace_back(actionType.construct());
				auto& metaAction = ActivatedActions[ActivatedActions.size() - 1];
				IAction* action = metaAction.try_cast<IAction>();

				action->Execute();
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}
}