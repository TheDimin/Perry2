#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

struct SerialReader
{
	void Entities(entt::entity* start, unsigned count)
	{
		for (unsigned i = 0; i < count; ++i)
		{
			LOG(LOG_SERIALIZER, "Loaded entity: %i", entt::to_integral(*(start + i)));
		}
		reg->create(start, start + count);
	}

	void ComponentStart(entt::meta_type& type, unsigned count) {}

	void ComponentInstance(entt::entity e, entt::meta_any& instance) {}

	void ComponentEnd() {}
	entt::registry* reg = nullptr;
};

struct JsonReader
{
	void Entities(std::vector<entt::entity>& e)
	{
		nlohmann::json entitiesObject = stream["Entities"];
		e.reserve(entitiesObject.size());

		for (int i = 0; i < entitiesObject.size(); ++i)
		{
			e.emplace_back(entitiesObject[i]);
		}
	}


	void ComponentTypeStart(entt::meta_type& type, unsigned& count)
	{
		componentIndex = 0;


		if (stream["Components"].size() <= TypeIndex)
		{
			type = entt::meta_type();
			return;
		}
		CurrentComponentObject = stream["Components"][TypeIndex];


		type = entt::resolve(CurrentComponentObject["typeID"].get<int>());
		count = CurrentComponentObject["Count"];
		TypeIndex++;
	}

	void ComponentInstance(entt::meta_any& instance, entt::entity& entityOut)
	{

		ComponentInstanceObject = CurrentComponentObject["Instances"][componentIndex];

		entityOut = entt::entity(ComponentInstanceObject["Owner"].get<int>());

		for (const auto& PropertyInstance : ComponentInstanceObject["Properties"])
		{
			entt::meta_any propInstance = instance.get(PropertyInstance["PropID"]);


			//If we ever decide to change a type of property this is where we detect it... for now, ideally we have a RevisionID, and then transfer it with something like protobuffer...
			//ASSERT_MSG(LOG_SERIALIZER, propInstance.type() == propType, "PropInstance and expected proptype do not match....");


			auto deserializeFunc = propInstance.type().func(f_Deserialize);

			ASSERT_MSG(LOG_SERIALIZER, deserializeFunc, "noob");

			auto propDeserializeFunc = deserializeFunc.invoke({}, PropertyInstance["PropData"], &propInstance);

			instance.set(PropertyInstance["PropID"], propInstance);
			ASSERT_MSG(LOG_SERIALIZER, propDeserializeFunc, "noob");

		}

		componentIndex++;
	}

	void ComponentEnd()
	{

	}


	entt::meta_any typeDefaultObject;

	nlohmann::json ComponentInstanceObject{};
	nlohmann::json CurrentComponentObject{};

	nlohmann::json stream = {};

	entt::registry* reg = nullptr;

	int TypeIndex = 0;
	int componentIndex = 0;
};


struct SerialWriter
{
	void Entities(const entt::entity* entities, unsigned count)
	{
		EntitiesInfo ei = EntitiesInfo(count);

		stream.write(reinterpret_cast<char*>(&ei), sizeof(EntitiesInfo));


		std::vector<entt::entity> ents = std::vector<entt::entity>();
		ents.reserve(count);
		for (auto first = entities, last = first + count; first != last; ++first) {

			//NOTE this uint16 is not garunteed to be correct, i just assume half of entt::Entity id is used for metadata, could be less.
			entt::entity newID = (entt::entity)entt::to_entity(*first);
			LOG(LOG_SERIALIZER, "Saving entity with id: %i", entt::to_integral(newID));

			stream.write(reinterpret_cast<char*>(&newID), sizeof(entt::entity));
		}
	}

	void ComponentStart(entt::meta_type& type, unsigned count)
	{
		auto compinfo = ComponentInfo(type.id(), count, type.info().name().data());
		typeDefaultObject = type.construct();

		stream.write(reinterpret_cast<const char*>(&compinfo), sizeof(ComponentInfo));
	}

	void ComponentInstance(entt::entity e, entt::meta_any& instance)
	{
		std::string instanceData = "";

		unsigned elementCount = 0;



		for (auto&& [propID, propMeta] : instance.type().data())
		{
			if (propMeta.prop(p_NotSerializable))
				continue;

			char* cdoProp = (char*)propMeta.get(typeDefaultObject).data();

			auto propType = propMeta.type();
			size_t propSize = propMeta.type().size_of();
			entt::meta_any propMetaInstance = propMeta.get(instance);
			char* PropPointer = (char*)propMetaInstance.data();

			if (memcmp(cdoProp, PropPointer, propSize) == 0)
				continue;

			PropertyHeader pl{ propID,propType.id() };

			instanceData += std::string(reinterpret_cast<const char*>(&pl), sizeof(PropertyHeader));

			instanceData += std::string(PropPointer, propSize);

			elementCount++;
		}

		ComponentInstanceInfo cii;
		cii.owner = e;
		cii.Count = elementCount;

		stream.write(reinterpret_cast<const char*>(&cii), sizeof(cii));

		if (elementCount != 0)
			stream.write(instanceData.c_str(), instanceData.size());
	}

	void ComponentEnd()
	{

	}

	std::string Dump()
	{
		return stream.str();
	}

	entt::meta_any typeDefaultObject;

	std::stringstream stream{};
};

struct JsonWriter
{
	void Entities(const entt::entity* entities, unsigned count)
	{
		nlohmann::json ents = nlohmann::json::array();
		for (auto first = entities, last = first + count; first != last; ++first) {

			//NOTE this uint16 is not garunteed to be correct, i just assume half of entt::Entity id is used for metadata, could be less.
			entt::entity newID = (entt::entity)entt::to_entity(*first);

			ents.emplace_back(newID);
		}

		stream["Entities"] = ents;
		stream["Components"] = nlohmann::json::array();
	}


	void ComponentStart(entt::meta_type& type, unsigned count)
	{
		CurrentComponentObject = {
			{"typeID" , type.id()},
			{"Count", count},
			{"DisplayName",type.info().name().data()},
			{"Instances", nlohmann::json::array()}
		};

		ComponentInstanceObject = &CurrentComponentObject["Instances"];

		typeDefaultObject = type.construct();
	}

	void ComponentInstance(entt::entity e, entt::meta_any& instance)
	{
		nlohmann::json Properties = nlohmann::json::array();
		for (auto&& [propID, propMeta] : instance.type().data())
		{
			if (propMeta.prop(p_NotSerializable))
				continue;

			char* cdoProp = (char*)propMeta.get(typeDefaultObject).data();

			auto propType = propMeta.type();
			size_t propSize = propMeta.type().size_of();
			entt::meta_any propMetaInstance = propMeta.get(instance);
			char* PropPointer = (char*)propMetaInstance.data();

			if (memcmp(cdoProp, PropPointer, propSize) == 0)
				continue;

			nlohmann::json elementData = {};
			auto f = propType.func(f_Serialize);
			ASSERT_MSG(LOG_SERIALIZER, f, "[%s] has no f_Serialize function???", propType.info().name().data());

			f.invoke({}, &elementData, propMetaInstance);

			if (elementData.empty())
			{
				ERROR(LOG_SERIALIZER, "DUM SERIALIZATION NOT WORKING");
			}

			Properties.push_back({
				{"PropID" , propID},
				{"PropType",propType.id()},
				{"PropData",elementData}//TODO this should be writting as string, not binary blob
				});
		}

		ComponentInstanceObject->push_back({ {"Owner",e},{"Properties",Properties} });
	}

	void ComponentEnd()
	{
		stream["Components"].push_back(CurrentComponentObject);
	}

	std::string Dump()
	{
		return stream.dump(1, '\t', false, nlohmann::detail::error_handler_t::ignore);
	}


	entt::meta_any typeDefaultObject;

	nlohmann::json* ComponentInstanceObject{};
	nlohmann::json CurrentComponentObject{};

	nlohmann::json stream = {};
};

void Perry::EnttSerializer::SaveRegistry(entt::registry& reg, const std::string& name)
{
	JsonWriter writer{};

	auto& estorage = reg.storage<entt::entity>();

	estorage.shrink_to_fit();
	writer.Entities(estorage.data(), static_cast<unsigned int>(estorage.size()));

	for (auto&& [typeID, metaInfo] : entt::resolve())
	{
		// Check if meta type is a component
		if (!metaInfo.prop(T_ReflectedComponent))
			continue;

		auto storagedata = reg.storage(typeID);

		if (storagedata == nullptr || storagedata->empty())
			continue;

		writer.ComponentStart(metaInfo,  static_cast<unsigned int>(storagedata->size()));

		for (auto entity : *storagedata)
		{
			auto voidInstance = storagedata->value(entity);
			auto metaInstance = metaInfo.from_void(voidInstance);

			writer.ComponentInstance(entity, metaInstance);
		}
		writer.ComponentEnd();
	}

	std::string data = writer.Dump();
	FileIO::WriteBinary(DirectoryType::Resource, "Levels/" + name + ".json", data.data(), data.size());
}

//{
//	"Revision" : 1,
//	"Data":[
////		{
//			"PropertyID" : 0000,
//			"Value" : "";
//	]
//}




void ObjectSerializer::SaveObject(entt::meta_any& instance, const std::string& name)
{
	nlohmann::json stream = {};

	auto revisionNum = instance.type().prop(p_SaveRevision);
	if (revisionNum)
		stream["Revision"] = revisionNum.value().cast<int>();

	stream["Data"] = nlohmann::json::array();

	for (auto&& [propID, propMeta] : instance.type().data())
	{
		if (propMeta.prop(p_NotSerializable))
			continue;

		nlohmann::json elementData = { {"PropID",propID} };
		auto prop = propMeta.get(instance);

		//if(propMeta.type().is_array())
		{
			//propMeta
			//for ()
			{
				
			}
		}
		//else
		{
			auto f = prop.type().func(f_Serialize);
			//assert(f);
			//ASSERT_MSG(LOG_SERIALIZER, f, "[%s] has no f_Serialize function???", instance.type().info().name().data());
			if(f)
			{
				nlohmann::json data;
				f.invoke(prop, &data, propMeta);
				elementData["Value"] = data;
			}

		}
		

		if (elementData.contains("Value"))
		{
			stream["Data"].emplace_back(elementData);
			//ERROR(LOG_SERIALIZER, "DUM SERIALIZATION NOT WORKING");
		}
	}


	std::string data = stream.dump(1,'\t');
	FileIO::WriteBinary(DirectoryType::Save, name + ".json", data.data(), data.size());
}

void ObjectSerializer::LoadObject(entt::meta_any& object, const std::string& name)
{
	JsonReader sr = JsonReader();
	sr.typeDefaultObject = object;

	auto jsonstring = FileIO::Read(DirectoryType::Save, name + ".json");

	sr.stream = nlohmann::json::parse(jsonstring);

	for (const auto& PropertyInstance : sr.stream["Data"])
	{
		entt::meta_any propInstance = object.get(PropertyInstance["PropID"]);

		auto f = propInstance.type().func(f_Deserialize);
		ASSERT_MSG(LOG_SERIALIZER, f, "noob");

		auto propDeserializeFunc = f.invoke({}, PropertyInstance["Value"], &propInstance);

		object.set(PropertyInstance["PropID"], propInstance);
		ASSERT_MSG(LOG_SERIALIZER, propDeserializeFunc, "noob");
	}
}


void Perry::EnttSerializer::LoadRegistry(entt::registry& reg, const std::string& name)
{
	JsonReader sr = JsonReader();
	sr.reg = &reg;

	auto jsonstring = FileIO::Read(DirectoryType::Resource, "Levels/" + name + ".json");

	sr.stream = nlohmann::json::parse(jsonstring);
	std::vector<entt::entity> e{};

	sr.Entities(e);

	reg.create(e.begin(), e.end());

	entt::meta_type currenType;
	unsigned count;
	sr.ComponentTypeStart(currenType, count);
	while (currenType)
	{
		for (unsigned i = 0; i < count; ++i)
		{
			entt::entity e;
			entt::meta_any instance = currenType.construct();
			sr.ComponentInstance(instance, e);

			auto f = instance.type().func(f_AddComponentSelf);
			auto llll = f.invoke({}, entt::forward_as_meta(reg), e, instance);
			ASSERT_MSG(LOG_SERIALIZER, llll, "Execution function failed !");
		}

		sr.ComponentEnd();

		sr.ComponentTypeStart(currenType, count);
	}
}