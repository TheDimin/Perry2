#include "EnginePCH.h"
#pragma hdrstop

namespace Perry
{
	CollisionComponent::CollisionComponent() : CollisionComponent(glm::vec2(1), "", LAYER_STATIC) {}

	CollisionComponent::CollisionComponent(const glm::vec2& Size, CollisionLayer l) : CollisionComponent(Size, "", l) {}

	CollisionComponent::CollisionComponent(const glm::vec2& Size, const std::string& Tag, CollisionLayer l) :
		m_CollisionLayer(l),
		m_Tag(Tag),
		m_Size(Size)
	{
		m_Size = m_Size * 0.5f;
		if (m_AutoResize)
		{
			//m_Size.x = m_TransformReference->m_Scale.x * 0.5f;
			 //  m_Size.y = m_TransformReference->m_Scale.y * 0.5f;
			 //  m_LastTransformScale = m_TransformReference->m_Scale;
		}
	}

	std::string CollisionComponent::GetTag() const
	{
		return m_Tag;
	}

	CollisionLayer CollisionComponent::GetLayer() const
	{
		return m_CollisionLayer;
	}

	glm::vec2 CollisionComponent::GetSize() const
	{
		return m_Size;
	}

	IMPLEMENT_REFLECT_COMPONENT(CollisionComponent)
	{
		meta.data<&CollisionComponent::m_Size>("M_SIZE"_hs)
			PROP_DISPLAYNAME("Size")
			PROP_MINMAX(0.f, 200.f)
			PROP_DRAGSPEED(0.01f);
		meta.data<&CollisionComponent::m_Offset>("m_Offset"_hs)
			PROP_DISPLAYNAME("WorldSpace Offset")
			PROP_DRAGSPEED(0.01f);

		meta.data<&CollisionComponent::m_Tag>("m_Tag"_hs)
			PROP_DISPLAYNAME("Tag");

		meta.data<&CollisionComponent::m_AutoResize>("m_AutoResize"_hs)
			PROP_DISPLAYNAME("AutoResize");

		meta.data<&CollisionComponent::m_TriggerBox>("m_TriggerBox"_hs)
			PROP_DISPLAYNAME("Trigger box");

		meta.data<&CollisionComponent::m_IsStatic>("m_IsStatic"_hs)
			PROP_DISPLAYNAME("Static");

		meta.data<&CollisionComponent::m_CollisionLayer>("m_collisionLayer"_hs)
			PROP_DISPLAYNAME("Layer");

		//TODO Add support for enums
		/*
			auto currentLayer = Perry::LayerDisplayNames[collider.collisionLayer];

		if (ImGui::BeginCombo("CollisionLayer", currentLayer.c_str()))
		{
			for (size_t i = 0; i < CollisionLayerCount; i++)
			{
				if (ImGui::Selectable(Perry::LayerDisplayNames[i].c_str(), currentLayer.c_str()))
				{
					collider.collisionLayer = (CollisionLayer)i;
				}
			}
			ImGui::EndCombo();
		}*/
	}
	FINISH_REFLECT()
}