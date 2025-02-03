#pragma once

namespace Perry
{
	template<typename T>
	static auto to_entity(entt::registry& reg, const T& component)
	{
		return ::entt::to_entity(reg.storage<T>(), component);
	}

	//We use this custom Handle isntead of entt::Handle because of API compatiblity
	//Entity handle
	struct Entity
	{
		//Creates a invalid Entity
		Entity() = default;
		Entity(entt::registry* reg, const entt::entity& ID);

		~Entity() = default;
		Entity(const Entity& other) : entID(other.entID), Registry(other.Registry) {}
		Entity(Entity&& other) noexcept : entID(other.entID), Registry(other.Registry) {}
		Entity& operator=(const Entity& other)
		{
			if (this == &other)
				return *this;
			entID = other.entID;
			Registry = other.Registry;
			return *this;
		}
		Entity& operator=(Entity&& other) noexcept
		{
			if (this == &other)
				return *this;
			entID = other.entID;
			Registry = other.Registry;
			return *this;
		}

		//These are not exact equals !!! because they can belong to a other reg, but with the how the ID's work that should be rare.
		//TODO: if we ever run into issues with miss matching registeries, we should store a reg id in the id.
		bool operator!=(const entt::entity ent)const;
		bool operator==(const entt::entity ent)const;

		operator entt::entity() const { return entID; }

		bool Valid() const;

		Entity& SetName(const std::string& name);
		Entity& SetParent(entt::entity Parent);

		void LoadModel(const std::string& filePath);

		template <typename T, typename ... Args>
		decltype(auto) AddComponent(Args&&... args);

		template <typename T>
		decltype(auto) RemoveComponent();

		//Find a component for this entity
		//Note that these type of calls are slow compared to systems iteration
		template<typename... T>
		decltype(auto) FindComponent() const;

		void Delete();
		static Entity Create(entt::registry& reg, const glm::vec3& position = glm::vec3(0), const glm::vec3& rotation = glm::vec3(0), const glm::vec3& scale = glm::vec3(1));

		entt::registry& GetRegistry() const;
		entt::entity GetID() const { return entID; }
	private:
		entt::entity entID = entt::null;
		entt::registry* Registry = nullptr;
	};

	template <typename T, typename ... Args>
	decltype(auto) Entity::AddComponent(Args&&... args)
	{
		static_assert(std::is_constructible_v<T, Args...>, "Error: Cannot execute constructor with given parameters");

		return  GetRegistry().emplace_or_replace<T>(entID, std::forward<Args>(args)...);
	}
	template <typename T>
	decltype(auto) Entity::RemoveComponent()
	{
		return  GetRegistry().remove<T>(entID);
	}

	template <typename... T>
	inline 	decltype(auto) Entity::FindComponent() const
	{
		return GetRegistry().get<T...>(entID);
	}
}