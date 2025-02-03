#include "EnginePCH.h"
#pragma hdrstop

#include "Graphics/GLTFLoader.h"

using namespace Perry;

void GLTFLoader::AddProperty(const std::string& name, PropertyFunc callback)
{
	if (m_EntityProperties.find(name) == m_EntityProperties.end())
	{
		m_EntityProperties.insert({ name,callback });
	}
}

void GLTFLoader::AddLight(Perry::Entity& light, const tinygltf::Model& model, const tinygltf::Value& data)
{
	int index = data.Get("light").Get<int>();
	//TODO validate index
	auto& tinyLight = model.lights[index];
	if (tinyLight.type == "point")
	{
		auto& pointLight = light.AddComponent<PointLightComponent>();
		pointLight.Color = glm::vec4(tinyLight.color[0], tinyLight.color[1], tinyLight.color[2], 1.0f);
		pointLight.Intensity = static_cast<float>(tinyLight.intensity);
	}
	else if (tinyLight.type == "spot")
	{
		auto& spotLight = light.AddComponent<SpotLightComponent>();
		spotLight.Color = glm::vec4(tinyLight.color[0], tinyLight.color[1], tinyLight.color[2], 1.0f);
		spotLight.Intensity = static_cast<float>(tinyLight.intensity);
		spotLight.OuterAngle = static_cast<float>(glm::clamp(tinyLight.spot.outerConeAngle, 0.0, 1.57));
		spotLight.InnerAngle = static_cast<float>(glm::clamp(tinyLight.spot.innerConeAngle, 0.0, 1.57));
	}
	else if (tinyLight.type == "directional")
	{
		auto& directionalLight = light.AddComponent<DirectionalLightComponent>();
		directionalLight.Color = glm::vec4(tinyLight.color[0], tinyLight.color[1], tinyLight.color[2], 1.0f);
		directionalLight.Intensity = static_cast<float>(tinyLight.intensity);
	}
}

void Perry::GLTFLoader::AddShadow(Perry::Entity& e, const tinygltf::Model&, const Perry::Mesh*)
{
	if (auto* light = e.GetRegistry().try_get<DirectionalLightComponent>(e))
	{
		//GetEngine().GetLevel().GetSystem<DirectionalLightSystem>()->AddShadow(light, 2048, 2048, 100.f, 200.f);
		e.GetRegistry().patch<DirectionalLightComponent>(e);
	}
	else if (auto* light = e.GetRegistry().try_get<SpotLightComponent>(e))
	{
		//GetEngine().GetLevel().GetSystem<SpotLightSystem>()->AddShadow(light, 256, 256, 0.65f, 100.f);
		e.GetRegistry().patch<SpotLightComponent>(e);
	}
}
