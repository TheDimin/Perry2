#pragma once

namespace Perry
{

	template<typename ReturnType = void, typename... Args>
	class LuaEvent {
	public:
		//template<typename = std::enable_if_t<sizeof...(Args) == 0>>
		LuaEvent(const std::string& funcName) : m_FunctionName(funcName) {}

		template<typename... Names>
		LuaEvent(const std::string& funcName)
			: m_FunctionName(funcName)
		{}


		/// <summary>
		/// Invoke event as a global event (Function will be looked up in global table !)
		/// </summary>
		/// <param name="luaState"></param>
		/// <param name="...args"></param>
		/// <returns></returns>
		ReturnType Invoke(lua_State* luaState, Args&... args) const
		{
			int nArgs = sizeof...(args);
			int returnValC = static_cast<int>(!std::is_same_v<ReturnType, void>);

			//TODO support invoke within a "table"
			lua_getglobal(luaState, m_FunctionName.c_str());

			if (!lua_isfunction(luaState, -1))
			{
				printf("Failed to find '%s' as a lua function \n", m_FunctionName.c_str());
				return ReturnType();
			}

			if constexpr (sizeof...(Args) != 0)
			{
				(LuaInspect::LuaPushProperty(luaState, args...));
			}

			if (lua_pcall(luaState, nArgs, returnValC, 0) != LUA_OK) {
				const char* errorMessage = lua_tostring(luaState, -1);
				printf("[LUA_C] ERROR: LUA threw an error calling '%s': %s\n", m_FunctionName.c_str(), errorMessage);
				Perry::LuaSystem::PrintTraceBack(luaState);
				return ReturnType();
			}


			if constexpr (!std::is_same_v<ReturnType, void>)
			{
				entt::meta_any returnValue;
				LuaInspect::LuaPullProperty<ReturnType>(luaState, -1, &returnValue);

				lua_settop(luaState, 0);
				return returnValue.cast<ReturnType>();
			}
			else
			{
				lua_settop(luaState, 0);
				//Suppose we return a void() here;
				return ReturnType();
			}
		}

		ReturnType Invoke(Perry::Entity e, ScriptComponent& ScriptComponent, Args&... args) const
		{
			int nArgs = sizeof...(args);
			int returnValC = static_cast<int>(!std::is_same_v<ReturnType, void>);

			//TODO support invoke within a "table"
			lua_getglobal(ScriptComponent.owner, "Entities");

			auto scriptOwnerId = entt::to_integral(e.GetID());
			lua_geti(ScriptComponent.owner, -1, scriptOwnerId);
			lua_getfield(ScriptComponent.owner, -1, m_FunctionName.c_str());

			lua_remove(ScriptComponent.owner, -2);
			lua_remove(ScriptComponent.owner, -2);


			if (!lua_isfunction(ScriptComponent.owner, -1))
			{
				printf("Failed to find '%s' as a lua function \n", m_FunctionName.c_str());
				return ReturnType();
			}

			if constexpr (sizeof...(Args) != 0)
			{
				(LuaInspect::LuaPushProperty(ScriptComponent.owner, args...));
			}

			if (lua_pcall(ScriptComponent.owner, nArgs, returnValC, 0) != LUA_OK) {
				const char* errorMessage = lua_tostring(ScriptComponent.owner, -1);
				printf("[LUA] ScriptComponent threw an Error calling '%s':: %s\n", m_FunctionName.c_str(), errorMessage);

				int stackSize = lua_gettop(ScriptComponent.owner);
				lua_pop(ScriptComponent.owner, stackSize);

				return ReturnType();
			}

			if constexpr (!std::is_same_v<ReturnType, void>)
			{
				entt::meta_type returnType = entt::resolve<ReturnType>();
				entt::meta_any returnedValue{};

				auto t = returnType.func(p_LuaPull).invoke({}, entt::forward_as_meta(ScriptComponent.owner), -1, &returnedValue);
				if (!t)
				{
					printf("Failed to get returned value\n");
					return {};
				}

				lua_settop(ScriptComponent.owner, 0);
				return returnedValue.cast<ReturnType>();
			}
			else
			{
				lua_settop(ScriptComponent.owner, 0);
				//Suppose we return a void() here;
				return ReturnType();
			}
		}

	private:
		const std::string m_FunctionName;
	};
}