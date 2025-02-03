#include "EnginePCH.h"
#pragma hdrstop


using namespace Perry;

Entity::Entity(entt::registry* reg, const entt::entity& ID) : Registry(reg), entID(ID)
{
}

bool Entity::operator!=(const entt::entity ent) const
{
	return entID != ent;
}

bool Entity::operator==(const entt::entity ent) const
{
	return entID == ent;
}

bool Entity::Valid() const
{
	if (!Registry)
		return false;

	return GetRegistry().valid(entID);
}

Entity& Entity::SetName(const std::string& name)
{
#ifndef SHIPPING
	Registry->replace<NameComponent>(entID, name);
#endif

	return *this;
}

Entity& Entity::SetParent(entt::entity Parent)
{
	Transform::SetParent(Parent, *this);
	return *this;
}

void Perry::Entity::LoadModel(const std::string& filePath)
{
	//try {
	Resource<Model> model;
	std::string path = filePath;
	if (filePath.find(".gltf") != std::string::npos)
	{
		INFO(LOG_GAMEOBJECTS, "LoadModel for '%s' called with file extension, this is depecrated !", filePath.c_str()); //
	}
	else {
		//path = filePath + (FileIO::Exist(DirectoryType::Resource, filePath + ".glb") ? ".glb" : ".gltf");
	}

	model = Perry::ResourceManager<Perry::Model>::Get(path);
	if (!model.IsLoading())
		model = Perry::ResourceManager<Perry::Model>::LoadAsync(path);

	Perry::Entity entityCopy = *this;

	model.OnLoad([entityCopy](Model* resource)
		{
			Model::LoadHierarchy(entityCopy, Perry::Resource(resource));
		});
	//}
	//catch (...)
	//{
	//	ERROR(LOG_GAMEOBJECTS, "UNKOWN ERROR DETECTED");
	//}
}

void Entity::Delete()
{
	Registry->destroy(entID);
}

Entity Entity::Create(entt::registry& reg, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
	std::mutex* mutex = reg.ctx().get<std::mutex*>();
	entt::entity e;
	{
		std::scoped_lock sl = std::scoped_lock(*mutex);
		e = reg.create();
	}

	Entity el(&reg, e);
	reg.emplace<TransformComponent>(el.entID, position, glm::quat(glm::radians(rotation)), scale);

#ifndef SHIPPING
	reg.emplace<NameComponent>(el.entID);
#endif
	return el;
}
entt::registry& Entity::GetRegistry() const
{
	ASSERT_MSG(LOG_GAMEOBJECTS, Registry, "Perry::Entity has no valid registery assigned to it!");
	return *Registry;
}