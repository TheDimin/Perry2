#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_SYSTEM(RotateSystem)
{

}
FINISH_REFLECT();

RotateComponent::RotateComponent() : m_Axis(glm::vec3(1.0f, 0.0f, 0.0f))

{
}
RotateComponent::RotateComponent(float speed, const glm::vec3& dir)
	:m_Axis(dir), m_Speed(speed)
{
}
IMPLEMENT_REFLECT_COMPONENT(RotateComponent)
{
	meta.data<&RotateComponent::m_Speed>("M_Speed"_hs)
		PROP_DISPLAYNAME("Speed");

	meta.ctor<float, glm::vec3>();

	meta.data<&RotateComponent::m_Axis>("M_DIRECTION"_hs)
		PROP_DISPLAYNAME("Direction")
		PROP_NORMALIZED
		PROP_DRAGSPEED(.01f);

}
FINISH_REFLECT()

void RotateSystem::Update(entt::registry& reg)
{
	auto group = reg.group<RotateComponent>(entt::get<TransformComponent>);

	//#define MT
#ifndef MT
	for (auto& entity : group)
	{
		TransformComponent& t = group.get<TransformComponent>(entity);
		RotateComponent& r = group.get<RotateComponent>(entity);

		float deltaAngle = r.m_Speed * GetDeltaTime() * glm::pi<float>() * 2.f;
		auto rot = Transform::GetRotation(t);
		rot *= glm::angleAxis(deltaAngle, glm::normalize(r.m_Axis));
		Transform::SetRotation(t, glm::normalize(rot));
	}
#else
	//Example of a system being executed parallel
	std::for_each(std::execution::par, group.begin(), group.end(), [group](auto&& entity)
		{
			TransformComponent& t = group.get<TransformComponent>(ent);
			const RotateComponent& r = group.get<const RotateComponent>(ent);

			t.m_Rotation += r.m_Axis * r.m_speed * GetDeltaTime();
		});
#endif
}
