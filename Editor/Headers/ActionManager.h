#pragma once

using entt::literals::operator ""_hs;


#define IMPLEMENT_REFLECT_ACTION(TYPE) \
	_REFLECT_INTERNAL(TYPE);\
	int TYPE::InitTypeReflect(){\
	[[maybe_unused]] auto meta = EnttEditor::ReflectObject<TYPE>((#TYPE));\
	meta.prop(Perry::Editor::T_ReflectedAction);\
	meta.template base<Perry::Editor::IAction>();

namespace Perry::Editor
{
	constexpr entt::hashed_string T_ReflectedAction = "T_Editor_ReflectedAction"_hs;
	class IAction
	{
	public:
		IAction() {};
		virtual ~IAction() = default;
		virtual void Execute() {}
		virtual void RollBack() {}


	


		REFLECT()
	};

	class ActionManager
	{
	public:
		ActionManager();
		IAction* Create();
		void Draw();


		std::vector<entt::meta_any> ActivatedActions{};

		std::vector<entt::meta_type> ActionTypes{};
		bool Active = false;
	};
}