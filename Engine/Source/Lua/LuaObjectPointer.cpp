#include "EnginePCH.h"

#include <Lua/lua.hpp>
#include "Lua/LuaInspect.h"
#pragma hdrstop

//Yeah the lua include is broken... fix soonTM

void LuaObjectPointerV2::RegisterType(lua_State* luaState, const entt::meta_type& type)
{
#ifdef ENABLE_LUA
	lua_getglobal(luaState, "_CLASSES");
	lua_getfield(luaState, -1, "NewType");


	//TODO we now push every parameter seperate, we may want to push it as a table of options ?
	//Order of this is determined by Class.Lua::NewType
	lua_pushvalue(luaState, -2);
	lua_pushstring(luaState, type.prop(p_DisplayName).value().cast<std::string>().c_str());

	auto f = type.func("PushCallBack"_hs);
	if (f)
		f.invoke({}, luaState);
	else
		printf("Cringe coder invalid meta type...\n");

	if (type.prop(p_LuaHiddenType))
		lua_pushboolean(luaState, false);
	else
		lua_pushboolean(luaState, true);

	if (lua_pcall(luaState, 4, 0, 0) != LUA_OK) {
		const char* errorMessage = lua_tostring(luaState, -1);
		printf("[LUA_C 4,0,0] ERROR: LUA threw an error calling 'RegisterType':: %s\n", errorMessage);
		assert(false);
	}
#endif
}

void LuaObjectPointerV2::New(lua_State* luaState, entt::meta_any& object, bool copyObject)
{
	if (copyObject)
	{
		assert(object.type().size_of() != 0);
		void* newObject = lua_newuserdata(luaState, object.type().size_of());
		memcpy(newObject, object.data(), object.type().size_of());
	}
	else
		lua_pushlightuserdata(luaState, object.data());

	auto typeName = object.type().prop(p_DisplayName).value().cast<std::string>();
	CreateLuaObject(luaState, typeName);
	lua_remove(luaState, -2);
}

void LuaObjectPointerV2::NewGlobal(lua_State* luaState, const std::string& globalName, entt::meta_any&& object)
{
	New(luaState, object);

	lua_setglobal(luaState, globalName.c_str());
}

void LuaObjectPointerV2::CreateLuaObject(lua_State* luaState, const std::string& className)
{
	lua_getglobal(luaState, "_CLASSES");
	lua_getfield(luaState, -1, "CNew");

	//TODO we now push every parameter seperate, we may want to push it as a table of options ?
	//Order of this is determined by Class.Lua::CNew
	lua_pushvalue(luaState, -2);
	lua_pushstring(luaState, className.c_str());

	lua_pushvalue(luaState, -5);//Push the Light user data back on the stack

	if (lua_pcall(luaState, 3, 1, 0) != LUA_OK) {
		const char* errorMessage = lua_tostring(luaState, -1);
		printf("LUA threw an Error calling '%s':: %s\n", "RegisterType", errorMessage);
		Perry::LuaSystem::PrintTraceBack(luaState);
	}

	lua_remove(luaState, -2);
}

std::vector<entt::meta_any> LuaObjectPointerV2::NewIn(lua_State* luaState, int startIndex)
{
	const int stackSize = lua_gettop(luaState); // Get the number of elements on the stack
	if (stackSize == 0) return {};
	std::vector<entt::meta_any> params = std::vector<entt::meta_any>((stackSize - startIndex));

	for (int i = 0; i < stackSize - startIndex; i++) {
		params[i] = LuaInspect::LuaPullProperty(luaState, i + startIndex + 1);
	}
	return params;
}
