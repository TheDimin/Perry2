#include "EditorPCH.h"
#pragma hdrstop


void ContextMenu::GenerateComponentContext(const Perry::Entity& owner, entt::meta_any& component)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Remove"))
		{
			component.type().func(f_RemoveComponent).invoke({}, entt::forward_as_meta(owner.GetRegistry()), owner);
		}

		ImGui::EndPopup();
	}

	// Idk if we do anything special here at some point, but we just define right click menu for components here
}

void ContextMenu::GenerateEntityContext(const Perry::Entity& e)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Delete"))
		{
			e.GetRegistry().destroy(e);
		}

		ImGui::EndPopup();
	}
}
