#include "EnginePCH.h"
#pragma hdrstop
#include <Lua/lua.hpp>
#include "Lua/LuaInspect.h"
#include "Lua/LuaEvent.h"

#include <regex>

namespace Perry
{
	IMPLEMENT_REFLECT_COMPONENT(ScriptComponent)
	{
		meta.data<&ScriptComponent::scriptFile>("m_ScriptFile"_hs)
			PROP_DISPLAYNAME("file");

		meta.data<&ScriptComponent::valid>("m_Valid"_hs)
			PROP_DISPLAYNAME("IsValid")
			PROP_READONLY;

	}
	FINISH_REFLECT();

	IMPLEMENT_REFLECT_SYSTEM(LuaSystem)
	{

	}
	FINISH_REFLECT();


	//Simple wrapper around LUA attempt at making it simpler to deal with tables from c++
	//TODO move this to new type system
	class LuaEntity
	{
	public:
		static bool Create(entt::registry& reg, ScriptComponent& scriptComponent)
		{
			//UserTable
			entt::entity e = Perry::to_entity(reg, scriptComponent);
			lua_State* luaState = scriptComponent.owner;


			lua_getglobal(luaState, "Entities");

			lua_pushnumber(luaState, entt::to_integral(e));

			lua_newtable(luaState);
			{
				//TODO Place this all in a proxy tables including entityID

				lua_pushstring(luaState, "_Entity");
				lua_pushinteger(luaState, entt::to_integral(e));
				lua_settable(luaState, -3);

				if (!Insert(scriptComponent))
					return false;

				if (!Finish(scriptComponent))
					return false;
			}
			return true;

		}
	private:

		static bool Finish(ScriptComponent& scriptComponent) {

			lua_pushvalue(scriptComponent.owner, -1);
			lua_setmetatable(scriptComponent.owner, -1);

			lua_settable(scriptComponent.owner, -3);

			lua_settop(scriptComponent.owner, 0);
			return true;
		}

		static bool Insert(ScriptComponent& scriptComponent)
		{
			lua_pushstring(scriptComponent.owner, "__index");
			if (!LuaSystem::LoadFile(scriptComponent.owner, scriptComponent.scriptFile, 0, 1))
			{
				const char* errorMessage = lua_tostring(scriptComponent.owner, -1);
				ERROR(LOG_LUA, "%s\n", errorMessage);
				lua_pop(scriptComponent.owner, 1);
				return false;
			}

			lua_settable(scriptComponent.owner, -3);

			return true;
		}
	};

	void LuaSystem::OnScriptComponentCreated(entt::registry& reg, entt::entity e)
	{
		auto& script = reg.get<ScriptComponent>(e);
		ASSERT("LUA", luaState);
		script.owner = luaState;
		script.valid = LuaEntity::Create(reg, script);
	}

	//chat gpt
	int add_path(lua_State* L, const char* path) {
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path");  // get current package.path
		const char* cur_path = lua_tostring(L, -1);
		lua_pushfstring(L, "%s;%s/?.lua", cur_path, path);  // append new path
		lua_setfield(L, -3, "path");  // set new package.path
		lua_pop(L, 1);  // pop package table
		return 0;
	}

	void LuaSystem::Init(entt::registry& reg)
	{
		INFO(LOG_LUA,"Initialization started");

		reg.on_construct<ScriptComponent>().connect<&LuaSystem::OnScriptComponentCreated>(this);

		instance = this;
		luaState = luaL_newstate();

		luaL_openlibs(luaState);

		std::string p = "package.path = 'D:/P2/Build/x64/Debug_Rts/Resources/Scripts/?.lua'";//"package.path = '" + FileIO::GetPath(DirectoryType::Resource, "Scripts") + "/?.lua'";

		int result = luaL_dostring(luaState, p.c_str());
		if (result != LUA_OK) {
			const char* error = lua_tostring(luaState, -1);
			ERROR(LOG_LUA, "%s\n", error);
			lua_pop(luaState, 1); // Pop the error message from the stack
		}


		if (!luaState)
		{
			ERROR(LOG_LUA, "Failed to create new state");
			return;
		}

		//TODO foreach file in luastate load in order// maybe do this from lau itself?

		if (!LoadFile(luaState, "main"))
		{
			IsEnabled = false;
			return;
		}


		lua_newtable(luaState);
		lua_setglobal(luaState, "Entities");
		//Todo This should be done using entt::meta loop
		{
			for (auto&& [TypeId, value] : entt::resolve())
			{
				if (value.prop(p_Scriptable))
					LuaObjectPointerV2::RegisterType(luaState, value);
			}
		}

		LuaObjectPointerV2::NewGlobal(luaState, "Input", entt::forward_as_meta(GetInput()));
		LuaObjectPointerV2::NewGlobal(luaState, "Engine", entt::forward_as_meta(GetEngine()));

		LuaEvent<void> initEvent = LuaEvent<void>("Init");
		initEvent.Invoke(luaState);

		INFO(LOG_LUA,"Initialization Finished");
	}
	static LuaEvent<void> GlobalUpdateEvent = LuaEvent<void>("Update");
	void LuaSystem::Update(entt::registry& reg)
	{
		//GlobalUpdateEvent.Invoke(luaState);
	}

	void LuaSystem::PrintTable(lua_State* luaState)
	{
		if ((lua_type(luaState, -1) == LUA_TSTRING))
		{
			ERROR(LOG_LUA, "Expected table as argument given value: %s",lua_tostring(luaState, -1));
			return;
		}


		lua_pushnil(luaState);
		while (lua_next(luaState, -2) != 0) {
			if (lua_isstring(luaState, -1))
				printf("%s = %s", lua_tostring(luaState, -2), lua_tostring(luaState, -1));
			else if (lua_isnumber(luaState, -1))
				printf("%s = %f", lua_tostring(luaState, -2), lua_tonumber(luaState, -1));
			else if (lua_istable(luaState, -1)) {
				PrintTable(luaState);
			}
			lua_pop(luaState, 1);
		}
	}

	void LuaSystem::PrintStack(lua_State* luaState)
	{
		int nargs = lua_gettop(luaState);

		int top = lua_gettop(luaState);

		// Check that the top of the stack is a table
		if (lua_istable(luaState, nargs)) 
		{
			ERROR(LOG_LUA,"Expected table as argument\n");
		}

		std::cout << "Lua stack:\n";
		for (int i = 1; i <= top; i++) {
			int type = lua_type(luaState, i);
			switch (type) {
			case LUA_TNIL:
				std::cout << "[" << i << "] NIL\n";
				break;
			case LUA_TBOOLEAN:
				std::cout << "[" << i << "] " << (lua_toboolean(luaState, i) ? "true" : "false") << "\n";
				break;
			case LUA_TNUMBER:
				std::cout << "[" << i << "] " << lua_tonumber(luaState, i) << "\n";
				break;
			case LUA_TSTRING:
				std::cout << "[" << i << "] \"" << lua_tostring(luaState, i) << "\"\n";
				break;
			case LUA_TTABLE:
				std::cout << "[" << i << "] TABLE\n";
				break;
			case LUA_TFUNCTION:
				std::cout << "[" << i << "] FUNCTION\n";
				break;
			case LUA_TUSERDATA:
				std::cout << "[" << i << "] USERDATA\n";
				break;
			default:
				std::cout << "[" << i << "] " << lua_typename(luaState, type) << "\n";
				break;
			}
		}
	}

	void LuaSystem::PrintDebugInfo(lua_State* luaState)
	{
		lua_Debug dd{};
		lua_getstack(luaState, 0, &dd);
		if (dd.name)
			printf("Name: %s\n", dd.name);
		if (dd.source)
			printf("Source: %s\n", dd.source);
		printf("Current line: %d\n", dd.currentline);
		printf("Line defined: %d\n", dd.linedefined);
		printf("Last line defined: %d\n", dd.lastlinedefined);
		if (dd.what)
			printf("What: %s\n", dd.what);
		if (dd.namewhat)
			printf("Function type: %s\n", dd.namewhat);
		printf("Is tail call: %d\n", dd.istailcall);
	}

	void LuaSystem::PrintTraceBack(lua_State* luaState)
	{
		// Retrieve line number using the debug library in Lua
		lua_getglobal(luaState, "debug");
		lua_getfield(luaState, -1, "traceback");
		lua_call(luaState, 0, 1);
		const char* traceback = lua_tostring(luaState, -1);
		lua_pop(luaState, 2);

		printf("[LUA_C] ERROR: Lua traceback:\n%s\n", traceback);

		int stackSize = lua_gettop(luaState);
		lua_pop(luaState, stackSize);
	}

	bool LuaSystem::LoadFile(lua_State* luaState, const std::string& fileName, int nargs, int nresults, int msgh)
	{
		if (luaL_loadfile(luaState,
			FileIO::GetPath(DirectoryType::Resource, "Scripts/" + fileName + ".lua").c_str()) || lua_pcall(luaState, nargs, nresults, msgh))
		{
			const char* errorMessage = lua_tostring(luaState, -1);
			printf("Error: %s\n", errorMessage);

			return false;
		}

		return true;
	}

	LuaSystem::~LuaSystem()
	{
		if (luaState)
		{
			lua_close(luaState);
			luaState = nullptr;
		}
	}
}
