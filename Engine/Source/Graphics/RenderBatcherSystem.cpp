#include "EnginePCH.h"
#pragma hdrstop

#include "ECS/Systems/RenderBatcherSystem.h"

#include "Graphics/Material.h"
#include "Graphics/Primitive.h"
#include "Transform.h"
#include <entt/entt.hpp>

#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Systems/MetaReflectSystem.h"
#include "Graphics/Model.h"
#include "Graphics/RenderInfo.h"

#include <EnttEditor/MetaReflectImplement.h>

using namespace Perry;


IMPLEMENT_REFLECT_SYSTEM(RenderBatcherSystem)
{

}
FINISH_REFLECT()

GraphInfo RenderBatcherSystem::GetGraphBinding()
{
	return GraphInfo({ TYPE_TO_ID(TransformComponent),TYPE_TO_ID(MeshComponent) }, {  });
}

bool ProxyContainers::Remove(const EntityGenerator& e) {
	if (e.m_Material->IsBlending()) {
		for (size_t i = 0; i < m_BlendBatch.size(); ++i) {
			if (m_BlendBatch[i].e == e.e)
				if (m_BlendBatch[i].m_Prim == e.m_Prim)
					if (m_BlendBatch[i].m_Material == e.m_Material) {
						m_BlendBatch.erase(m_BlendBatch.begin() + i);

						return m_SimpleBatch.empty();
					}
		}
	}
	else {
		for (size_t i = 0; i < m_SimpleBatch.size(); ++i) {
			if (m_SimpleBatch[i].m_Material == e.m_Material) {
				if (m_SimpleBatch[i].Remove(e)) {
					m_SimpleBatch.erase(m_SimpleBatch.begin() + i);
				}

				return m_SimpleBatch.empty();
			}
		}
	}

	// failed to delete;
	return false;
}

MaterialBatch* ProxyContainers::Find(const Resource<Material>& material)
{
	if (material->IsBlending())
	{
		throw "TODO";
	}
	else
	{
		for (int i = 0; i < m_SimpleBatch.size(); ++i)
		{
			if (m_SimpleBatch[i].m_Material == material)
			{
				return &m_SimpleBatch[i];
			}
		}
	}
	return nullptr;
}

void MaterialBatch::Insert(const EntityGenerator& eg)
{
	for (int i = 0; i < m_primitivebatch.size(); ++i)
	{
		if (m_primitivebatch[i].m_Prim == eg.m_Prim)
		{
			m_primitivebatch[i].Insert(eg);
			return;
		}
	}

	m_primitivebatch.emplace_back(eg.m_Prim);
	m_primitivebatch[m_primitivebatch.size() - 1].Insert(eg);
}

bool MaterialBatch::Remove(const EntityGenerator& e) {
	for (int i = 0; i < m_primitivebatch.size(); ++i) {
		if (m_primitivebatch[i].m_Prim == e.m_Prim) {
			if (m_primitivebatch[i].Remove(e)) {
				m_primitivebatch.erase(m_primitivebatch.begin() + i);
			}

			return m_primitivebatch.empty();
		}
	}
	return false;
}

PrimitiveBatch* MaterialBatch::Find(Primitive* prim)
{
	for (int i = 0; i < m_primitivebatch.size(); ++i)
	{
		if (m_primitivebatch[i].m_Prim == prim)
		{
			return &m_primitivebatch[i];
		}
	}
	return nullptr;
}

EntityGenerator::EntityGenerator() :e(entt::null)
{
}

PrimitiveBatch::PrimitiveBatch(const Primitive* prim)
	: m_Prim(prim), m_InstanceData("PrimtiveTransformBatch")
{
}

void PrimitiveBatch::Insert(const EntityGenerator& eg)
{
	batchCount++;
	m_Entities.emplace_back(eg.e);

	InstanceData data = { Transform::GetMatrix(eg.reg->get<const TransformComponent>(eg.e)) };
	unsigned index = m_InstanceData.AddElement(data);

	MeshComponent* meshcomp = eg.reg->try_get<MeshComponent>(eg.e);

	if (meshcomp) {
		ASSERT_MSG(LOG_GRAPHICS, index != (unsigned)-1, "Invalid Batch ID !");
		meshcomp->BatchID = index;
	}
}

bool PrimitiveBatch::Remove(const EntityGenerator& e)
{
	for (int i = 0; i < m_Entities.size(); ++i)
	{
		if (m_Entities[i] == e.e)
		{
			MeshComponent* meshcomp = e.reg->try_get<MeshComponent>(e.e);

			if (meshcomp) {
				ASSERT_MSG(LOG_GRAPHICS, meshcomp->BatchID != (unsigned)-1, "Invalid Batch ID !");
				m_InstanceData.RemoveElement(meshcomp->BatchID);
			}

			m_Entities.erase(m_Entities.begin() + i);
			return m_Entities.empty();
		}
	}
	return false;
}

void ProxyContainers::Insert(const EntityGenerator& eg)
{
	if (eg.m_Prim)
	{
		if (eg.m_Material->IsBlending())
		{

			auto ep = EntityProxy();
			ep.m_Prim = eg.m_Prim;
			ep.m_Material = eg.m_Material;
			ep.e = eg.e;

			m_BlendBatch.emplace_back(ep);
			return;
		}
		else {
			for (int i = 0; i < m_SimpleBatch.size(); ++i)
			{
				if (m_SimpleBatch[i].m_Material == eg.m_Material)
				{
					m_SimpleBatch[i].Insert(eg);
					return;
				}
			}

			m_SimpleBatch.emplace_back(eg.m_Material);
			m_SimpleBatch[m_SimpleBatch.size() - 1].Insert(eg);
			return;
		}
	}
	printf("no good \n");
}

void RenderBatcherSystem::Init(entt::registry& reg)
{
	//m_TransformObserver = entt::basic_observer<entt::registry>();
	m_Observer.connect(reg, entt::collector.update<TransformComponent>().where<MeshComponent>());

	reg.on_construct<MeshComponent>().connect<&RenderBatcherSystem::OnMeshComponentCreated>(this);
	reg.on_destroy<MeshComponent>().connect<&RenderBatcherSystem::OnMeshComponentDestroyed>(this);
	reg.on_update<MeshComponent>().connect<&RenderBatcherSystem::OnMeshComponentUpdate>(this);
}

void RenderBatcherSystem::ShutDown(entt::registry& reg) {
	reg.on_construct<MeshComponent>()
		.disconnect<&RenderBatcherSystem::OnMeshComponentCreated>(this);
	reg.on_destroy<MeshComponent>()
		.disconnect<&RenderBatcherSystem::OnMeshComponentDestroyed>(this);
	reg.on_update<MeshComponent>()
		.disconnect<&RenderBatcherSystem::OnMeshComponentUpdate>(this);
}

void RenderBatcherSystem::Update(entt::registry& reg)
{
	if (!GlobalReadMutex.try_lock())
		return;

	EntityGenerator currentGen;

	while (createQueue.try_pop(currentGen))
	{
		batcher.Insert(currentGen);
	}

	while (deleteQueue.try_pop(currentGen))
	{
		batcher.Remove(currentGen);
	}

	while (updateQueue.try_pop(currentGen))
	{
		batcher.Remove(currentGen);
		batcher.Insert(currentGen);
	}


	for (auto e : m_Observer) {

		auto&& [transform, meshComponent] = reg.get<const TransformComponent, const MeshComponent>(e);
		for (auto& prim : meshComponent.GetMesh().m_Primitives)
		{
			auto matrix = Transform::GetMatrix(transform);

			auto& instanceData = batcher.Find(meshComponent.GetMaterial(prim.m_MaterialIndex))
				->Find(&prim)->m_InstanceData;

			instanceData.UpdateElement(meshComponent.GetBatchId(), { matrix });
		}

	}

	m_Observer.clear();

	GlobalReadMutex.unlock();
}

void RenderBatcherSystem::OnMaterialBlendChange(Material* mat)
{
	for (auto blendBatch : batcher.m_BlendBatch)
	{
	}
}

void RenderBatcherSystem::OnMaterialChange(entt::registry& reg, entt::entity e, Resource<Material> oldMaterial, unsigned index)
{
	if (e == entt::null)
	{
		printf(" error \n");
		return;
	}
	auto mc = reg.get<MeshComponent>(e);

	for (auto& prim : mc.GetMesh().m_Primitives)
	{
		if (prim.m_MaterialIndex == index)
		{
			auto eg = EntityGenerator();
			eg.e = e;
			eg.m_Prim = &prim;
			eg.m_Material = oldMaterial;
			eg.reg = &reg;
			batcher.Remove(eg);

			eg.m_Material = mc.GetMaterial(index);
			batcher.Insert(eg);
		}
	}
}

void RenderBatcherSystem::OnMeshComponentCreated(entt::registry& reg, entt::entity e)
{
	TransformScope ts{ &reg };


	const auto& mc = reg.get<MeshComponent>(e);


	if (!mc.GetModel().IsLoaded())
	{
		mc.GetModel()->OnLoad([&reg, e, this](IResourceType*)
			{
				//Check if the entity is still alive...
				//TODO: maybe we should validate that the component is still refering to same index..
				if (!reg.valid(e))
					return;

				//Entity doesn't have a mesh component anymore...
				if (!reg.all_of<MeshComponent>(e))
					return;

				OnMeshComponentCreated(reg, e);

			});
		return;
	}

	for (const auto& prim : mc.GetMesh().m_Primitives)
	{
		auto eg = EntityGenerator();
		eg.e = e;
		eg.m_Prim = &prim;
		eg.m_Material = mc.GetMaterial(prim.m_MaterialIndex);
		eg.reg = &reg;

		assert(eg.m_Material);

		createQueue.push(eg);
	}
}

void RenderBatcherSystem::OnMeshComponentDestroyed(entt::registry& reg, entt::entity e)
{
	auto mc = reg.get<MeshComponent>(e);

	for (auto& prim : mc.GetMesh().m_Primitives)
	{
		auto eg = EntityGenerator();
		eg.e = e;
		eg.m_Prim = &prim;
		eg.m_Material = mc.GetMaterial(prim.m_MaterialIndex);
		eg.reg = &reg;

		deleteQueue.push(eg);
	}
}

void RenderBatcherSystem::OnMeshComponentUpdate(entt::registry& reg, entt::entity e)
{
	auto mc = reg.get<MeshComponent>(e);

	for (const auto& prim : mc.GetMesh().m_Primitives)
	{
		auto eg = EntityGenerator();
		eg.e = e;
		eg.m_Prim = &prim;
		eg.m_Material = mc.GetMaterial(prim.m_MaterialIndex);
		eg.reg = &reg;

		assert(eg.m_Material);

		updateQueue.push(eg);
	}
}