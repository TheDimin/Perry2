#include "EnginePCH.h"

#include "Lua/Lua.hpp"
#include "Lua/LuaInspect.h"
#pragma hdrstop

void LuaInspect::CallFunction(lua_State* luaState, const entt::meta_any& value)
{
#ifdef LUA_ENABLED
	if (value.type().is_class())
	{
		for (auto&& [id, element] : value.type().data())
		{
			//TODO check if type is table, Recursive call to this

			const auto l = value.get(id);
			auto f = element.type().func(p_LuaPush);
			const std::string t = element.prop(p_DisplayName).value().cast<std::string>();
			f.invoke({}, luaState, entt::forward_as_any(&t), &l);
		}
	}
#endif
}