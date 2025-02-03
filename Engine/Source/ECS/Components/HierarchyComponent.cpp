#include "EnginePCH.h"

namespace Perry {

	IMPLEMENT_REFLECT_SYSTEM(HierarchySystem)
	{

	}
	FINISH_REFLECT();

	IMPLEMENT_REFLECT_COMPONENT(HierarchyComponent)
	{
		meta.prop(p_Internal);

		meta.data<&HierarchyComponent::parent>("M_Parent"_hs)
			PROP_DISPLAYNAME("Parent");

		meta.data<&HierarchyComponent::child>("M_Child"_hs)
			PROP_DISPLAYNAME("Child");

		meta.data<&HierarchyComponent::next>("M_Next"_hs)
			PROP_DISPLAYNAME("Next");

		meta.data<&HierarchyComponent::prev>("M_Previous"_hs)
			PROP_DISPLAYNAME("Previous");
	}
	FINISH_REFLECT();



	//Destroy all childs aswell
	void HierarchySystem::OnDestroy(entt::registry& reg, entt::entity e)
	{
		if (reg.valid(e))
		{
			HierarchyComponent& hierarchy = reg.get<HierarchyComponent>(e);

			for (auto element = hierarchy.begin(reg); element != hierarchy.end(); ++element)
			{
				auto e = element.GetEntity();
				if (reg.valid(e))
					reg.destroy(e);
			}

			if (reg.valid(hierarchy.child))
			{
				reg.destroy(hierarchy.child);
			}

			hierarchy = {};
		}
	}

	void HierarchySystem::Init(entt::registry& reg)
	{
		reg.on_destroy<HierarchyComponent>().connect<&OnDestroy>();
	}

	void HierarchySystem::ShutDown(entt::registry& reg)
	{
		reg.on_destroy<HierarchyComponent>().disconnect<&OnDestroy>();
	}

	void HierarchyComponent::InsertElementAfter(entt::registry& reg, entt::entity currentID, entt::entity element)
	{
		HierarchyComponent& elementHierarchy = reg.get<HierarchyComponent>(element);

		if (reg.valid(next))
		{
			HierarchyComponent& oldNextHierarchy = reg.get<HierarchyComponent>(next);

			oldNextHierarchy.prev = element;

		}
		//First set the new elements next to our next, then set our next to newlement
		elementHierarchy.next = next;
		next = element;

		elementHierarchy.parent = parent;
		elementHierarchy.prev = currentID;
	}

	void HierarchyComponent::RemoveElement(entt::registry& reg, entt::entity elementToRemove)
	{
		//We itterate on all entities that are at the same depth of this.
		//I think we can get rid of this loop and it will work fine....
		for (auto hierarchyEntity : this->begin(reg))
		{
			//We seek for the element we want to remove
			if (hierarchyEntity != elementToRemove) continue;

			auto& hierarchyToRemove = reg.get<HierarchyComponent>(elementToRemove);

			hierarchyToRemove.RemoveFromParent(reg);
		}
	}

	void HierarchyComponent::RemoveFromParent(entt::registry& reg)
	{
		if (prev != entt::null)
		{
			auto& prevHierarchy = reg.get<HierarchyComponent>(prev);
			prevHierarchy.next = next;
		}
		else
		{
			auto& parentHierarchy = reg.get<HierarchyComponent>(parent);
			parentHierarchy.child = next;
		}

		if (this->next != entt::null)
		{
			auto& nextHierarchy = reg.get<HierarchyComponent>(next);
			nextHierarchy.prev = prev;
		}

		next = entt::null;
		prev = entt::null;
		parent = entt::null;
	}

	//This is not great, can we do better?
	entt::entity HierarchyComponent::LastElement(entt::registry& reg)
	{
		entt::entity lastEntity = entt::null;
		//Entities that share the same depth as us.
		for (auto hierarchyEntity : this->begin(reg))
		{
			if (hierarchyEntity == entt::null) break;
			lastEntity = hierarchyEntity;
		}

		return lastEntity;
	}


	HierarchyComponent::Iterator::Iterator(entt::entity ent, entt::registry* reg) : m_current(ent), Registry(reg)
	{
	}

	HierarchyComponent::Iterator& HierarchyComponent::Iterator::operator++()
	{
		if (!Registry->valid(m_current))
		{
			m_current = entt::null;
			return *this;
		}

		auto& currentHierarchy = Registry->get<HierarchyComponent>(m_current);
		m_current = currentHierarchy.next;
		return *this;
	}

	bool HierarchyComponent::Iterator::operator!=(const Iterator& iterator) const
	{
		return m_current != iterator.m_current;
	}

	entt::entity HierarchyComponent::Iterator::operator*() const
	{
		return m_current;
	}
	entt::entity  HierarchyComponent::Iterator::GetEntity() const
	{
		return m_current;
	}
} // namespace Perry
