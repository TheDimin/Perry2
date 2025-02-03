#include "EditorPCH.h"
#pragma hdrstop

void Inspector::InspectComponent(Perry::Entity& owner, entt::meta_any& data)
{
	if (!data)
	{//TODO debug & research why data.pointer is invalid...
		return;
	}

#ifndef ShowInternal
	if (data.type().prop(p_Internal))
		return;
#endif
	std::string componentName = std::string(data.type().info().name()); //TODO: trim generated name.

	//Incase the user has set a custom Name, use that otherwise fallback to generated one
	{
		entt::meta_prop userPropertyName = data.type().prop(p_DisplayName);
		if (userPropertyName && userPropertyName.value().allow_cast<std::string>())
			componentName = userPropertyName.value().cast<std::string>();
	}

	bool MenuOpen = ImGui::CollapsingHeader(componentName.c_str());
	ContextMenu::GenerateComponentContext(owner, data);

	if (!MenuOpen)
		return;

	ImGui::Indent(10.f);
	std::string id = std::to_string(data.type().id()).append("Component");
	ImGui::PushID(id.c_str());

	for (auto&& [functionID, functionMeta] : data.type().func())
	{
		if (functionMeta.prop(p_ShownInEditor))
			InspectFunction(functionMeta);
	}

	//For each property on the entity
	for (auto&& [propertyId, propertyMetaData] : data.type().data())
	{
		auto property = data.get(propertyId);
		auto oldProp = property;

		InspectProperty(property, propertyMetaData, propertyId);

		data.set(propertyId, property);
		if (property != oldProp)
		{
			if (!data.type().func(f_PatchComponent).invoke({}, entt::forward_as_meta(owner.GetRegistry()), owner.GetID()))
			{
				ERROR(LOG_GAMEOBJECTS, "Failed to patch object");
			}
		}

	}
	ImGui::Unindent(10);
	ImGui::PopID();
}

void Inspector::InspectClass(entt::meta_any& elementObject, const unsigned elementID, bool createTreeNode)
{
	//Todo generate a nicer inspector name
	std::string sid = std::to_string(elementID).append("Container");
	ImGui::PushID(sid.c_str());

	auto elementType = elementObject.type();
	int i = 0;
	while (elementType.is_pointer_like() && i <= 2)
	{
		elementType = elementType.remove_pointer();
		i++;
		//assert(i > 3);
	}


	bool treeNodeOpen = true;

	if (createTreeNode) {
		treeNodeOpen = ImGui::TreeNode(elementType.info().name().data());
	}

	if (treeNodeOpen)
	{
		bool hasElements = false;

		//Here we loop over all elements inside the container
		//The elementData is metadata associated with the element.
		for (auto&& [containerElementID, containerElementData] : elementType.data())
		{
			hasElements = true;
			//A property inside the container
			auto containerElement = elementObject.get(containerElementID);

			InspectProperty(containerElement, containerElementData, containerElementID);
			elementObject.set(containerElementID, containerElement);
		}

		if (!hasElements)
			ImGui::Text("No member variables exposed");

		//Loop over any baseclass of the container
		for (auto&& [baseID, baseType] : elementType.base())
		{
			ImGui::Separator();
			//We have to cast the meta_any from containing meta_type child class to meta_type of Parent type
			entt::meta_any elementObjectBase = baseType.from_void(elementObject.data());
			InspectClass(elementObjectBase, baseID, false);
		}

		if (createTreeNode)
			ImGui::TreePop();
	}

	ImGui::PopID();
}

void Inspector::InspectProperty(entt::meta_any& property, entt::meta_data& propertyMeta, const unsigned propertyID)
{
	entt::meta_type propertyType = property.type();
#ifndef ShowInternal
	if (propertyType.prop(p_Internal))
		return;
#endif
	if (propertyMeta.prop(p_HideFromInspector)) return;

	if (propertyType.is_pointer_like())
	{
		propertyType = propertyType.remove_pointer();
		if (entt::meta_any ref = *property; ref)
		{
			InspectProperty(ref, propertyMeta, propertyID);
			return;
		}
	}

	ImGui::PushID((std::to_string(propertyType.id()) + "prop").c_str());

	entt::meta_func&& propertyInspectFunction = propertyType.func(f_Inspect);

	std::string propertyName = std::string(propertyType.info().name()); //TODO: trim generated name.

	//Incase the user has set a custom Name, use that otherwise fallback to generated one
	{
		entt::meta_prop userPropertyName = propertyMeta.prop(p_DisplayName);
		if (userPropertyName && userPropertyName.value().allow_cast<std::string>())
			propertyName = userPropertyName.value().cast<std::string>();
	}

	//Does this property have a defined Inspect function?
	if (propertyInspectFunction)
	{
		InspectType(propertyName, property, propertyMeta);
	}
	else if (propertyType.is_sequence_container())
	{
		entt::meta_sequence_container propertyAsContainer = property.as_sequence_container();
		ImGui::Text("%s", propertyName.c_str()); ImGui::SameLine();
		ImGui::Separator();
		ImGui::Indent(10);

		int i = 0;

		entt::meta_type valueType = propertyAsContainer.value_type();
		bool wasPointerLike = false;
		if (valueType.is_pointer_like())
		{
			wasPointerLike = true;
			valueType = valueType.remove_pointer();
		}

		propertyInspectFunction = valueType.func(f_Inspect);
		for (auto cl = propertyAsContainer.begin(); cl != propertyAsContainer.end(); ++cl)
		{

			///This is not ideal, but atleast supports recursive container types.
			entt::meta_any containerElement = valueType.from_void(cl->data());
			InspectProperty(containerElement, propertyMeta, propertyID);

			/*
			auto finalContElement = containerElement;
			//incase we are a pointer cast it away
			if (wasPointerLike)
			{
				finalContElement = *finalContElement;

				if (!finalContElement)
				{
					printf("Failed a cast \n");
					continue;
				}
			}

			if (!valueType.is_class())
			{
				const std::string elementName = std::to_string(i);

				auto propertyInspected = propertyInspectFunction.invoke<const std::string&, entt::meta_any*, const entt::meta_data&>({}, elementName, &finalContElement, propertyMeta);

				if (!propertyInspected)
					printf("Failed to inspect container property....\n");
			}
			else
			{
				InspectClass(containerElement, propertyID);
			}
			i++;*/
		}

		ImGui::Unindent(10);
		ImGui::Separator();
	}
	else if (propertyType.is_class())
	{
		ImGui::Text("%s", propertyName.c_str()); ImGui::SameLine();
		InspectClass(property, propertyID);
	}
	else
	{
		ImGui::TextColored(ImColor(255, 0, 0), "[%s]: Unreflected type: %s\n", propertyName.c_str(), propertyType.info().name().data());
	}
	ImGui::PopID();
}

void Inspector::InspectType(std::string& name, entt::meta_any& value, entt::meta_data& meta)
{
	entt::meta_type type = value.type();

	if (!type.func(f_Inspect))
	{
		ImGui::Text("No meta inspect function detected\n");
		return;
	}

	PreInspect(name, value, meta);

	if (meta.prop(p_ReadOnlyInInspector))
	{
		entt::meta_any v = value;
		auto propertyInspected = type.func(f_Inspect).invoke< const char*, entt::meta_any, const entt::meta_data&>({}, name.c_str(), entt::forward_as_meta(v), meta);
		if (!propertyInspected)
			ImGui::TextColored(ImVec4(1, 0, 0, 0), "Failed to render type");
	}
	else
	{
		auto propertyInspected = type.func(f_Inspect).invoke< const char*, entt::meta_any, const entt::meta_data&>({}, name.c_str(), entt::forward_as_meta(value), meta);

		if (!propertyInspected)
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to render type");

	}
	PostInspect(name, value, meta);
}

void Inspector::InspectFunction(entt::meta_func& func)
{
	std::string functionName = "unnamed function";
	entt::meta_prop functionNameProperty = func.prop(p_DisplayName);
	if (functionNameProperty && functionNameProperty.value().allow_cast<std::string>())
		functionName = functionNameProperty.value().cast<std::string>();

	if (ImGui::Button(functionName.c_str()))
	{
		func.invoke({});
	}

	//For now functions don't support pre/post inspect
	auto toolTip = func.prop(p_ToolTip);
	if (toolTip && ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", toolTip.value().cast<std::string>().c_str());
}

void Inspector::PreInspect(const std::string& name, entt::meta_any& value, const entt::meta_data& meta)
{
	if (meta.prop(p_ReadOnlyInInspector))
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1.0));
}

void Inspector::PostInspect(const std::string& name, entt::meta_any& value, const entt::meta_data& meta)
{
	if (meta.prop(p_ReadOnlyInInspector))
		ImGui::PopStyleColor(1);

	auto toolTip = meta.prop(p_ToolTip);
	if (toolTip && ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", toolTip.value().cast<std::string>().c_str());
}
