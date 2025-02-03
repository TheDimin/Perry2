#pragma once

namespace Perry::Editor::Actions
{
	class CreateObject final : public Perry::Editor::IAction
	{
	public:

		void Execute() override;
		void RollBack() override;
		REFLECT();
	};
}
