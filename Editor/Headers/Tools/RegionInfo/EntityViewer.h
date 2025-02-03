#pragma once

namespace Perry {
	class HierarchyComponent;
	class EntityViewer
	{
		friend class RegionInfoTool;
	public:
		EntityViewer();
	protected:
		void Draw(entt::registry& reg);

	private:
		void DrawEnttFilterd(entt::registry& reg, entt::entity e, const std::string& nameFilter, const std::vector<entt::meta_type>& components);
		void DrawEnttRecursive(entt::registry& reg, entt::entity e, Perry::HierarchyComponent* h);
		void GenerateComponentList(Perry::Entity& e);

		std::vector<unsigned int> SelectedEntityComponents{};
		Perry::Entity SelectedEntity = {};
		std::string ComponentSearch = "";
	};
}
