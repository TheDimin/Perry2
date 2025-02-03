#pragma once
#include "Level.h"
#include "Navmesh.h"

namespace Perry {
	class PathFindingDemoLevel : public Level {
	public:
		void OnLoad() override;
		void Update(float deltaTime) override;

	private:
		NavMesh nm;
		std::vector<const node*> m_path;
	};
}  // namespace Perry