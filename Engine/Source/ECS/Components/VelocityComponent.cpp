#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;
using namespace Perry::Utilities;

IMPLEMENT_REFLECT_COMPONENT(VelocityComponent)
{
	meta.data<&VelocityComponent::m_Velocity>("m_Velocity"_hs) PROP_DISPLAYNAME("Velocity");

	meta.data<&VelocityComponent::m_Friction>("m_Friction"_hs) PROP_DISPLAYNAME("Friction");
}
FINISH_REFLECT()

IMPLEMENT_REFLECT_SYSTEM(ApplyVelocitySystem)
{
}
FINISH_REFLECT()

GraphInfo ApplyVelocitySystem::GetGraphBinding()
{
	return GraphInfo({},
		{TYPE_TO_ID(Perry::TransformComponent), TYPE_TO_ID(VelocityComponent)}
		);
}

ApplyVelocitySystem::ApplyVelocitySystem()
{
	ExecutionPriority = 1;
}

void ApplyVelocitySystem::Update(entt::registry& reg)
{
	System::Update(reg);
    // ...

    #pragma omp parallel for
    for (auto&& [ent, velocity, transform] : reg.view<VelocityComponent, Perry::TransformComponent>().each())
    {
        if (glm::length(velocity.m_Velocity) > 0.0f)
        {
            // Update the position of the transform based on velocity and delta time
            Perry::Transform::AddTranslation(transform, velocity.m_Velocity * GetDeltaTime() * 0.1f);

            // Apply friction to reduce velocity over time
            // Clamp friction to avoid negative values and ensure stability
            float clampedFriction = glm::clamp(velocity.m_Friction, 0.0f, 1.0f);
            velocity.m_Velocity *= (1.0f - clampedFriction * GetDeltaTime());

            // Optionally, draw the velocity vector for debugging purposes
            if (drawVelocity)
            {
                auto position = Transform::GetPosition(transform);
                // Avoid division by zero when drawing the velocity vector
                float frictionFactor = clampedFriction > 0.0f ? velocity.m_Friction : 1.0f;
                Debug::DrawLine(position, position + velocity.m_Velocity / frictionFactor, glm::vec3(1.0f, 0.647f, 0.0f));
            }
        }
    }
}
