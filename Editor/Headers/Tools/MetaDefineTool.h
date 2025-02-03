#pragma once

namespace Perry {
	namespace Reflect {
		constexpr entt::hashed_string T_ReflectedTool = "ReflectedTool"_hs;

		template<typename T>
		static entt::meta_factory<T> ReflectTool(const std::string& name)
		{
			entt::locator<entt::meta_ctx>::reset(EnttEditor::MetaContext::GetMetaContext());

			auto factoryT = entt::meta<T>();

			factoryT.prop(T_ReflectedTool);
			factoryT.template base<ToolBase>();
			factoryT.type(entt::hashed_string{ name.c_str() });
			factoryT PROP_DISPLAYNAME(name);


			return factoryT;
		}
	}
}



#define IMPLEMENT_REFLECT_TOOL(TYPE) \
	_REFLECT_INTERNAL(TYPE);\
	int TYPE::InitTypeReflect(){\
	[[maybe_unused]] auto meta = Perry::Reflect::ReflectTool<TYPE>((#TYPE));