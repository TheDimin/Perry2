#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;
using namespace entt::literals;

IMPLEMENT_REFLECT_SYSTEM(SpriteAnimationSystem)
{

}
FINISH_REFLECT();

IMPLEMENT_REFLECT_COMPONENT(SpriteComponent)
{
	meta.func<&entt::registry::emplace<SpriteComponent, const std::string&>>(f_AddComponent);


	meta.data<&SpriteComponent::m_FramesInTexture>("m_FramesInTexture"_hs)
		PROP_DISPLAYNAME("FramesInTexture");
	//PROP_READONLY;
	meta.data<&SpriteComponent::m_CurrentFrame>("m_CurrentFrame"_hs)
		PROP_DISPLAYNAME("CurrentFrame")
		PROP_READONLY;

	meta.data<&SpriteComponent::m_AnimationSpeed>("m_AnimationSpeed"_hs)
		PROP_DISPLAYNAME("AnimationSpeed");

	meta.data<&SpriteComponent::m_AnimationTimer>("m_AnimationTimer"_hs)
		PROP_DISPLAYNAME("AnimationTimer")
		PROP_READONLY;

	meta.data<&SpriteComponent::m_FlipX>("m_FlipX"_hs)
		PROP_DISPLAYNAME("Flip X");

	meta.data<&SpriteComponent::m_FlipY>("m_FlipY"_hs)
		PROP_DISPLAYNAME("Flip Y");
}
FINISH_REFLECT();

SpriteComponent::SpriteComponent()
{
	m_Material = ResourceManager<Material>::Get("SpriteFallback");

	if (m_Material.IsLoaded())
		return;

	MaterialCreator matInfo{};
	matInfo.m_Shader = ResourceManager<Shader>::Get("SpriteShader");
	{
		matInfo.m_Textures.insert({ TextureSlotNameLookup[5],
			ResourceManager<Texture>::Get("Textures/DefaultORM.png") });

	}
	matInfo.m_UsesBlending = false;
	m_Material = ResourceManager<Material>::LoadAsync("SpriteFallback", matInfo);
}

SpriteComponent::SpriteComponent(const std::string& filePath, unsigned framesInTexture, float animationSpeed,
	bool hasBlendTexture, glm::vec4 color, float emissiveFactor)
{
	m_FramesInTexture = framesInTexture;
	m_AnimationSpeed = animationSpeed;

	MaterialCreator matInfo{};
	matInfo.m_Shader = ResourceManager<Shader>::Load("SpriteShader", "Sprite.Vertex", "Sprite.Fragment");
	{
		matInfo.m_Textures.insert({ TextureSlotNameLookup[0],
			ResourceManager<Texture>::LoadAsync(filePath)
			});
	}
	matInfo.m_AlbedoFactor = color;
	matInfo.m_EmissiveFactor = glm::vec3(emissiveFactor);
	matInfo.m_UsesBlending = hasBlendTexture;

	m_Material = ResourceManager<Material>::LoadAsync(filePath, matInfo);
}

void Perry::SpriteComponent::SetAnimationStride(unsigned int startFrame, unsigned int endFrame, float animationSpeed)
{
	m_HasAnimationStride = true;

	if (m_AnimationStart != startFrame && m_AnimationEnd != endFrame)
	{
		m_CurrentFrame = startFrame;
		m_AnimationTimer = 0.0f;
	}

	m_AnimationStart = startFrame;
	m_AnimationEnd = endFrame;
	m_AnimationSpeed = animationSpeed;
}

void Perry::SpriteComponent::SetTexture(const std::string& filepath, int slot)
{
	m_Material->m_Textures.insert({ TextureSlotNameLookup[slot],ResourceManager<Texture>::Get(filepath) });
}

float SpriteComponent::GetFrameProgress() const { return 1.0f - (ceilf(m_AnimationTimer) - m_AnimationTimer); }

void SpriteAnimationSystem::Init(entt::registry& reg)
{
	ExecutionPriority = -100;
}

void SpriteAnimationSystem::Update(entt::registry& reg)
{
	for (auto&& [entity, spriteComponent] : reg.view<SpriteComponent>().each())
	{
		if (spriteComponent.m_FramesInTexture > 1 && !spriteComponent.m_Paused)
		{
			spriteComponent.m_AnimationTimer += GetDeltaTime() * spriteComponent.m_AnimationSpeed;

			if (spriteComponent.m_HasAnimationStride)
			{
				if (spriteComponent.m_AnimationTimer > 1.0f)
				{
					spriteComponent.m_AnimationTimer = 0.0f;
					spriteComponent.m_PreviousFrame = spriteComponent.m_CurrentFrame;
					spriteComponent.m_CurrentFrame++;

					if (spriteComponent.m_CurrentFrame > spriteComponent.m_AnimationEnd)
					{
						spriteComponent.m_CurrentFrame = spriteComponent.m_AnimationStart;
					}
				}
			}
			else
			{
				int oldCurrentFrame = spriteComponent.m_CurrentFrame;
				spriteComponent.m_CurrentFrame =
					int(spriteComponent.m_AnimationTimer) % spriteComponent.m_FramesInTexture;

				if (oldCurrentFrame != spriteComponent.m_CurrentFrame)
				{
					spriteComponent.m_PreviousFrame = oldCurrentFrame;
				}
			}
		}
	}
}
GraphInfo SpriteAnimationSystem::GetGraphBinding()
{
	return GraphInfo({},{TYPE_TO_ID(SpriteComponent)});
}