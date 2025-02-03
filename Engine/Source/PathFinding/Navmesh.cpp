#include "EnginePCH.h"
#pragma hdrstop

#include "PathFinding/Navmesh.h"

#include "Debug.h"

#include <imgui.h>
#include <map>
#include <set>
#include <string>

#include "Collision/CollisionWorld.h"
#include "Engine.h"
#include "Graphics/Renderer.h"
#include "Transform.h"
#include "ECS/Components/CollisionComponent.h"
#include "ECS/Components/TransformComponent.h"

using namespace Perry;

void NavMesh::Generate(glm::ivec2 worldSize, glm::vec2 nodeSize)
{
	m_NodeSize = nodeSize;
	m_WorldSize = worldSize;

	m_nodes.resize(worldSize.x * worldSize.y);

	for (int x = 0; x < worldSize.x; x++)
		for (int y = 0; y < worldSize.y; y++)
		{
			m_nodes[x + y * worldSize.x] = { glm::vec2(x, y) * nodeSize };
		}

	//Set neighbours

	SetNeighbours();

	entt::registry& reg = *registry;

	//Clear collision mask
	for (auto&& [entity, collider, transform] : reg.group<CollisionComponent>(entt::get<const TransformComponent>).each())
	{
		//TODO we can return this group sorted , by layer
		//Then We can run this much faster
		if (collider.m_CollisionLayer != LAYER_STATIC)
			continue;

		glm::vec3 ps = Transform::GetPosition(transform);
		glm::vec3 cs = glm::vec3(collider.GetSize(), 0);

		//TODO also check inbetween by correct stepsize

		node* n1 = FindClosestNode(ps + glm::vec3(-cs.x, cs.y, 0));
		node* n2 = FindClosestNode(ps + glm::vec3(-cs.x, -cs.y, 0));
		node* n3 = FindClosestNode(ps + glm::vec3(cs.x, -cs.y, 0));
		node* n4 = FindClosestNode(ps + glm::vec3(cs.x, cs.y, 0));

		n1->blocked = true;
		n2->blocked = true;
		n3->blocked = true;
		n4->blocked = true;
	}
}

void DrawNode(node& n)
{
	void* nnn = &n;
	if (ImGui::TreeNode((std::string("ENTT: ##") + std::to_string(reinterpret_cast<unsigned long long>(nnn))).c_str()))
	{
		ImGui::Text(" [%i , %i] ", n.pos.x, n.pos.y);
		//{

		for (int i = 0; i < 4; ++i)
		{
			if (n.neighbours[i])
				DrawNode(*n.neighbours[i]);
			//}
			//
		}
		ImGui::TreePop();
	}
}

void NavMesh::Imgui()
{
	ImGui::Begin("Nody");
	DrawNode(m_nodes[0]);
	ImGui::End();
}

void NavMesh::Update()
{
	Imgui();

	glm::vec2 s = m_NodeSize - glm::vec2(0.1f);
	for (int i = 0; i < m_nodes.size(); ++i)
	{
		glm::vec3 c = glm::vec3(0.1f, 1.0f, 0.1f);

		if (m_nodes[i].blocked)
			c = glm::vec3(1, 0, 0);

		Debug::DrawBox(glm::vec3(m_nodes[i].pos, 0), glm::vec3(s, 0), c);
	}

	//if (t)
	//	DrawBox(t->m_Position, glm::vec3(f->GetSize() * 2.f, 0), glm::vec3(1, 1, 0));
}

std::vector<const node*>  NavMesh::CalculatePath(glm::vec3 startPos, glm::vec3 endPos)
{
	node* start = FindClosestNode(startPos);
	node* end = FindClosestNode(endPos);

	//Algorithm used for path gen
	return BreadthFirstSearch(start, end);
}

glm::vec2 NavMesh::GetNodeSize() const
{
	return m_NodeSize;
}

node* NavMesh::FindClosestNode(glm::vec3 pos)
{
	const glm::vec3 nodeSizeV3 = glm::vec3(m_NodeSize, 0);
	glm::ivec2 index = (pos + nodeSizeV3 * 0.5f) / nodeSizeV3;

	//index = glm::max(glm::ivec2(0), glm::min(index, m_WorldSize));

	return &m_nodes[index.x + index.y * m_WorldSize.x];
}


std::vector<const node*> NavMesh::BreadthFirstSearch(node* start, node* end) const
{
	std::vector<node*> queue = {};

	std::set<node*> explored{};
	std::map<node*, node*> pathy{};

	queue.push_back(start);

	while (!queue.empty())
	{
		node* c = queue[0];
		queue.erase(begin(queue));

		if (c == end)
		{
			std::vector<const node*> path{};
			node* last = end;
			while (last != start)
			{

				path.insert(begin(path), last);
				auto l = pathy.find(last);
				last = l->second;
			}

			path.insert(begin(path), start);

			return path;
		}

		for (int i = 0; i < 4; ++i)
		{
			if (!(c->neighbours[i]))continue;
			if (explored.find(c->neighbours[i]) != std::end(explored)) continue;

			if (c->neighbours[i]->blocked)continue;

			explored.insert(c->neighbours[i]);

			pathy.insert({ c->neighbours[i], c });

			queue.push_back(c->neighbours[i]);
		}
	}

	//Failed to find path
	return {};
}

void NavMesh::SetNeighbours()
{
	for (int x = 0; x < m_WorldSize.x; x++)
		for (int y = 0; y < m_WorldSize.y; y++)
		{
			auto& cn = m_nodes[x + y * m_WorldSize.x];
			if (y > 0)
			{
				cn.neighbours[2] = &m_nodes[x + (y - 1) * m_WorldSize.x];
			}
			if (y < m_WorldSize.y - 1)
				cn.neighbours[0] = &m_nodes[x + (y + 1) * m_WorldSize.x];


			if (x > 0)
				cn.neighbours[3] = &m_nodes[x - 1 + y * m_WorldSize.x];

			if (x < m_WorldSize.x - 1)
				cn.neighbours[1] = &m_nodes[x + 1 + y * m_WorldSize.x];
		}
}