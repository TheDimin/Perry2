#pragma once

namespace Perry
{
	template<typename T>
	static System* CreateSystem()
	{
		return new T();
	}

	template<typename T>
	static inline entt::meta_factory<T> ReflectSystem(const std::string& name)
	{
		entt::locator<entt::meta_ctx>::reset(EnttEditor::MetaContext::GetMetaContext());

		auto factoryT = entt::meta<T>();

		/*if constexpr (MetaInspectors::is_meta_inspect_callable_v<T>)
		{
			factoryT.template func <&MetaInspectors::Inspect<T>>(f_Inspect);
		}

		if constexpr (is_meta_serialize_callable_v<T>)
		{
			factoryT.template func <&Perry::Serializers::MetaSerialize<T>>(f_Serialize);
		}

		if constexpr (is_meta_deserialize_callable_v<T>)
		{
			//factoryT.template func <&Perry::Serializers::Deserialize<T>>(f_Deserialize);
		}*/


		//LUA
		//This does not guarantee the type will be LuaCallable!!! (see p_Scriptable)
		//factoryT.template func<&LuaObjectTemplate::TLinker<T>>("PushCallBack"_hs);
		//factoryT.template func<&LuaInspect::LuaPushProperty<T>>(p_LuaPush);
		//factoryT.template func<&LuaInspect::LuaPullProperty<T>>(p_LuaPull);

		factoryT.template func<&CreateSystem<T>>(c_CreateSystem);


		factoryT.template base<Perry::System>();
		factoryT.type(entt::hashed_string{ name.c_str() });
		factoryT PROP_DISPLAYNAME(name);
		factoryT.prop(T_ReflectedSystem);

		return factoryT;
	}

}

#define IMPLEMENT_REFLECT_SYSTEM(TYPE) \
	_REFLECT_INTERNAL(TYPE);\
	int TYPE::InitTypeReflect(){\
	[[maybe_unused]] auto meta = Perry::ReflectSystem<TYPE>((#TYPE));\

#define TYPE_TO_ID(Type) entt::resolve<Type>().id()