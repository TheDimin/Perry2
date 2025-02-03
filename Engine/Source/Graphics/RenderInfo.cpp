#include "EnginePCH.h"
#pragma hdrstop

#include "Graphics/RenderInfo.h"

#include "ECS/Components/TransformComponent.h"
#include "Transform.h"
#include "ECS/Components/LightComponents.h"

Perry::PointLight::PointLight(const Perry::TransformComponent& transform, const PointLightComponent& lightSource) :
	m_Position(Transform::GetPosition(transform)), m_Intensity(lightSource.Intensity),
	m_Color(lightSource.Color)
{
}

Perry::DirectionalLight::DirectionalLight(const Perry::TransformComponent& transform,const DirectionalLightComponent& lightSource) :
	m_Direction(Transform::GetForwardVector(transform)),
	m_Intensity(lightSource.Intensity),
	m_Color(lightSource.Color)
{
}

Perry::SpotLight::SpotLight(const Perry::TransformComponent& transform,const Perry::SpotLightComponent& lightSource) :
	m_Position(Transform::GetPosition(transform)),
	m_Color(lightSource.Color),
	m_Direction(Transform::GetForwardVector(transform)),
	m_InnerAngle(lightSource.InnerAngle),
	m_OuterAngle(lightSource.OuterAngle),
	m_Intensity(lightSource.Intensity)
{
}