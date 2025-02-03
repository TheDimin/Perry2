#pragma once

namespace Perry
{
	class ToolBase
	{
		friend class ToolManager;

	public:
		ToolBase();
		ToolBase(const char* category) : Category(category){};
		virtual ~ToolBase() = default;

	protected:
		virtual void Init(){};
		virtual void Draw(){};
		bool IsOpen = false;
		const char* Category = "UNDEFINED\n";

		//By default the ToolManager calls begin/end for you. set this to false to cancel that.
		bool DrawDefaultWindow = true;

	private:
		// This is automatically filled out by generator
		const char* ToolName = "";
		REFLECT();
	};
} // namespace Perry