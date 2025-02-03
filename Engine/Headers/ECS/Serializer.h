#pragma once

namespace Perry
{

	struct EntitiesInfo
	{
		EntitiesInfo(unsigned Count) :count(Count) {}
		unsigned count;
	};

	struct PropertyHeader
	{
		unsigned propInstanceId;
		entt::id_type propertyTypeID;
	};

	struct ComponentInfo
	{
		ComponentInfo(entt::id_type id, unsigned count, const std::string& Name) :
			Id(id), Count(count)
		{
		}

		entt::id_type Id;
		unsigned Count;
	};

	struct ComponentInstanceInfo
	{
		entt::entity owner;
		unsigned Count; //We can optimize our serialization, by not writing default values to disk., this is why every instance can have a variable count
	};


	class TransformComponent;


	//TODO: Types should be able to have their own "Serialize" function to inject into serialization system. BUT by default run trough reflection!!!
	//TODO: support arrays..

	/// <summary>
	/// Custom serializer which tracks ID's Snapshots should be faster, but are not trustworthy when adding new types !
	///	This should work when adding new components and parameters. As the system tracks internally all the info that was saved
	/// </summary>
	class EnttSerializer
	{
	public:
		static void LoadRegistry(entt::registry& reg, const std::string& name);

		static void SaveRegistry(entt::registry& reg, const std::string& name);
	};

	class ObjectSerializer
	{
	public:
		static void LoadObject(entt::meta_any& object, const std::string& name);
		static void SaveObject(entt::meta_any& object, const std::string& name);
	};
	
} // namespace Perry