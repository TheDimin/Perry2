#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

/*IMPLEMENT_REFLECT_OBJECT(World)
{
	meta.data<&World::Regions>("Regions"_hs);
}
FINISH_REFLECT()*/

Perry::World::World()
{
}

void Perry::World::InitSystems()
{
	for (auto& region : Regions)
	{
		Transform::SetRegistery(&region->Registry);
		region->OnLoadInternal();
	}
	Transform::SetRegistery(nullptr);
}

void Perry::World::Update(float deltaTime)
{
#ifdef SYSTEM_THREADING
	//System threading moves fiber logic to the region itself
	for (auto& region : Regions)
	{
		region->WorldUpdate(deltaTime);
	}
#else
	auto regionFibers = std::vector<Fiber*>(Regions.size());

	for (int i = 0; i < Regions.size(); ++i)
	{
		Region* r = Regions[i];
		regionFibers[i] = new Fiber([r,deltaTime](FiberContext& context)
			{
				r->WorldUpdate(deltaTime);
			}, nullptr);

		regionFibers[i]->Schedule();
	}

#endif



	Transform::SetRegistery(nullptr);
}

std::vector<Perry::Region*>& Perry::World::ActiveRegions()
{
	return Regions;
}

Region* World::CreateRegion(const glm::ivec2& location, const glm::ivec2& size, const std::string& name)
{
	auto* nRegion = new Region(location, size, name);
	Regions.emplace_back(nRegion);

	Transform::SetRegistery(&nRegion->Registry);
	nRegion->OnLoadInternal();
	Transform::SetRegistery(nullptr);

	return Regions[Regions.size() - 1];
}

void Perry::World::Save(const std::string& path)
{
	nlohmann::json WorldSave = {
		{"Name",path},
		{"Regions",nlohmann::json::array()}
	};

	for (auto& region : Regions)
	{
		nlohmann::json obj;

		region->Save(path, obj);
		WorldSave["Regions"].emplace_back(obj);
	}


	FileIO::Write(DirectoryType::Resource, "Levels/" + path + "/WorldInfo" + ".json", WorldSave.dump(1, '\t'));
}

void Perry::World::Load(const std::string& path)
{
	auto jsonstring = FileIO::Read(DirectoryType::Resource, "Levels/" + path + "/WorldInfo" + ".json");
	nlohmann::json jData = nlohmann::json::parse(jsonstring);

	std::string name = jData["Name"];

	Regions.clear();

	for (const nlohmann::json& regionData : jData["Regions"])
	{
		Regions.emplace_back();
		Region* region = Regions[Regions.size() - 1];

		Transform::SetRegistery(&region->Registry);
		region->OnLoadInternal();
		region->Load(name, regionData);

		Transform::SetRegistery(nullptr);
	}
}

Perry::World* Perry::World::CreateWorld()
{
	World* world = new World;
	Region* region = world->CreateRegion(glm::ivec2(0, 0), glm::ivec2(500, 500), "default");
	auto& regionReg = region->Registry;

	Transform::SetRegistery(&regionReg);
	auto camE = Entity::Create(regionReg, Forward() * -2.f, glm::vec3(), glm::vec3(1, 1, 1));
	camE.AddComponent<NameComponent>("EditorCamera");
	camE.AddComponent<CameraComponent>();
	camE.AddComponent<FlyingMovementComponent>(0.1f, 0.5f);

	Camera::SetCamera({ &regionReg,camE });

	auto Sun = Entity::Create(regionReg);
	Sun.AddComponent<DirectionalLightComponent>(glm::vec3(1), 4.f);
	regionReg.patch<DirectionalLightComponent>(Sun);
	Sun.SetName("Sun");

	auto mode = Entity::Create(regionReg, glm::vec3(), glm::vec3(), glm::vec3(5, 5, 5));
	mode.LoadModel("Models/Avocado/Avocado.gltf");
	mode.SetName("Avocado");

	return world;
}
Perry::World& Perry::World::GetWorld()
{
	ASSERT_MSG(LOG_ARCHIVE, ActiveWorld, "No active world found... this should not be possible");
	return *ActiveWorld;
}