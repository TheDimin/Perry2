#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_COMPONENT(MeshComponent)
{
	//meta.func<&entt::registry::emplace<MeshComponent, const std::string&>>(f_AddComponent);
	//meta.ctor<const std::string&>();

	meta.data<&MeshComponent::m_Model>("Mesh"_hs)
		PROP_DISPLAYNAME("Mesh");

	meta.data<&MeshComponent::m_MeshIndex>("MeshIndex"_hs)
		PROP_DISPLAYNAME("Mesh Index");

	meta.data<&MeshComponent::m_Materials>("MATERIALS"_hs)
		PROP_DISPLAYNAME("Materials")
		PROP_NOT_SERIALIZABLE;
}
FINISH_REFLECT()

MeshComponent::MeshComponent(const Resource<Model>& model, unsigned int meshIndex) :
	m_Model(model),
	m_MeshIndex(meshIndex)
{
	m_Materials.resize(GetMaterialCount(), {});
	assert(m_Model);
}

Mesh& MeshComponent::GetMesh() const
{
	return m_Model->GetMesh(m_MeshIndex);
}

int MeshComponent::GetMaterialCount() const
{
	return int(m_Model->GetMaterialCount());
}

Resource<Material> MeshComponent::GetMaterial(unsigned index) const
{
	if (m_Materials.size() > index && m_Materials[index])
	{
		return m_Materials[index];
	}

	return m_Model->GetMaterial(index);
}

void MeshComponent::SetMaterial(unsigned index, const Resource<Material>& material)
{
	auto oldmat = GetMaterial(index);

	m_Materials[index] = material;
	//Not ideal way of doing it, but fast to test
	//GetEngine().GetLevel().GetSystem<RenderBatcherSystem>()->OnMaterialChange(entt::to_entity(GetRegistry(), *this), oldmat, index);
}