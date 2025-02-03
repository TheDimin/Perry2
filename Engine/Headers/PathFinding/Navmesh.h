#pragma once
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "VectorHash.h"
#include "entt/entity/registry.hpp"
namespace Perry {

	class CollisionWorld;

	struct node {
		glm::ivec2 pos;
		node* neighbours[4];  // N E S W
		bool blocked = false;
	};

	class NavMesh {
	public:
		NavMesh(entt::registry* reg) : registry(reg) {}
		void Generate(glm::ivec2 worldSize, glm::vec2 nodeSize);

		void Imgui();
		void Update();

		/// <summary>
		/// Update a already calculated path
		/// </summary>
		/// <param name="startPos"></param>
		/// <param name="endPos"></param>
		/// <returns></returns>
		std::vector<const node*> CalculatePath(glm::vec3 startPos, glm::vec3 endPos);
		glm::vec2 GetNodeSize() const;

		node* FindClosestNode(glm::vec3 pos);

	private:
		void SetNeighbours();

		/// <summary>
		/// Algorithm for finding path
		/// </summary>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <returns></returns>
		std::vector<const node*> BreadthFirstSearch(node* start, node* end) const;

		std::vector<node> m_nodes;
		glm::vec2 m_NodeSize;
		glm::ivec2 m_WorldSize;
		entt::registry* registry;
	};
}  // namespace Perry