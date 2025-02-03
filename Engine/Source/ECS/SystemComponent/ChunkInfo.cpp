#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_COMPONENT(ChunkInfoComponent)
{

}
FINISH_REFLECT()

IMPLEMENT_REFLECT_SYSTEM(ChunkInfoGenerator)
{

}
FINISH_REFLECT()


void ChunkInfoGenerator::OnChunkInfoCreated(entt::registry& reg, entt::entity e)
{
	const TransformComponent& transfrom = reg.get<const TransformComponent>(e);

	AddToChunk(e, transfrom);
}


void ChunkInfoGenerator::OnChunkInfoDestroyed(entt::registry& reg, entt::entity e)
{
	const TransformComponent& transfrom = reg.get<const TransformComponent>(e);

	RemoveFromChunk(e, transfrom);
}
GraphInfo ChunkInfoGenerator::GetGraphBinding()
{
	return GraphInfo(
		{ TYPE_TO_ID(TransformComponent) },
		{ TYPE_TO_ID(ChunkInfoComponent) }
	);
}

void ChunkInfoGenerator::Init(entt::registry& reg)
{
	ChunkInfoObserver.connect(reg, entt::collector.update< TransformComponent>().where<ChunkInfoComponent>());
	reg.on_construct<ChunkInfoComponent>().connect<&ChunkInfoGenerator::OnChunkInfoCreated>(this);
	reg.on_destroy<ChunkInfoComponent>().connect<&ChunkInfoGenerator::OnChunkInfoDestroyed>(this);
}

void ChunkInfoGenerator::ShutDown(entt::registry& reg)
{
	reg.on_construct<ChunkInfoComponent>().disconnect<&ChunkInfoGenerator::OnChunkInfoCreated>(this);
	reg.on_destroy<ChunkInfoComponent>().disconnect<&ChunkInfoGenerator::OnChunkInfoDestroyed>(this);
}

void ChunkInfoGenerator::Update(entt::registry& reg)
{
	System::Update(reg);

	for (auto e : ChunkInfoObserver) {

		auto&& [transform, chunkInfo] = reg.get<const TransformComponent, ChunkInfoComponent>(e);

		Transform::GetPosition(transform);
	}

	ChunkInfoObserver.clear();
}

[[nodiscard]] const std::set<entt::entity>& ChunkInfoGenerator::GetChunk(const Perry::TransformComponent& transform) const
{
	glm::vec3 pos = Transform::GetPosition(transform);

	glm::ivec2 chunkID = glm::vec2(pos.x, pos.z) / ChunkSize;

	auto map = ChunkLookup.find(chunkID);
	if (map == ChunkLookup.end())
	{
		return std::set<entt::entity>();
	}

	return map->second;
}

std::vector<const std::set<entt::entity>*> ChunkInfoGenerator::GetNeighbourHood(const Perry::TransformComponent& transform,
	float maxDistance) const
{
	glm::vec3 pos = Transform::GetPosition(transform);
	glm::ivec2 ogChunkID = glm::vec2(pos.x, pos.z) / ChunkSize;

	std::vector<const std::set<entt::entity>*> targets = { &GetChunk(transform) };

	{
		pos.x += maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.x -= maxDistance;
	}
	{
		pos.x -= maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.x += maxDistance;
	}

	{
		pos.z += maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.z -= maxDistance;
	}
	{
		pos.z -= maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.z += maxDistance;
	}

	{
		pos.z += maxDistance;
		pos.x += maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.z -= maxDistance;
		pos.x -= maxDistance;
	}
	{
		pos.z += maxDistance;
		pos.x -= maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.z -= maxDistance;
		pos.x += maxDistance;
	}
	{
		pos.z -= maxDistance;
		pos.x += maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.z += maxDistance;
		pos.x -= maxDistance;
	}
	{
		pos.z -= maxDistance;
		pos.x -= maxDistance;
		glm::ivec2 newChunk = glm::vec2(pos.x, pos.z) / ChunkSize;
		if (ogChunkID != newChunk)
		{
			auto mapSearch = ChunkLookup.find(newChunk);
			if (mapSearch != ChunkLookup.end())
			{
				targets.emplace_back(&mapSearch->second);
			}
		}
		pos.z += maxDistance;
		pos.x += maxDistance;
	}

	return targets;
}
void ChunkInfoGenerator::AddToChunk(entt::entity e, const Perry::TransformComponent& transform)
{
	glm::vec3 pos = Transform::GetPosition(transform);

	glm::ivec2 chunkID = glm::vec2(pos.x, pos.z) / ChunkSize;

	auto map = ChunkLookup.find(chunkID);
	if (map == ChunkLookup.end())
	{
		return;
	}

	map->second.insert(e);
}

void ChunkInfoGenerator::RemoveFromChunk(entt::entity e, const Perry::TransformComponent& transform)
{
	glm::vec3 pos = Transform::GetPosition(transform);

	glm::ivec2 chunkID = glm::vec2(pos.x, pos.z) / ChunkSize;

	auto map = ChunkLookup.find(chunkID);
	if (map == ChunkLookup.end())
	{
		return;
	}

	map->second.erase(e);
}