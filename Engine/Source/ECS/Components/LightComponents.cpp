#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;


IMPLEMENT_REFLECT_COMPONENT(PointLightComponent)
{
	meta.data<&PointLightComponent::Color>("M_Color"_hs)
		PROP_DISPLAYNAME("Color")
		PROP_COLORPICKER;

	meta.data<&PointLightComponent::Intensity>("M_Intensity"_hs)
		PROP_DRAGSPEED(0.01f)
		PROP_DISPLAYNAME("Intensity");
}
FINISH_REFLECT()

IMPLEMENT_REFLECT_COMPONENT(SpotLightComponent)
{
	meta.data<&SpotLightComponent::Color>("M_Color"_hs)
		PROP_DISPLAYNAME("Color")
		PROP_COLORPICKER;
	meta.data<&SpotLightComponent::InnerAngle>("m_InnerAngle"_hs)
		PROP_DRAGSPEED(0.1f)
		PROP_MINMAX(0.1f, 3.14f)
		PROP_DISPLAYNAME("Inner Angle");

	meta.data<&SpotLightComponent::OuterAngle>("m_OuterAngle"_hs)
		PROP_DRAGSPEED(0.1f)
		PROP_MINMAX(0.1f, 3.14f)
		PROP_DISPLAYNAME("Outer Angle");

	meta.data<&SpotLightComponent::Intensity>("m_Intensity"_hs)
		PROP_DRAGSPEED(0.01f)
		PROP_DISPLAYNAME("Intensity");
}
FINISH_REFLECT()

IMPLEMENT_REFLECT_COMPONENT(DirectionalLightComponent)
{
	meta.data<&DirectionalLightComponent::Color>("M_Color"_hs) PROP_DISPLAYNAME("Color") PROP_COLORPICKER;

	meta.data<&DirectionalLightComponent::Intensity>("m_Intensity"_hs) PROP_DRAGSPEED(0.1f)
		PROP_DISPLAYNAME("Intensity");
}
FINISH_REFLECT()

IMPLEMENT_REFLECT_SYSTEM(LightCalculationSystem)
{
}
FINISH_REFLECT()


PointLightComponent::PointLightComponent(const glm::vec3& color, float intensity)
	: Color(glm::vec4(color, 1.0f))
	, Intensity(intensity)
{}




SpotLightComponent::SpotLightComponent(const glm::vec3& color, float innerAngle, float outerAngle, float intensity)
	: Color(color.r, color.g, color.b, 1.0f)
	, InnerAngle(glm::pi<float>() / innerAngle)
	, OuterAngle(glm::pi<float>() / outerAngle)
	, Intensity(intensity)
{
}

DirectionalLightComponent::DirectionalLightComponent(const glm::vec3& color, float intensity) :
	Color(color.r, color.g, color.b, 1.0f), Intensity(intensity)
{
}

LightCalculationSystem::LightCalculationSystem() :
	PointLightBuffer(std::make_unique< MapBuffer<PointLight>>("PointLights")),
	DirectionalLightBuffer(std::make_unique< MapBuffer<DirectionalLight>>("DirectionalLights")),
	SpotLightBuffer(std::make_unique< MapBuffer<SpotLight>>("SpotLights"))
{
}


template<typename T>
void OnLightCreated(entt::registry& reg, entt::entity e)
{
	reg.patch<T>(e);
}

void LightCalculationSystem::Init(entt::registry& reg)
{
	System::Init(reg);
	SpotLightObserver.connect(reg, entt::basic_collector<>::update<SpotLightComponent>().update<TransformComponent>().where<SpotLightComponent>());
	DirectionalLightObserver.connect(reg, entt::basic_collector<>::update<DirectionalLightComponent>().update<TransformComponent>().where<DirectionalLightComponent>());
	PointLightObserver.connect(reg, entt::basic_collector<>::update<PointLightComponent>().update<TransformComponent>().where<PointLightComponent>());

	reg.on_construct<SpotLightComponent>().connect<&OnLightCreated<SpotLightComponent>>();
	reg.on_construct<DirectionalLightComponent>().connect<&OnLightCreated<DirectionalLightComponent>>();
	reg.on_construct<PointLightComponent>().connect<&OnLightCreated<PointLightComponent>>();


	SpotLightBuffer->Initialize();
	DirectionalLightBuffer->Initialize();
	PointLightBuffer->Initialize();
}

void LightCalculationSystem::Update(entt::registry& reg)
{
	System::Update(reg);

	for (auto entity : SpotLightObserver)
	{
		auto&& [transform, light] = reg.get<TransformComponent, SpotLightComponent>(entity);
		SpotLightBuffer->AddOrUpdateElement(entt::to_integral(entity), SpotLight(transform, light));
	}

	for (auto entity : DirectionalLightObserver)
	{
		auto&& [transform, light] = reg.get<TransformComponent, DirectionalLightComponent>(entity);
		DirectionalLightBuffer->AddOrUpdateElement(entt::to_integral(entity), DirectionalLight(transform, light));
	}

	for (auto entity : PointLightObserver)
	{
		auto&& [transform, light] = reg.get<TransformComponent, PointLightComponent>(entity);
		PointLightBuffer->AddOrUpdateElement(entt::to_integral(entity), PointLight(transform, light));
	}

	SpotLightObserver.clear();
	DirectionalLightObserver.clear();
	PointLightObserver.clear();
}

void LightCalculationSystem::BindLights()
{
	PointLightBuffer->Bind(0);
	//DirectionalLightBuffer->Bind(1);
	//SpotLightBuffer->Bind(2);
}
GraphInfo LightCalculationSystem::GetGraphBinding()
{
	return GraphInfo({ TYPE_TO_ID(Renderer),TYPE_TO_ID(SpotLightComponent),TYPE_TO_ID(PointLightComponent),TYPE_TO_ID(DirectionalLightComponent) },
		{}
		
		);
}