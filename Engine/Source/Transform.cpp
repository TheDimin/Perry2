#include "EnginePCH.h"
#include "glm/gtx/quaternion.hpp"
#pragma hdrstop



//#define VALIDATE_HIERARCHY

using namespace Perry;
#ifdef VALIDATE_HIERARCHY
#define RESOLVE_HIERARCHY(Source)\
	if (hierarchy)\
		{assert(entt::to_entity(GetRegistry(), Source) == entt::to_entity(GetRegistry(), *hierarchy)/*"Passed in different entities"*/);}\
	else\
		hierarchy = GetRegistry().try_get<HierarchyComponent>(entt::to_entity(GetRegistry(), Source));
#else
#define RESOLVE_HIERARCHY(Source)\
	if (!hierarchy) hierarchy = GetRegistry().try_get<HierarchyComponent>(Perry::to_entity(GetRegistry(), Source));
#endif

glm::vec3 Transform::GetPosition(const glm::mat4& matrix)
{
	return glm::vec3(matrix[3]);
}

glm::quat Transform::GetRotation(const glm::mat4& matrix)
{
	return glm::quat_cast(matrix);
}

glm::vec3 Transform::GetScale(const glm::mat4& matrix)
{
	return glm::vec3(
		glm::length(glm::vec3(matrix[0])),
		glm::length(glm::vec3(matrix[1])),
		glm::length(glm::vec3(matrix[2]))
	);
}

glm::vec3 Transform::GetLocalForwardVector(const TransformComponent& transform)
{
	return transform.m_Rotation * Forward();
}

glm::vec3 Transform::GetLocalUpVector(const TransformComponent& transform)
{
	return transform.m_Rotation * Up();
}

glm::vec3 Transform::GetLocalRightVector(const TransformComponent& transform)
{
	return transform.m_Rotation * Right();
}

glm::vec3 Transform::GetUpVector(const TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);


	if (!Transform::HasParent(hierarchy))
		return Transform::GetLocalUpVector(transform);

	return GetRotation(transform, hierarchy) * Up();
}

glm::vec3 Transform::GetForwardVector(const TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
		return Transform::GetLocalForwardVector(transform);

	return GetRotation(transform, hierarchy) * Forward();
}

glm::vec3 Perry::Transform::GetRightVector(const TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
		return Transform::GetLocalRightVector(transform);

	return GetRotation(transform, hierarchy) * Right();
}

TransformScope::TransformScope(entt::registry* reg) : newOne(reg)
{
	Fallback = Transform::Registry;
	Transform::SetRegistery(newOne);
}

TransformScope::~TransformScope()
{
	ASSERT_MSG(LOG_GAMEOBJECTS, &Transform::GetRegistry() == newOne, "TransformScope has changed but has not been closed off");

	Transform::SetRegistery(Fallback);
}

glm::vec3 Transform::GetPosition(const TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
		return transform.m_Position;

	return GetPosition(GetMatrix(transform, hierarchy));
}

glm::quat Transform::GetRotation(const TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
		return transform.m_Rotation;

	return GetRotation(GetMatrix(transform, hierarchy)); // convert to quaternion
}

glm::vec3 Transform::GetScale(const TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
		return transform.m_Scale;

	return GetScale(GetMatrix(transform, hierarchy));
}

glm::mat4 Transform::GetMatrix(const TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);
	
	if (Transform::HasParent(hierarchy))
	{
		if (transform.m_Dirty)
		{
			RecalculateMatrix(transform, hierarchy);
		}

		return transform.m_Global;
	}

	return Transform::GetLocalMatrix(transform);
}

glm::vec3 Transform::GetLocalPosition(const TransformComponent& transform)
{
	return transform.m_Position;
}

glm::quat Transform::GetLocalRotation(const TransformComponent& transform)
{
	return transform.m_Rotation;
}

glm::vec3 Transform::GetLocalScale(const TransformComponent& transform)
{
	return transform.m_Scale;
}

glm::mat4 Transform::GetLocalMatrix(const TransformComponent& transform)
{
	if (transform.m_Dirty)
	{
		HierarchyComponent* hierachy = GetRegistry().try_get<HierarchyComponent>(Perry::to_entity(GetRegistry(), transform));
		RecalculateLocalMatrix(transform, hierachy);
	}
	return transform.m_Local;
}

void Transform::SetPosition(TransformComponent& transform, const glm::vec3& position, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
	{
		SetLocalPosition(transform, position);
		return;
	}

	const auto& oldWorldPos = GetPosition(transform, hierarchy);
	transform.m_Position += (position - oldWorldPos);

	SetDirty(transform, hierarchy);
}

void Transform::SetRotation(TransformComponent& transform, const glm::quat& rotation, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
	{
		SetLocalRotation(transform, rotation);
		return;
	}

	const auto& OldWorldRot = GetRotation(transform, hierarchy);
	transform.m_Rotation *= glm::normalize(glm::inverse(OldWorldRot) * rotation); // Maybe * is not needed (we do not know)

	SetDirty(transform, hierarchy);
}

void Transform::SetRotation(TransformComponent& transform, const glm::vec3& rotation, const HierarchyComponent* hierarchy)
{
	SetRotation(transform, glm::quat(rotation), hierarchy);
}

void Transform::SetScale(TransformComponent& transform, const glm::vec3& scale, const HierarchyComponent* hierarchy)
{
	RESOLVE_HIERARCHY(transform);

	if (!Transform::HasParent(hierarchy))
	{
		SetLocalScale(transform, scale);
		return;
	}

	auto&& [parentTransform, parentHierarchy] = GetRegistry().get<TransformComponent, const HierarchyComponent>(Transform::GetParent(hierarchy));

	const auto& parentScale = GetScale(parentTransform, &parentHierarchy) * transform.m_Scale;

	transform.m_Scale += scale - parentScale;
	SetDirty(transform, hierarchy);
}

void Transform::SetLocalPosition(TransformComponent& transform, const glm::vec3& position)
{
	transform.m_Position = position;
	SetDirty(transform);
}

void Transform::SetLocalRotation(TransformComponent& transform, const glm::quat& rotation)
{
	transform.m_Rotation = glm::normalize(rotation);
	SetDirty(transform);
}
void Transform::SetLocalRotation(TransformComponent& transform, const glm::vec3& rotation)
{
	SetLocalRotation(transform, glm::quat(glm::radians(rotation)));
}
void Transform::SetLocalScale(TransformComponent& transform, const glm::vec3& scale)
{
	transform.m_Scale = scale;
	SetDirty(transform);
}

void Transform::AddTranslation(TransformComponent& transform, const glm::vec3& translationToAdd)
{
	if (glm::length(translationToAdd) < 0.0001f) return;

	transform.m_Position += translationToAdd;
	SetDirty(transform);
}

void Transform::AddRotation(TransformComponent& transform, const glm::quat& rotationToAdd)
{
	transform.m_Rotation *= rotationToAdd;
	SetDirty(transform);
}

void Transform::AddRotation(TransformComponent& transform, const glm::vec3& rotationToAdd)
{
	transform.m_Rotation *= glm::quat(glm::radians(rotationToAdd));
	SetDirty(transform);
}

void Transform::AddScale(TransformComponent& transform, const glm::vec3& scaleToAdd)
{
	transform.m_Scale += scaleToAdd;
	SetDirty(transform);
}

void Transform::SetParent(entt::entity newParent, entt::entity child, bool worldPositionStays)
{
	entt::registry& reg = GetRegistry();

	//Check if both entites have a Hieracycomponent
	//Check if entity has parent
		//Clear it if it got one

	HierarchyComponent& newChildHierarchy = reg.get_or_emplace<HierarchyComponent>(child);
	HierarchyComponent& newParentHierarchy = reg.get_or_emplace<HierarchyComponent>(newParent);

	const TransformComponent& childTransform = reg.get<TransformComponent>(child);

	if (reg.valid(newChildHierarchy.parent))
	{
		auto& oldParentTransform = reg.get< TransformComponent>(newChildHierarchy.parent);

		auto pMat4 = Transform::GetMatrix(oldParentTransform);
		reg.replace<TransformComponent>(child, pMat4 * Transform::GetLocalMatrix(childTransform));
		newChildHierarchy.RemoveFromParent(reg);
		SetDirty(oldParentTransform);
	}

	const TransformComponent& newParentTransform = reg.get<const TransformComponent>(newParent);
	newChildHierarchy.parent = newParent;

	if (worldPositionStays)
	{
		auto pMat4 = Transform::GetMatrix(newParentTransform, &newParentHierarchy);
		reg.replace<TransformComponent>(child, glm::inverse(pMat4) * (Transform::GetLocalMatrix(childTransform)));
	}

	if (reg.valid(newParentHierarchy.child))
	{
		HierarchyComponent& ParentChildHierarchy = reg.get<HierarchyComponent>(newParentHierarchy.child);

		ParentChildHierarchy.InsertElementAfter(reg, newParentHierarchy.child, child);
		return;
	}

	newParentHierarchy.child = child;

	{
		auto&& [parentTransform, parentHierarchy] = GetRegistry().get<TransformComponent, const HierarchyComponent>(newParent);
		SetDirty(parentTransform, &parentHierarchy);
		auto&& [childTransform2, childHierarchy2] = GetRegistry().get<TransformComponent, const HierarchyComponent>(child);
		SetDirty(childTransform2, &childHierarchy2);
	}
}

entt::entity Perry::Transform::GetParent(const HierarchyComponent* hierarchy)
{
	if (!hierarchy)
		return entt::null;

	return hierarchy->parent;
}

entt::entity Perry::Transform::GetTopParent(const HierarchyComponent* hierarchy) {
	entt::entity prevEntity = GetParent(hierarchy);
	while (GetParent(GetRegistry().try_get<HierarchyComponent>(prevEntity)) != entt::null) {
		prevEntity = GetParent(GetRegistry().try_get<HierarchyComponent>(prevEntity));
	}
	return prevEntity;
}
std::vector<entt::entity> Perry::Transform::GetAllParents(const HierarchyComponent* hierarchy) {
	entt::entity prevEntity = GetParent(hierarchy);
	std::vector<entt::entity> parents;
	parents.push_back(prevEntity);
	while (GetParent(GetRegistry().try_get<HierarchyComponent>(prevEntity)) != entt::null) {
		prevEntity = GetParent(GetRegistry().try_get<HierarchyComponent>(prevEntity));
		parents.push_back(prevEntity);
	}
	return parents;
}
bool Transform::HasParent(const HierarchyComponent* hierarchy)
{
	if (!hierarchy)
		return false;

	return hierarchy->parent != entt::null;
}

bool Transform::HasChild(const HierarchyComponent* hierarchy)
{
	if (!hierarchy)
		return false;

	return hierarchy->child != entt::null;
}

entt::entity Transform::GetChild(const HierarchyComponent* hierarchy)
{
	if (!hierarchy)
		return entt::null;

	return hierarchy->child;
}

void Transform::DeleteChilderen(const HierarchyComponent* hierarchy)
{
	if (!HasChild(hierarchy)) return;

	auto& childHierarchy = GetRegistry().get<HierarchyComponent>(GetChild(hierarchy));

	entt::entity nextEnt = childHierarchy.next;
	while (nextEnt != entt::null)
	{
		auto current = nextEnt;

		nextEnt = GetRegistry().get<HierarchyComponent>(current).next;

		GetRegistry().destroy(current);
	}

	GetRegistry().destroy(GetChild(hierarchy));
}

std::vector<entt::entity> Transform::GetDirectChildren(entt::entity parent)
{
	std::vector<entt::entity> children{};

	auto& hierarchy = GetRegistry().get<HierarchyComponent>(parent);
	entt::entity child = hierarchy.child;

	while (child != entt::null)
	{
		children.push_back(child);
		auto& childHierarchy = GetRegistry().get<HierarchyComponent>(child);
		child = childHierarchy.next;
	}

	return children;
}
void Transform::SetRegistery(entt::registry* reg)
{
	Registry = reg;
}

entt::registry& Transform::GetRegistry()
{
	ASSERT_MSG(LOG_GAMEOBJECTS, Registry, "Transform API has no active Registery please call this beforehand !");
	return *Registry;
}

void Transform::SetDirty(TransformComponent& transform, const HierarchyComponent* hierarchy)
{
	transform.m_Dirty = true;

	GetRegistry().patch<TransformComponent>(Perry::to_entity(GetRegistry(), transform));

	RESOLVE_HIERARCHY(transform);
	if (hierarchy && hierarchy->child != entt::null)
		MarkChildrenDirty(hierarchy->child);
}
std::vector<entt::entity> Transform::HiearchyRelation(HierarchyComponent& h)
{
	std::vector<entt::entity> ents{};

	for (auto ent : h.begin(*Registry))
	{
		ents.emplace_back(ent);
	}
	return ents;
}
bool Transform::HasHiearchy(entt::entity e)
{
	if (!Registry->valid(e))
		return false;

	return Registry->all_of<HierarchyComponent>(e);
}
HierarchyComponent& Transform::GetHierarchy(entt::entity e)
{
	return Registry->get<HierarchyComponent>(e);
}

void Transform::RecalculateLocalMatrix(const TransformComponent& transform, const HierarchyComponent* hiearchy)
{
	glm::mat4 m_Model = glm::identity<glm::mat4>();
	m_Model = glm::translate(m_Model, transform.m_Position);
	m_Model *= glm::toMat4(transform.m_Rotation);
	transform.m_Local = glm::scale(m_Model, transform.m_Scale);

	if (!Transform::HasParent(hiearchy))
		transform.m_Global = transform.m_Local;

	transform.m_Dirty = false;
}

void Transform::RecalculateMatrix(const TransformComponent& transform, const HierarchyComponent* hiearchy)
{
	RecalculateLocalMatrix(transform, hiearchy);

	if (Transform::HasParent(hiearchy))
	{
		auto&& [pTransform, PHierarchy] = GetRegistry().get<TransformComponent, HierarchyComponent>(hiearchy->parent);

		if (hiearchy != &PHierarchy)
			RecalculateMatrix(pTransform, &PHierarchy);

		transform.m_Global = pTransform.m_Global * transform.m_Local;
	}
	//If we are the root note we already calculated the m_Global in LocalMatrix

	transform.m_Dirty = false;
}

bool Transform::IsDirty(const TransformComponent& transform)
{
	return transform.m_Dirty;
}

void Transform::MarkChildrenDirty(entt::entity e)
{
	if (!GetRegistry().valid(e)) return;

	auto&& [childTransform, childHierarchy] = GetRegistry().get<TransformComponent, const HierarchyComponent>(e);

	childTransform.m_Dirty = true;
	GetRegistry().patch<TransformComponent>(e);

	if (childHierarchy.next != entt::null)
		MarkChildrenDirty(childHierarchy.next);


	if (childHierarchy.child != entt::null)
	{
		//This entities childeren are already dirty.. No need to check its childere
		if (!GetRegistry().get<TransformComponent>(childHierarchy.child).m_Dirty)
			MarkChildrenDirty(childHierarchy.child);
	}
}

