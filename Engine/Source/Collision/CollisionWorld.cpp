#include "EnginePCH.h"



#include "ECS/Systems/MetaReflectSystem.h"

#include <EnttEditor/MetaReflectImplement.h>

using namespace Perry;

static int checkCount = 0;

//#define COLLISION_DRAW_DEBUG

#ifdef COLLISION_DRAW_DEBUG
static int avgcheckCount = 0;

static int  avgColliders = 0;
#endif

#ifdef ENGINE_DEPRECATED
IMPLEMENT_REFLECT_SYSTEM(CollisionWorld)
{
	meta.data<&CollisionWorld::grid>("M_GRID"_hs)
		PROP_DISPLAYNAME("Grid");

	meta.data<&CollisionWorld::CollisionMatrix>("M_Layers"_hs)
		PROP_DISPLAYNAME("Layers");

	meta.data<&CollisionWorld::CellSize>("M_CellSize"_hs)
		PROP_DISPLAYNAME("CellSize");

	meta.data<&CollisionWorld::m_Skin>("M_Skin"_hs)
		PROP_DISPLAYNAME("Skin")
		PROP_DESCRIPTION("Added offset for collision resolving so colliders don't clip into each other after resolving the collision.");
}
FINISH_REFLECT();

void CollisionWorld::Init(entt::registry& reg)
{
	RunInEditorMode = true;//Just for debug drawing...
}

void CollisionWorld::Update(entt::registry& reg)
{
	{
		auto group = reg.view<CollisionComponent,const TransformComponent>();

		// Clear collision mask
		grid.clear();

		for (auto&& [entity, Collider, Transform] : group.each())
		{
			glm::ivec2 cells = ceil(Collider.m_Size / glm::vec2(CellSize * 0.5f));
			glm::vec3 pos = Transform::GetPosition(Transform) + glm::vec3(Collider.m_Offset.x, 0, Collider.m_Offset.y);
			glm::vec3 minP = pos - glm::vec3(Collider.m_Size.x, 0, Collider.m_Size.y);

			Collider.centerPoint = glm::vec2(pos.x, pos.z);

			glm::vec3 stepSize = glm::vec3(Collider.m_Size * 2.f, 0) / glm::vec3(cells, 1);
			for (int x = 0; x <= cells.x; ++x)
			{
				for (int y = 0; y <= cells.y; ++y)
				{
					glm::vec3 pp = minP + glm::vec3(x * stepSize.x, 0, y * stepSize.y);
					auto& childCell = GetCell(pp);

					// Sanity check, Don't insert the entity id multiple times into same cell
					if (!(std::find(childCell.ids.begin(), childCell.ids.end(), &Collider) == childCell.ids.end()))
						continue;

					childCell.ids.emplace_back(&Collider);
				}
			}
#ifdef COLLISION_DRAW_DEBUG
			Debug::DrawBox(pos, glm::vec3(Collider.m_Size.x, 0.f, Collider.m_Size.y));
#endif

		}
#ifdef COLLISION_DRAW_DEBUG
		for (auto&& [id, cell] : grid)
		{
			sDrawBox(glm::vec3(id.x, -.25, id.y), glm::vec3(CellSize), glm::vec3(0, 0, 1));
		}
#endif
	}
	checkCount = 0;

	for (auto&& [id, cell] : grid)
	{
		auto& cc = cell.ids;
		for (int i = 0; i < cc.size(); ++i)
		{
			CollisionComponent* firstCollider = cc[i];
			CollisionLayer FirstCollisionLayerMap =
				CollisionMatrix.find(firstCollider->m_CollisionLayer)->second;
			for (int x = i + 1; x < cc.size(); ++x)
			{
				checkCount++;
				CollisionComponent* secondCollider = cc[x];

				CollisionLayer SecondCollisionLayerMap =
					CollisionMatrix.find(secondCollider->m_CollisionLayer)->second;
				if ((FirstCollisionLayerMap & secondCollider->m_CollisionLayer) == 0 &&
					(SecondCollisionLayerMap & firstCollider->m_CollisionLayer) == 0)
					continue;

				float overlap = 0.0f;
				glm::vec2 normal;
				if (AABB(*firstCollider, *secondCollider, normal, overlap))
				{
					glm::vec3 position = {firstCollider->centerPoint.x, 0.0f, firstCollider->centerPoint.y};

					// Perry::Renderer::DrawDebugLine(
					//	position,
					//	position + glm::vec3(normal.x, 0.0f, normal.y),
					//	{ 1.0f, 0.0f, 1.0f });

					Debug::DrawLine(
						position, position + (glm::vec3(normal.x, 0.0f, normal.y) * overlap), {0.0f, 1.0f, 1.0f});

					Collision c;

					// add collided object
					// TODO change this Pointers move now!!!

					CollisionPair pair;
					pair = CollisionPair{firstCollider, secondCollider};

					cell.pairs.emplace_back(pair);
					c.pair = pair;
					c.overlap = overlap;
					c.normal = normal;
					m_Collisions.push_back(c);
				}
			}
		}
	}

	std::set<CollisionPair> collisionsThisFrame{};
	for (auto&& [id, cell] : grid)
	{
		for (auto& collisionPair : cell.pairs)
		{
			collisionsThisFrame.insert(collisionPair);
			if (m_CollisionPairsLastFrame.find(collisionPair) == m_CollisionPairsLastFrame.end())
			{
				if (!collisionPair.first->m_OnCollideStartSignal.empty())
					collisionPair.first->m_OnCollideStartSignal.publish(*collisionPair.first, *collisionPair.second);

				if (!collisionPair.second->m_OnCollideStartSignal.empty())
					collisionPair.second->m_OnCollideStartSignal.publish(*collisionPair.second, *collisionPair.first);
			}

			if (!collisionPair.first->m_OnCollideSignal.empty())
				collisionPair.first->m_OnCollideSignal.publish(*collisionPair.first, *collisionPair.second);

			if (!collisionPair.second->m_OnCollideSignal.empty())
				collisionPair.second->m_OnCollideSignal.publish(*collisionPair.second, *collisionPair.first);
		}
	}

	for (auto& it : m_CollisionPairsLastFrame)
	{
		if (collisionsThisFrame.find(it) == collisionsThisFrame.end())
		{
			if (!it.first->m_OnCollideEndSignal.empty())
			{
				it.first->m_OnCollideEndSignal.publish(*it.first, *it.second);
			}

			if (!it.second->m_OnCollideEndSignal.empty())
			{
				it.second->m_OnCollideEndSignal.publish(*it.second, *it.first);
			}
		}
	}

	for (auto& collision : m_Collisions)
	{
		CollisionComponent* first = collision.pair.first;
		CollisionComponent* second = collision.pair.second;

		if (first->m_TriggerBox || second->m_TriggerBox)
		{
			continue;
		}

		glm::vec3 push = glm::vec3(collision.normal.x, 0.0f, collision.normal.y) * (collision.overlap + m_Skin);

		// @Note(Jesper): Getting the velocity component every time we resolve a collision for a non-static collider is
		// ugly. Maybe we can optimize it by caching the velocity component, But it doesnt' matter that much.
		if (!first->m_IsStatic)
		{
			auto* v = reg.try_get<VelocityComponent>(Perry::to_entity(reg, *first));
			if (v)
			{
				v->m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			auto* t = reg.try_get<TransformComponent>(Perry::to_entity(reg, *first));

			assert(t && "First entity does not have a transform to resolve collision.");

			Transform::SetPosition(*t, Transform::GetPosition(*t) - push);
		}

		if (!second->m_IsStatic)
		{
			auto* v = reg.try_get<VelocityComponent>(Perry::to_entity(reg, *second));

			if (v)
			{
				v->m_Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			auto* t = reg.try_get<TransformComponent>(Perry::to_entity(reg, *second));

			assert(t && "Second entity does not have a transform to resolve collision.");

			Transform::SetPosition(*t, Transform::GetPosition(*t) + push);
		}
	}

	m_Collisions.clear();

	m_CollisionPairsLastFrame = collisionsThisFrame;

#ifdef COLLISION_DRAW_DEBUG
	auto group = reg.group<CollisionComponent>(entt::get<const TransformComponent>);
	avgColliders = static_cast<int>((avgColliders + static_cast<int>(group.size())) / 2.f);
	avgcheckCount = static_cast<int>((avgcheckCount + checkCount) / 2.f);

	printf("Frame: Colliders [%i] Checks [%i]\n", avgColliders, avgcheckCount);
#endif
}
GraphInfo CollisionWorld::GetGraphBinding()
{
	//TODO: we can move some code to a seperate system allowing collision proccesing to be done with read-only velocity & transform
	return GraphInfo{{},{TYPE_TO_ID(TransformComponent),TYPE_TO_ID(VelocityComponent),TYPE_TO_ID(CollisionComponent)},true};
}

bool CollisionWorld::AABB(const CollisionComponent& a, const CollisionComponent& b, glm::vec2& out_normal, float& out_overlap) const
{
	// get a and b position
	glm::vec2 aPosition = a.centerPoint;
	glm::vec2 bPosition = b.centerPoint;

	//compare values
	glm::vec2 aMin, aMax, bMin, bMax;

	aMin = aPosition - a.m_Size / 2.0f;
	aMax = aPosition + a.m_Size / 2.0f;
	bMin = bPosition - b.m_Size / 2.0f;
	bMax = bPosition + b.m_Size / 2.0f;

	float overlapOnAxis[] = {
		bMax.x - aMin.x,
		aMax.x - bMin.x,
		bMax.y - aMin.y,
		aMax.y - bMin.y,
	};

	float overlap = FLT_MAX;
	glm::vec2 normal = glm::vec2(0.0f);

	for (size_t i = 0; i < 4; i++) {
		if (overlapOnAxis[i] < overlap) {
			overlap = overlapOnAxis[i];
			normal = AABB_NORMALS[i];
		}
	}

	out_normal = normal;
	out_overlap = overlap / 2.0f;

	return
		(
			aMin.x <= bMax.x &&
			aMax.x >= bMin.x &&
			aMin.y <= bMax.y &&
			aMax.y >= bMin.y
			);
}

CollisionWorld::Cell& CollisionWorld::GetCell(const glm::vec3& p)
{
	auto pt = p + (glm::vec3(CellSize / 2.f) * glm::vec3(p.x > 0 ? 1 : -1, 0, p.z > 0 ? 1 : -1));
	glm::ivec2 id = glm::ivec2(pt.x / CellSize, pt.z / CellSize);

	if (grid.find(id) == grid.end())
	{
		grid.emplace(std::pair<glm::ivec2, CollisionWorld::Cell>{ id, {} });
	}

	return grid.find(id)->second;
}
#endif