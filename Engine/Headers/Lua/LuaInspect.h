#pragma once


class LuaInspect
{
public:
	template<class Type>
	static inline auto CallFunction(lua_State* luaState, Type&& value)
	{
		entt::meta_any valueMetaAny = entt::meta_any(value);
		return CallFunction(luaState, valueMetaAny);
	}

	static void CallFunction(lua_State* luaState, const entt::meta_any& value);

	template<typename Type>
	static inline auto LuaPushProperty(lua_State* luaState, const Type& value)
	{
		if constexpr (std::is_same_v<Type, float> || std::is_same_v<Type, double> || std::is_same_v<Type, int>)
		{
			lua_pushnumber(luaState, static_cast<lua_Number>(value));
		}
		else if constexpr (std::is_integral_v<Type>)
		{
			lua_pushinteger(luaState, static_cast<lua_Integer>(value));
		}
		else if constexpr (std::is_same_v<Type, bool>)
		{
			lua_pushboolean(luaState, value);
		}
		else if constexpr (std::is_same_v<Type, const char*> || std::is_same_v<Type, std::string>)
		{
			lua_pushstring(luaState, value.c_str());
		}
		else if constexpr (std::is_pointer_v<Type>)
		{
			entt::meta_any valueM = entt::forward_as_meta(const_cast<Type&>(value));
			LuaObjectPointerV2::New(luaState, valueM);
		}
		else
		{
			entt::meta_any valueM = entt::forward_as_meta(const_cast<Type&>(value));
			LuaObjectPointerV2::New(luaState, valueM, true);
		}
	}

	static entt::meta_any LuaPullProperty(lua_State* luaState, int index)
	{
		if (lua_isnumber(luaState, index))
		{
			// TODO make double convert to anytype...
			return entt::forward_as_meta(lua_tonumber(luaState, index));
		}
		if (lua_isstring(luaState, index))
		{
			return entt::forward_as_meta(static_cast<std::string>(lua_tostring(luaState, index)));
		}

		printf("[LUA_C] ERROR: Not implemented conversion for value at index %d with type '%s'\n",
			index,
			lua_typename(luaState, lua_type(luaState, index)));
		return {};
	}

	template<typename T>
	static entt::meta_any PullObject(lua_State* luaState, int index)
	{
		// TODO handle it being a different type...
		lua_getfield(luaState, index, "_pointer");
		T& myuserdata = *static_cast<T*>(lua_touserdata(luaState, -1));
		lua_pop(luaState, 1);
		return entt::forward_as_meta(myuserdata);
	}

	template<typename T>
	static entt::meta_any PullObjectFromUserData(lua_State* luaState, int index)
	{
		T& myuserdata = *static_cast<T*>(lua_touserdata(luaState, index));
		lua_pop(luaState, 1);
		return entt::forward_as_meta(myuserdata);
	}

	template<typename Type>
	static inline void LuaPullProperty(lua_State* luaState, int index, entt::meta_any* ReturnValue)
	{
		if constexpr (std::is_same_v<Type, float>)
		{
			if (lua_isnumber(luaState, index))
			{
				Type v = static_cast<Type>(lua_tonumber(luaState, index));
				*ReturnValue = entt::forward_as_meta(v);
				return;
			}
		}
		else if constexpr (std::is_same_v<Type, char*> || std::is_same_v<Type, const char*> ||
			std::is_same_v<Type, std::string>)
		{
			if (lua_isstring(luaState, index))
			{
				*ReturnValue = static_cast<Type>(lua_tostring(luaState, index));
				return;
			}
			else if (lua_isuserdata(luaState, index))
			{
				*ReturnValue = PullObjectFromUserData<Type>(luaState, index);
				return;
			}
		}
		else if constexpr (std::is_same_v<Type, bool>)
		{
			if (lua_isboolean(luaState, index))
			{
				*ReturnValue = entt::forward_as_meta(static_cast<bool>(lua_toboolean(luaState, index)));
				return;
			}
		}
		else if constexpr (std::is_same_v<Type, void*> || std::is_pointer_v<Type> || std::is_object_v<Type>)
		{
			if (lua_isuserdata(luaState, index))
			{
				*ReturnValue = PullObjectFromUserData<Type>(luaState, index);
				return;
			}
			else if (lua_istable(luaState, index))
			{
				*ReturnValue = PullObject<Type>(luaState, index);
				return;
			}
		}
		else if constexpr (std::is_same_v<Type, lua_CFunction>)
		{
			if (lua_iscfunction(luaState, index))
			{
				// return static_cast<Type>(lua_tocfunction(luaState, index));
				printf("TODO\n");
				return;
			}
		}
		else
		{
			if (lua_istable(luaState, index))
			{
				*ReturnValue = PullObject<Type>(luaState, index);
				return;
			}
		}

		printf("[LUA_C] ERROR: Failed to parse property at index %d as %s. Expected type: %s\n",
			index,
			luaL_typename(luaState, index),
			typeid(Type).name());
	}
};