#pragma once

namespace Perry
{
	class ImguiDemoTool : public ToolBase
	{
	public:
		ImguiDemoTool();

	protected:
		void Draw() override;
		
		REFLECT();
	};

} // namespace Perry