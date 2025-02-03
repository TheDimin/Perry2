#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_COMPONENT(StaticTransform)
{

}
FINISH_REFLECT()

IMPLEMENT_REFLECT_COMPONENT(TransformComponent)
{
	meta.data<&TransformComponent::m_Position>("M_Pos"_hs) PROP_DISPLAYNAME("Position");

	meta.data<&TransformComponent::m_Rotation>("M_Rot"_hs) PROP_DISPLAYNAME("Rotation");

	meta.data<&TransformComponent::m_Scale>("M_Scale"_hs) PROP_DISPLAYNAME("Scale");
}
FINISH_REFLECT()

TransformComponent::TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl) :
	m_Position(pos),
	m_Rotation(rot),
	m_Scale(scl)
{
}

TransformComponent::TransformComponent(const glm::mat4& data) :
	m_Position(Transform::GetPosition(data)),
	m_Rotation(Transform::GetRotation(data)),
	m_Scale(Transform::GetScale(data))
{
};

IMPLEMENT_REFLECT_SYSTEM(TransformSystem)
{
}
FINISH_REFLECT()
TransformSystem::TransformSystem()
{
	ExecutionPriority=100;
	RunInEditorMode = true;
}
void TransformSystem::Init(entt::registry& reg)
{
	updatedTransforms.connect(reg, entt::basic_collector<>::update<TransformComponent>());
}

void TransformSystem::Update(entt::registry& reg)
{
	for (auto e : updatedTransforms)
	{
		auto hiearchy = reg.try_get<HierarchyComponent>(e);

		Transform::RecalculateMatrix(reg.get<TransformComponent>(e), hiearchy);
	}
	updatedTransforms.clear();
};
