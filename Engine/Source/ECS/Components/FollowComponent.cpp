#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_COMPONENT(FollowComponent)
{
	meta.data<&Perry::FollowComponent::m_Smoothing>("M_Smoothing"_hs) PROP_DISPLAYNAME("Smoothing")
		PROP_DRAGSPEED(0.01f) PROP_MINMAX(0.0f, 2.0f);

	meta.data<&FollowComponent::m_PosOffset>("M_PosOffset"_hs) PROP_DISPLAYNAME("Position Offset") PROP_DRAGSPEED(0.1f)
		PROP_MINMAX(-100.f, 100.f);

	meta.data<&FollowComponent::following>("M_FollowingEntity"_hs) PROP_DISPLAYNAME("Follow Target");
}
FINISH_REFLECT()

IMPLEMENT_REFLECT_SYSTEM(FollowSystem)
{}
FINISH_REFLECT()

GraphInfo FollowSystem::GetGraphBinding()
{
	return GraphInfo(
		{},
		{ TYPE_TO_ID(FollowComponent),TYPE_TO_ID(TransformComponent) }
	);
}


using namespace Perry;

FollowComponent::FollowComponent()
	:following(entt::null)
{
}
FollowComponent::FollowComponent(entt::entity FollowingEntity, glm::vec3 followOffset)
{
	//TODO ENTT make this folowerEntity instead of transform
	following = FollowingEntity;
	m_PosOffset = followOffset;
}

glm::vec3 lerp(const glm::vec3& a, const glm::vec3& b, float t)
{
	return a + t * (b - a);
}

void FollowSystem::Update(entt::registry& reg)
{
	for (auto&& [entity, follower, transform] : reg.view<FollowComponent, Perry::TransformComponent>().each())
	{
		if (!reg.valid(follower.following))
			continue;

		auto&& followingTransform = reg.get<Perry::TransformComponent>(follower.following);

		glm::vec3 targetPos = Transform::GetPosition(followingTransform);
		follower.m_SmoothPosition = Transform::GetPosition(transform);
		follower.m_SmoothPosition = lerp(follower.m_SmoothPosition,
			targetPos + follower.m_PosOffset,
			1.0f / (follower.m_Smoothing + 0.00001f) * GetDeltaTime());

		glm::vec3 dir = glm::normalize(targetPos - follower.m_SmoothPosition);
		glm::quat rot =
			glm::quat_cast(glm::inverse(glm::lookAt(targetPos + glm::normalize(follower.m_SmoothPosition), dir, Up())));

		Transform::SetPosition(transform, follower.m_SmoothPosition);
		// if (glm::isnan(rot[0]) || glm::isnan(rot[1]) || glm::isnan(rot[2]) || glm::isnan(rot[3]))
		// Transform::SetRotation(transform, rot);
	}
}
