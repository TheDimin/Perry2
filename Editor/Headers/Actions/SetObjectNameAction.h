#pragma once

namespace Perry::Editor::Actions
{
	class SetObjectNameAction final : public Perry::Editor::IAction
	{
	public:
		void Execute() override;
		REFLECT();
	};
	
} // namespace Perry::Editor::Actions
