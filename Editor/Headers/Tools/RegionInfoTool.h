#pragma once

namespace Perry
{
	class Region;

	class RegionInfoTool : public ToolBase
	{
	public:
		RegionInfoTool() { IsOpen = false; };

		static inline Region* SelectedRegion = nullptr;
	protected:
		void Draw() override;

		void DrawRegionBox(const Region& region);
		void DrawRegionSystems(const Region& region);

		float Zoom = 1;
		glm::vec2 Offset{ 100, 100 };

		EntityViewer viewer{};

		REFLECT();
	};

} // namespace Perry