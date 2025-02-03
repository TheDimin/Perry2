#pragma once

namespace Perry
{

	class ScriptComponent
	{
	public:
		ScriptComponent() = default;
		ScriptComponent(const std::string& path) :scriptFile(path) {};

		std::string scriptFile = "";
		//TODO it would be nice if we could get rid of this..
		lua_State* owner = nullptr;
		bool valid = true;

		REFLECT()
	};


	//TODO: LuaSystem should be a complete sync point for the execution graph...
	class LuaSystem : public Perry::System
	{
	public:
		~LuaSystem() override;
		LuaSystem* instance = nullptr;
		static bool LoadFile(lua_State* state, const std::string& fileName, int nargs = 0, int nresults = 0, int msgh = 0);
		lua_State* luaState = nullptr;

		static void PrintTable(lua_State* luaState);
		static void PrintStack(lua_State* luaState);
		static void PrintDebugInfo(lua_State* luaState);
		static void PrintTraceBack(lua_State* luaState);
	protected:
		void OnScriptComponentCreated(entt::registry& reg, entt::entity e);
		void Init(entt::registry& reg) override;

		void Update(entt::registry& reg) override;
		

		std::string defaultFile = "main.lua";

		REFLECT()
	};
}
