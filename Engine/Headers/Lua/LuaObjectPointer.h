#pragma once

struct lua_State;

//MetaTable for any lua class
class LuaObjectPointerV2
{
public:
	//Simple setup part that gets the Global Class table and Calls the create new type (Handeld in lua)
	//We Pass in self,TheClassName,And the callback function (to register functions/metatable stuff)
	static void RegisterType(lua_State* luaState, const entt::meta_type& type);

	/// <summary>
	/// Create a new LuaObject for a existing object which will be placed on top of the stack
	/// </summary>
	/// <param name="luaState"></param>
	/// <param name="object"></param>
	///	<param name="copyObject"></param>
	static void New(lua_State* luaState, entt::meta_any& object, bool copyObject = false);

	/// <summary>
	/// Create a new LuaObject for a existing object which will become avaialbe globally (By name)
	/// </summary>
	/// <param name="luaState"></param>
	///	<param name="globalName"> The name that will be used for this type In Global Table</param>
	/// <param name="object"></param>
	static void NewGlobal(lua_State* luaState, const std::string& globalName, entt::meta_any&& object);

private:
	friend class LuaObjectTemplate;
	/// <summary>
	/// This will initialize the object in lua, We expect the UserData to be on top of the stack !!!! before calling this
	/// </summary>
	/// <param name="luaState"></param>
	/// <param name="className">Name of the class we push Used for reflection</param>
	static void CreateLuaObject(lua_State* luaState, const std::string& className);

public:
	static std::vector<entt::meta_any> NewIn(lua_State* luaState, int startIndex = 1);
};