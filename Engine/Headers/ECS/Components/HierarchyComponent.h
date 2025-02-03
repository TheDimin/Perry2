#pragma once

namespace Perry
{	
	class HierarchyComponent
	{
	public:
		HierarchyComponent() = default;
	private:
		friend class HierarchySystem;
		friend class Transform;
		entt::entity parent{ entt::null };
		entt::entity child{ entt::null };//First child

		entt::entity next{ entt::null }; // Same depth element
		entt::entity prev{ entt::null };

		void InsertElementAfter(entt::registry& reg, entt::entity currentID, entt::entity newElement);
		void RemoveElement(entt::registry& reg, entt::entity element);
		void RemoveFromParent(entt::registry& reg);
		entt::entity LastElement(entt::registry& reg);

	public:

		class Iterator
		{
		public:
			Iterator() = default;
			Iterator(entt::entity ent, entt::registry* reg);
			Iterator& operator++();
			bool operator!=(const Iterator& iterator)const;
			entt::entity operator*() const;

			entt::entity GetEntity()const;
			Iterator begin() const { return *this; }
			Iterator end() const { return Iterator(); }
		private:
			entt::entity m_current{ entt::null };
			entt::registry* Registry = nullptr;
		};

		Iterator begin(entt::registry& registry) const {
			return Iterator(next, &registry);
		}
		Iterator end() const { return Iterator(); }

		REFLECT()
	};

	class HierarchySystem : public Perry::System
	{
	public:
		void Init(entt::registry& reg) override;
		void ShutDown(entt::registry& reg) override;
		void Update(entt::registry& reg) override{};

	private:
		static void OnDestroy(entt::registry& reg, entt::entity e);
		REFLECT()
	};
}