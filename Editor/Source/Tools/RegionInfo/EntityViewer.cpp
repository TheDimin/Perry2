#include "EditorPCH.h"
#pragma hdrstop

using namespace Perry;

void EntityViewer::GenerateComponentList(Perry::Entity& e)
{
	if (e.Valid())
		for (auto&& [id, storage] : (*GetEditor().SelectedRegistry).storage())
		{
			if (storage.contains(e))
			{
				SelectedEntityComponents.emplace_back(entt::resolve(storage.type()).id());
			}
		}
}

std::string GetDisplayString(entt::registry& reg, entt::entity e)
{
	auto entityID = entt::to_entity(e);
	auto entityVersion = entt::to_version(e);

	std::string label = reg.get_or_emplace<Perry::NameComponent>(e).name;
	if (entityVersion != 0)
	{
		label += " (" + std::to_string(entityID) + " V: " + std::to_string(entityVersion) + ")";
	}
	else
	{
		label += " (" + std::to_string(entityID) + ")";
	}

	return label;
}

void EntityViewer::DrawEnttRecursive(entt::registry& reg, entt::entity e, Perry::HierarchyComponent* h)
{

	ImGui::PushID(static_cast<int>(e));
	std::string label = GetDisplayString(reg, e);
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	bool isSelectedEntity = false;

	if (GetEditor().GetSelectedEntity() == e)
	{
		flags += ImGuiTreeNodeFlags_Selected;
		isSelectedEntity = true;
	}

	bool hasChild = false;
	if (h)
	{
		hasChild = reg.valid(Transform::GetChild(h));
	}

	if (!hasChild)
		flags += ImGuiTreeNodeFlags_Leaf;

	bool isOpen = ImGui::TreeNodeEx((label).c_str(), flags);

	ContextMenu::GenerateEntityContext({ &reg,e });
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload(
			"EntiyID", &e, sizeof(entt::entity)); // Set payload to carry the index of our item (could be anything)
		ImGui::Text("%s", label.c_str());
		ImGui::EndDragDropSource();
	}

	// ImGui::Dummy(ImVec2(ImGui::GetWindowWidth(), 2));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntiyID"))
		{
			IM_ASSERT(payload->DataSize == sizeof(entt::entity));

			entt::entity payload_n = *(const entt::entity*)payload->Data;
			Transform::SetParent(e, payload_n);
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked() && !ImGui::IsItemEdited() && !ImGui::IsMouseDragging(0, 0.1f))
		if (!isSelectedEntity)
			GetEditor().SetSelectedEntity({ &reg, e });

	if (isOpen)
	{
		if (!hasChild)
		{
			ImGui::TreePop();
			ImGui::PopID();
			return;
		}

		auto& c = reg.get<Perry::HierarchyComponent>(Transform::GetChild(h));
		DrawEnttRecursive(reg, Transform::GetChild(h), &c);
		for (auto i : c.begin(reg))
		{
			auto& cc = reg.get<Perry::HierarchyComponent>(i);

			DrawEnttRecursive(reg, i, &cc);
		}

		ImGui::TreePop();
	}
	ImGui::PopID();

	return;
}

bool containsIgnoreCase(const std::string& str, const std::string& sub)
{
	if (sub.empty())
		return true;
	// Convert both strings to lowercase
	std::string strLower = str;
	std::string subLower = sub;
	std::transform(strLower.begin(), strLower.end(), strLower.begin(), [](unsigned char c) { return std::tolower(c); });
	std::transform(subLower.begin(), subLower.end(), subLower.begin(), [](unsigned char c) { return std::tolower(c); });

	// Check if strLower contains subLower
	return strLower.find(subLower) != std::string::npos;
}

bool MatchComponentFilter(entt::registry& reg, entt::entity e, const std::vector<entt::meta_type>& components)
{
	if (components.empty())
		return true;

	for (auto componentType : components)
	{
		auto l = componentType.func(f_ContainsComponent).invoke({}, entt::forward_as_meta(reg), e);
		if (!l.cast<bool>())
			return false;
	}
	return true;
}

void EntityViewer::DrawEnttFilterd(entt::registry& reg, entt::entity e, const std::string& nameFilter,
	const std::vector<entt::meta_type>& components)
{
	std::string label = GetDisplayString(reg, e);

	if (containsIgnoreCase(label, nameFilter) && MatchComponentFilter(reg, e, components))
	{
		ImGui::PushID(static_cast<int>(e));
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;

		if (GetEditor().GetSelectedEntity() == e)
			flags += ImGuiTreeNodeFlags_Selected;

		bool isOpen = ImGui::TreeNodeEx((label).c_str(), flags);

		ContextMenu::GenerateEntityContext({ &reg,e });

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload(
				"EntiyID", &e, sizeof(entt::entity)); // Set payload to carry the index of our item (could be anything)
			ImGui::Text("%s", label.c_str());
			ImGui::EndDragDropSource();
		}

		// ImGui::Dummy(ImVec2(ImGui::GetWindowWidth(), 2));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EntiyID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(entt::entity));

				entt::entity payload_n = *(const entt::entity*)payload->Data;
				Transform::SetParent(e, payload_n);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked() && !ImGui::IsItemEdited() && !ImGui::IsMouseDragging(0, 0.1f))
			if (e != GetEditor().GetSelectedEntity())
				GetEditor().SetSelectedEntity({ &reg,e });

		if (isOpen)
		{
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	return;
}

void DrawComponentFilter(std::vector<entt::meta_type>& currentData, std::string& ComponentSearch)
{
	if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
		ImGui::SetKeyboardFocusHere(0);

	ImGui::InputText("SearchName", &ComponentSearch);

	for (auto&& [metaID, metaType] : entt::resolve())
	{
		if (!metaType.prop(T_ReflectedComponent))
			continue;

		auto name = metaType.prop(p_DisplayName).value().cast<std::string>();
		if (!containsIgnoreCase(name, ComponentSearch))
		{
			continue;
		}

#ifndef ShowInternal
		if (metaType.prop(p_Internal))
			continue;

#endif
		entt::meta_type metaTypeCopy = metaType;

		auto selectedIndex =
			std::find_if(std::begin(currentData),
				std::end(currentData),
				[metaTypeCopy](entt::meta_type& type)
				{ return static_cast<unsigned>(type.id()) == static_cast<unsigned>(metaTypeCopy.id()); });
		bool selected = selectedIndex != std::end(currentData);

		ImGui::PushID(metaID);

		if (ImGui::Selectable(name.c_str(), selected))
		{
			if (selected)
				currentData.erase(selectedIndex);
			else
				currentData.emplace_back(metaType);
		}

		ImGui::PopID();
	}
}

ImVec2 AddImVec(ImVec2 a, ImVec2 b)
{
	return ImVec2(a.x + b.x, a.y + b.y);
}

EntityViewer::EntityViewer()
{
}

void EntityViewer::Draw(entt::registry& reg)
{
	// ImGui::SetNextWindowPos(ImVec2(window.GetWindowWidth() - 550.f, 50.f), ImGuiCond_FirstUseEver);
	// ImGui::SetNextWindowSize(ImVec2(550, 600), ImGuiCond_FirstUseEver);

	if (ImGui::Button("NewObject"))
	{
		auto ent = Entity::Create(reg);
		SelectedEntity = ent;
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete"))
	{
		if (SelectedEntity.Valid())
			SelectedEntity.Delete();
	}


	static ImVec2 entityList{ 0, 100 };
	static ImVec2 componentList{ 0, 500 };

	{
		static std::string nameFilter;
		static std::vector<entt::meta_type> componentFilter{};

		std::string filterTitle = "ComponentFilter";
		if (!componentFilter.empty())
			filterTitle += "(Active)";

		if (ImGui::Button(filterTitle.c_str()))
		{
			ComponentSearch = "";
			ImGui::OpenPopup("FilterComponentModal");
		}

		ImGui::SameLine();
		ImGui::InputText("SearchName", &nameFilter);

		if (ImGui::BeginPopup("FilterComponentModal"))
		{
			DrawComponentFilter(componentFilter, ComponentSearch);
			ImGui::EndPopup();
		}

		ImGui::PushItemWidth(-1);

		if (ImGui::BeginListBox("##Empty", ImVec2(0, entityList.y)))
		{
			auto entityView = reg.view<entt::entity>();

			if (nameFilter.empty() && componentFilter.empty())
			{
				// TODO we could optimize this by looping over all entities with hiearchy first then without.
				for (auto&& [e] : entityView.each())
				{
					if (!reg.valid(e))
						continue;

					auto* h = reg.try_get<Perry::HierarchyComponent>(e);

					if (h)
					{
						// If we are not the root, We will wait with drawing this Entity
						if (!Transform::HasParent(h))
							DrawEnttRecursive(reg, e, h);
					}
					else
					{
						DrawEnttRecursive(reg, e, h);
					}
				}
			}
			else
			{
				for (auto&& [e] : entityView.each())
				{
					DrawEnttFilterd(reg, e, nameFilter, componentFilter);
				}
			}

			ImGui::EndListBox();
		}
	}

	if (!GetEditor().HasEntitySelected())
		return;

	if (SelectedEntity != GetEditor().GetSelectedEntity())
	{
		// We are responsible telling editor if we changed selection. so always force copy there entity ID to us.
		SelectedEntity = GetEditor().GetSelectedEntity();
		GenerateComponentList(SelectedEntity);
	}


	Perry::Entity selectedE = GetEditor().GetSelectedEntity();

	ImVec2 realCursor = AddImVec(ImGui::GetCursorPos(), ImVec2(0, -ImGui::GetScrollY()));

	ImVec2 max = AddImVec(AddImVec(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowWidth(), 5)), realCursor);

	ImGui::SplitterBehavior(ImRect(AddImVec(realCursor, ImGui::GetWindowPos()), max),
		ImGui::GetID("Entities"),
		ImGuiAxis_Y,
		&entityList.y,
		&componentList.y,
		50,
		50,
		5,
		1);
	ImGui::Dummy(ImVec2(0, 5));

	ImGui::BeginChild("entityList");
	{
		if (selectedE.Valid())
		{
			if (ImGui::Button("AddComponent"))
			{
				ComponentSearch = "";
				ImGui::OpenPopup("AddComponentModal");
			}

			// For each Storage that contains current entity

			for (auto&& [id, storage] : selectedE.GetRegistry().storage())
			{
				if (storage.contains(selectedE))
				{
					auto componentMeta = entt::resolve(storage.type());

					// Entity has no meta data...
					if (!componentMeta)
						continue;

					if (!componentMeta.prop(T_ReflectedComponent))
					{
						if (componentMeta.prop(p_Internal))
							return; // This combo will probably never happen...

						if (ImGui::CollapsingHeader(componentMeta.info().name().data(),
							ImGuiTreeNodeFlags_CollapsingHeader))
						{
							ImGui::TextColored(ImColor(255, 0, 0),
								"Component is not reflected, Add 'REFLECT_COMPONENT' macro");
						}

						continue;
					}

					void* dat = storage.value(selectedE);
					entt::meta_any metaProp = componentMeta.from_void(dat);
					ASSERT_MSG(LOG_SYSTEM, metaProp.data(), "invalid meta prop");
					EnttEditor::Inspector::InspectComponent(reg, selectedE.GetID(), metaProp);
				}
			}

			if (ImGui::BeginPopup("AddComponentModal"))
			{
				if (!ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
					ImGui::SetKeyboardFocusHere(0);

				ImGui::InputText("SearchName", &ComponentSearch);

				for (auto&& [metaID, metaType] : entt::resolve())
				{
					if (!metaType.prop(T_ReflectedComponent))
						continue;

#ifndef ShowInternal
					if (metaType.prop(p_Internal))
						continue;

#endif
					std::string componentName = metaType.prop(p_DisplayName).value().cast<std::string>();
					if (!containsIgnoreCase(componentName, ComponentSearch))
						continue;

					if (std::find(std::begin(SelectedEntityComponents),
						std::end(SelectedEntityComponents),
						metaType.id()) != SelectedEntityComponents.end())
						continue;

					ImGui::PushID(metaID);

					auto flags = ImGuiSelectableFlags_None;

					auto f = metaType.func(f_AddComponent);

					if (!f)
						flags = ImGuiSelectableFlags_Disabled;

					if (ImGui::Selectable(componentName.c_str(), false, flags))
					{
						// TODO fix calling it with parameters
						// We do this by creating a modal popup, and showing args from function (YES THIS WORKS :O)
						// f.arg()
						f.invoke({}, entt::forward_as_meta(reg), SelectedEntity.GetID());
						// Refresh active selection
						GenerateComponentList(SelectedEntity);
					}
					ImGui::PopID();
				}
				ImGui::EndPopup();
			}
		}
	}
	ImGui::EndChild();
}