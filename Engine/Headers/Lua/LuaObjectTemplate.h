#pragma once
#include <EnttEditor/MetaDefines.h>

#include <entt/entt.hpp>
#include <stdio.h>
extern "C" {
#include "Lua/lua.h"
#include "Lua/Lualib.h"
#include "Lua/lstate.h"
#include "Lua/lauxlib.h"
}

#include "LuaObjectPointer.h"

#include "LuaInspect.h"
//Templated information about the object,
//Generally Do not use this in .h files !
class LuaObjectTemplate
{
public:
	template<typename T, typename ...args>
	static void New(lua_State* lua_state, args...);

	template<typename T>
	static void TLinker(lua_State* luaState)
	{
		lua_pushcfunction(luaState, &RegisterTypeCallback<T>);
	}

private:
	friend class LuaObjectPointerV2;


	//This gets called when Class Type has been created
	//Stack will Contain the TypeTable, Now you can add functions to it!
	template<typename T>
	static int RegisterTypeCallback(lua_State* luaState)
	{
		entt::meta_type typeInfo = entt::resolve<T>();

		lua_getfield(luaState, -1, "memberFunctions");
		lua_getfield(luaState, -2, "staticFunctions");
		for (auto&& [funcID, func] : typeInfo.func())
		{
			if (!func.prop(p_Scriptable)) continue;

			auto funcName = func.prop(p_DisplayName);
			if (!funcName)
			{
				printf("[LUAC] Class '%s' contains a function that has no name, Failed to expose it to lua \n", typeInfo.info().name().data());
			}

			lua_pushstring(luaState, funcName.value().cast<std::string>().c_str());
			lua_pushnumber(luaState, funcID);

			if (func.is_static())
			{
				lua_pushcclosure(luaState, &LUA_StaticFunctionInvoke<T>, 1);
				lua_settable(luaState, -3);
			}
			else
			{
				lua_pushcclosure(luaState, &LUA_MemberFunctionInvoke<T>, 1);
				lua_settable(luaState, -4);
			}
		}
		lua_pop(luaState, 2);


		lua_getfield(luaState, -1, "variableGet");
		lua_getfield(luaState, -2, "variableSet");
		for (auto&& [memberID, member] : typeInfo.data())
		{
			const auto memberNameProp = member.prop(p_DisplayName);
			if (!memberNameProp)
			{
				printf("[LUAC] Class '%s' contains a member that has no name, Failed to expose it to lua \n", typeInfo.info().name().data());
			}

			std::string memberName = memberNameProp.value().cast<std::string>();

			{
				lua_pushstring(luaState, memberName.c_str());
				lua_pushnumber(luaState, memberID);

				lua_pushcclosure(luaState, &LUA_MemberGet<T>, 1);
				lua_settable(luaState, -4);
			}

			{
				lua_pushstring(luaState, memberName.c_str());
				lua_pushnumber(luaState, memberID);

				lua_pushcclosure(luaState, &LUA_MemberSet<T>, 1);
				lua_settable(luaState, -3);
			}

		}

		//Pop get and set field of stack
		lua_pop(luaState, 2);
		RegisterOperators<T>(luaState);

		return 0;
	}


	template<typename T>
	static int LUA_MemberFunctionInvoke(lua_State* luaState)
	{
		entt::meta_type type = entt::resolve<T>();
		entt::meta_any returnedValue{};

		if (!type.func(p_LuaPull))
		{
			printf("Type does not contain p_LuaPull \n");
			return 0;
		}

		if (!type.func(p_LuaPull).invoke({}, entt::forward_as_meta(luaState), 1, &returnedValue))
		{
			printf("Failed to execute invoke\n");
			return -1;
		}

		//In the closure we saved FunctionID based on the entt type
		auto f = static_cast<entt::id_type>(lua_tonumber(luaState, lua_upvalueindex(1)));
		auto funcToInvoke = type.func(f);

		return InvokeGeneric(luaState, funcToInvoke, returnedValue);
	}

	template<typename T>
	static int LUA_StaticFunctionInvoke(lua_State* luaState)
	{
		//In the closure we saved FunctionID based on the entt type
		auto f = static_cast<entt::id_type>(lua_tonumber(luaState, lua_upvalueindex(1)));
		auto funcToInvoke = entt::resolve<T>().func(f);
		auto met = entt::meta_any();
		return InvokeGeneric(luaState, funcToInvoke, met);
	}

	static int InvokeGeneric(lua_State* luaState, entt::meta_func& funcToInvoke, entt::meta_any& caller)
	{
		auto params = LuaObjectPointerV2::NewIn(luaState, 1);

		assert(funcToInvoke.arity() == params.size());

		entt::meta_any rt;
		//if (funcToInvoke.is_const())
	//		rt = funcToInvoke.invoke(caller);
		//else
		rt = funcToInvoke.invoke(caller, params.data(), params.size());

		//We need to check if the data itself is not a nullptr in case the function returned void
		if (rt && rt.data() != nullptr)
		{
			auto f = rt.type().func(p_LuaPush);
			auto returnedValue = f.invoke({}, entt::forward_as_meta(luaState), rt);

			if (!returnedValue)
			{
				printf("failed to return value to lua...\n");
				return 0;
			}

			return 1;
		}
		else
		{
			if (!rt.data())
			{
				printf("Invoke Generic [ReturnValue] failed to call function \n");
				printf("ParameterDump Inc:\n");
				for (int i = 0; i < params.size() && i < funcToInvoke.arity(); ++i)
				{
					printf("Paramter[%i] is Input with type [%s] Expected [%s]", i, params[i].type().info().name().data(), funcToInvoke.arg(i).info().name().data());
				}
			}
		}

		return 0;
	}


	template<typename T>
	static int LUA_MemberGet(lua_State* luaState)
	{
		entt::meta_type type = entt::resolve<T>();
		entt::meta_any returnedValue{};

		if (!type.func(p_LuaPull).invoke({}, entt::forward_as_meta(luaState), 1, &returnedValue))
		{
			printf("Failed to execute invoke\n");
			return -1;
		}

		//In the closure we saved MemberID based on the entt type
		auto pId = static_cast<entt::id_type>(lua_tonumber(luaState, lua_upvalueindex(1)));

		auto objectProp = returnedValue.get(pId);

		auto pushedProp = objectProp.type().func(p_LuaPush).invoke({}, entt::forward_as_meta(luaState), objectProp);

		if (!pushedProp)
		{
			printf("[LUA_C] ERROR: Failed to push data of type %s\n", objectProp.type().info().name().data());

			return 0;
		}

		return 1;
	}


	template<typename T>
	static int LUA_MemberSet(lua_State* luaState)
	{
		entt::meta_type type = entt::resolve<T>();
		entt::meta_any returnedValue{};

		if (!type.func(p_LuaPull).invoke({}, entt::forward_as_meta(luaState), 1, &returnedValue))
		{
			printf("Failed to execute invoke\n");
			return -1;
		}

		entt::meta_any t = LuaInspect::LuaPullProperty(luaState, 2);

		//In the closure we saved MemberID based on the entt type
		auto pId = static_cast<entt::id_type>(lua_tonumber(luaState, lua_upvalueindex(1)));

		[[maybe_unused]] auto objectProp = returnedValue.set(pId, t);


		return 0;
	}

	template<typename T>
	struct has_multiplication_operator
	{
		template<typename U>
		static auto test(int) -> decltype(std::declval<U>()* std::declval<U>(), void(), std::true_type());
		template<typename>
		static auto test(...) -> std::false_type;

		static constexpr bool value = decltype(test<T>(0))::value;
	};

	template<typename T>
	struct has_addition_operator
	{
		template<typename U>
		static auto test(int) -> decltype(std::declval<U>() + std::declval<U>(), void(), std::true_type());
		template<typename>
		static auto test(...) -> std::false_type;

		static constexpr bool value = decltype(test<T>(0))::value;
	};
	template<typename T>
	struct has_sub_operator
	{
		template<typename U>
		static auto test(int) -> decltype(std::declval<U>() - std::declval<U>(), void(), std::true_type());
		template<typename>
		static auto test(...) -> std::false_type;

		static constexpr bool value = decltype(test<T>(0))::value;
	};
	template<typename T>
	struct has_div_operator
	{
		template<typename U>
		static auto test(int) -> decltype(std::declval<U>() / std::declval<U>(), void(), std::true_type());
		template<typename>
		static auto test(...) -> std::false_type;

		static constexpr bool value = decltype(test<T>(0))::value;
	};

	template<typename T>
	static void RegisterOperators(lua_State* luaState)
	{
		lua_getfield(luaState, -1, "instanceMetaTable");

		if constexpr (has_multiplication_operator<T>::value)
		{
			lua_pushstring(luaState, "__mul");
			lua_pushcfunction(luaState, &Mul<T>);
			lua_settable(luaState, -3);
		}

		if constexpr (has_addition_operator<T>::value)
		{
			lua_pushstring(luaState, "__add");
			lua_pushcfunction(luaState, &Add<T>);
			lua_settable(luaState, -3);
		}

		if constexpr (has_sub_operator<T>::value)
		{
			lua_pushstring(luaState, "__sub");
			lua_pushcfunction(luaState, &Sub<T>);
			lua_settable(luaState, -3);
		}

		if constexpr (has_div_operator<T>::value)
		{
			lua_pushstring(luaState, "__div");
			lua_pushcfunction(luaState, &Div<T>);
			lua_settable(luaState, -3);
		}

		lua_pop(luaState, 1);

		lua_pushstring(luaState, "__call");
		lua_pushcfunction(luaState, &LuaNewV<T>);
		lua_settable(luaState, -3);
	}

	template<typename T>
	static int Mul(lua_State* luaState)
	{
		entt::meta_any returnValueA, returnValueB;
		LuaInspect::LuaPullProperty<T>(luaState, 1, &returnValueA);
		LuaInspect::LuaPullProperty<T>(luaState, 2, &returnValueB);

		assert(returnValueA.allow_cast<T>());
		const T& a = returnValueA.cast<T>();

		assert(returnValueB.allow_cast<T>());
		const T& b = returnValueB.cast<T>();

		const T v = a * b;
		LuaInspect::LuaPushProperty<T>(luaState, v);

		return 1;
	}

	template<typename T>
	static int Add(lua_State* luaState)
	{
		entt::meta_any returnValueA, returnValueB;
		LuaInspect::LuaPullProperty<T>(luaState, 1, &returnValueA);
		LuaInspect::LuaPullProperty<T>(luaState, 2, &returnValueB);

		//		assert(returnValueA.allow_cast<T>());
		const T& a = returnValueA.cast<T>();

		//	assert(returnValueB.allow_cast<T>());
		const T& b = returnValueB.cast<T>();

		const T v = a + b;
		LuaInspect::LuaPushProperty<T>(luaState, v);

		return 1;
	}

	template<typename T>
	static int Sub(lua_State* luaState)
	{
		entt::meta_any returnValueA, returnValueB;
		LuaInspect::LuaPullProperty<T>(luaState, 1, &returnValueA);
		LuaInspect::LuaPullProperty<T>(luaState, 2, &returnValueB);

		assert(returnValueA.allow_cast<T>());
		const T& a = returnValueA.cast<T>();

		assert(returnValueB.allow_cast<T>());
		const T& b = returnValueB.cast<T>();

		const T v = a - b;
		LuaInspect::LuaPushProperty<T>(luaState, v);

		return 1;
	}
	template<typename T>
	static int Div(lua_State* luaState)
	{
		entt::meta_any returnValueA, returnValueB;
		LuaInspect::LuaPullProperty<T>(luaState, 1, &returnValueA);
		LuaInspect::LuaPullProperty<T>(luaState, 2, &returnValueB);

		assert(returnValueA.allow_cast<T>());
		const T& a = returnValueA.cast<T>();

		assert(returnValueB.allow_cast<T>());
		const T& b = returnValueB.cast<T>();

		//#pragma warning( push )
		//#pragma warning( disable : 4804) //Yeah this type is templated I don't wanna deal with division warnings...
		const T v = a / b;
		//#pragma warning (pop)
		LuaInspect::LuaPushProperty<T>(luaState, v);

		return 1;
	}

	template<typename T>
	static int LuaNewV(lua_State* luaState)
	{
		auto params = LuaObjectPointerV2::NewIn(luaState, 1);

		auto l = entt::resolve<T>().construct(params.data(), params.size());

		if (!l)
		{
			printf("Failed to call New with given parameters...");
			return 0;
		}

		LuaObjectPointerV2::New(luaState, l, true);
		return 1;
	}
};

template <typename T, typename ... args>
void LuaObjectTemplate::New(lua_State* lua_state, args... arg)
{
	T* newObject = (T*)lua_newuserdata(lua_state, sizeof(T));
	newObject = T(arg...);

	const entt::meta_any metaObject = entt::forward_as_meta(*newObject);
	const auto typeName = metaObject.type().prop(p_DisplayName).value().cast<std::string>();

	LuaObjectPointerV2::CreateLuaObject(lua_state, typeName);
}