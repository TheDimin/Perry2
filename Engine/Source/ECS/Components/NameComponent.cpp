#include "EnginePCH.h"
#pragma hdrstop

IMPLEMENT_REFLECT_COMPONENT(Perry::NameComponent)
{
	meta.func<&entt::registry::emplace<Perry::NameComponent, const std::string&>>(f_AddComponent);
	meta.prop(p_Internal);
	meta.ctor<const std::string&>();

	meta.data<&Perry::NameComponent::name>("NAME"_hs)
		PROP_DISPLAYNAME("Name");

}
FINISH_REFLECT()