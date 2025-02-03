#include "EnginePCH.h"
#pragma hdrstop


using namespace Perry;

void Region::OnLoadInternal()
{
	// auto parentType = entt::resolve<System>();
	entt::flow builder{};

	std::unordered_map<unsigned, std::vector<entt::meta_type>> typeOrderMap{};

	unsigned min = 99999999;
	unsigned max = -99999;

	for (auto&& [id, metaType] : entt::resolve())
	{
		if (!metaType.is_class())
			continue;

		if (!metaType.prop(T_ReflectedSystem))
			continue;

		INFO(LOG_REGION, "FOUND a reflected system");


		auto newSystem = metaType.func(c_CreateSystem).invoke({});

		System* systemInstance = newSystem.cast<System*>();
		ASSERT_MSG(LOG_REGION, systemInstance, "ATTEMPTED to initialize a invalid system!");
		systemInstance->typeInfo = metaType;
		int systemPriority = systemInstance->ExecutionPriority;

		if (SystemInstances.empty())
		{
			SystemInstances.insert(SystemInstances.begin(), std::unique_ptr<System>(systemInstance));
		}
		else
		{
			[[maybe_unused]] bool insertedSystem = false;
			for (size_t i = 0; i < SystemInstances.size(); i++)
			{
				int p = SystemInstances[i]->ExecutionPriority;

				if (systemPriority >= p || i == SystemInstances.size() - 1)
				{
					SystemInstances.insert(SystemInstances.begin() + i, std::unique_ptr<System>(systemInstance));
					insertedSystem = true;
					break;
				}
			}

			assert(insertedSystem);
		}
	}

	for (auto& system : SystemInstances)
	{
		auto info = system->GetGraphBinding();
		using entt::literals::operator ""_hs;
		//TODO fix builder lol
		auto& flow = builder.bind(system->getType().id());
		flow.ro(info.Read.begin(), info.Read.end());
		flow.rw(info.Write.begin(), info.Write.end());
		flow.rw(system->getType().id());
		flow.ro(info.DependsOn.begin(), info.DependsOn.end());

		if (info.Sync)
			flow.sync();

		system->Registry = &Registry;
		system->Init(Registry);
	}


	ASSERT_MSG(LOG_GENERIC, !SystemInstances.empty(), "Failed to find system !");

	JobGraph = builder.graph();

	std::ostringstream output{};
	entt::dot(output,
		JobGraph,
		[this](auto& output, auto vertex)
		{
			std::unique_ptr<Perry::System>& system = SystemInstances[vertex];
			auto info = system->GetGraphBinding();
			//info.Write std::vector of resources we write to
			//info.Read std::vector of resources we read from

			// Start the output for the current system
			auto secondJobName = system->getType().prop(p_DisplayName).value().cast<std::string>();
			output << vertex << " [label=\"" << secondJobName << "\",shape=\"box\"];\n";
		});

	INFO(LOG_REGION, "JobGraph: %s", output.str().c_str());
}

entt::registry& Region::GetRegistry()
{
	return Registry;
}

/*IMPLEMENT_REFLECT_OBJECT(Region)
{
	meta.data<&Region::Name>("Name"_hs);
	meta.data<&Region::Location>("Location"_hs);
	meta.data<&Region::Size>("Size"_hs);
}
FINISH_REFLECT()*/

Region::Region() : Registry({}), Location(0), Size(100), Name("UNDEFINED")
{
	Registry.ctx().emplace_as< float>("DeltaTime"_hs, RegionDeltaTime);
	Registry.ctx().emplace_as<Region*>("Owner"_hs, this);
	Registry.ctx().emplace<std::mutex*>(new std::mutex());
	LastUpdateTime = std::chrono::system_clock::now();
}

Region::Region(const glm::ivec2& location, const glm::ivec2& size, const std::string& name) :
	Registry({}), Location(location), Size(size), Name(name)
{
	Registry.ctx().emplace_as< float>("DeltaTime"_hs, RegionDeltaTime);
	Registry.ctx().emplace_as<Region*>("Owner"_hs, this);
	Registry.ctx().emplace<std::mutex*>(new std::mutex());
	LastUpdateTime = std::chrono::system_clock::now();
}

Region::Region(Region&& other) noexcept
	:
	Registry(std::move(other.Registry)),
	JobGraph(std::move(other.JobGraph)),
	SystemInstances(std::move(other.SystemInstances)),
	Location(std::move(other.Location)),
	Size(std::move(other.Size)),
	Name(std::move(other.Name)),
	IsLoaded(other.IsLoaded),
	LastUpdateTime(std::move(other.LastUpdateTime)),
	RegionDeltaTime(other.RegionDeltaTime)
{
	Registry.ctx().emplace_as< float>("DeltaTime"_hs, RegionDeltaTime);
	Registry.ctx().emplace_as<Region*>("Owner"_hs, this);
	Registry.ctx().emplace<std::mutex*>(new std::mutex());
}

Region::~Region()
{
	for (auto& system : SystemInstances)
	{
		system->ShutDown(Registry);
	}

	Registry.clear();
	Registry = {};
}

void Region::WorldUpdate(float GlobalDeltaTime)
{
	//Process all the events from previous frames
	{
		Event pEvent;
		if(Events.try_pop(pEvent))
		{
			pEvent(Registry);
		}
	}

#ifdef REGION_DEBUG
	Debug::DrawBox(glm::vec3(0, 0, 0), GetRegionAuthorityBounds(), glm::vec3(0, 1, 0));
	Debug::DrawBox(glm::vec3(0, 0, 0), GetRegionMaxBounds(), glm::vec3(1, 0, 0));
#endif

	auto t1 = ::std::chrono::system_clock::now();
	RegionDeltaTime = (t1 - LastUpdateTime).count() * .00001f;
	LastUpdateTime = t1;

	//TODO fix being able to set this once and keep it around
	Registry.ctx().insert_or_assign("DeltaTime"_hs, RegionDeltaTime);

	UpdateSystems();
}

void Region::BeginExecute(unsigned long long vertex)
{
	auto& system = SystemInstances[vertex];

	//TODO this check should be part of the generator, not runtime
	if ((!GetEngine().GetGame().IsGamePaused() || system->RunInEditorMode) && system->IsEnabled)
	{
		Transform::SetRegistery(&Registry);
		system->Update(Registry);
	}

	std::vector<unsigned long long> awaits{};
	std::vector<Fiber*> fibers = std::vector<Fiber*>(JobGraph.size());
	for (auto outEdge : JobGraph.out_edges(vertex))
	{
		fibers[outEdge.second] = new Fiber([&](FiberContext& context)
			{
				BeginExecute(outEdge.second);
			}, nullptr);

		awaits.emplace_back(outEdge.second);
		fibers[outEdge.second]->Schedule();
	}
}

void Region::UpdateSystems()
{
#ifdef SYSTEM_THREADING
	std::vector<entt::adjacency_matrix<entt::directed_tag>::vertex_type> nextExecutions{};

	auto rootFibers = std::vector<Fiber*>(JobGraph.size());
	// First run root nodes
	for (auto vertex : JobGraph.vertices())
	{
		if (auto in_edges = JobGraph.in_edges(vertex); in_edges.begin() == in_edges.end())
		{
			// ...
			rootFibers[vertex] = new Fiber([vertex, &rootFibers, this](FiberContext& context)
				{
					BeginExecute(vertex);
				}, nullptr);

			rootFibers[vertex]->Schedule();
		}
	}
#else
	Transform::SetRegistery(&Registry);
	for (auto& system : SystemInstances)
	{
		system->Update(Registry);
	}

#endif
}

const float& Region::GetRegionDeltaTime()
{
	return Registry.ctx().get<float>("DeltaTime"_hs);
}

void Region::Save(const std::string& worldName, nlohmann::json& jobject)
{
	//The actual region entities have been saved in a different format already !

	jobject = {
		{"Name",Name},
		{"Location",{Location.x,Location.y}}
	};

	EnttSerializer::SaveRegistry(Registry, worldName + "/" + Name);

}

void Region::Load(const std::string& worldName, const nlohmann::json& jobject)
{
	// The actual region entities will be loaded from a different format !
	Name = jobject["Name"];
	Location = glm::ivec2(jobject["Location"][0], jobject["Location"][1]);

	EnttSerializer::LoadRegistry(Registry, worldName + "/" + Name);
}

glm::vec3 Region::GetRegionMaxBounds() const
{
	return glm::vec3(Size[0], 0, Size[1]) + glm::vec3(overlapExtend, 0, overlapExtend);
}

glm::vec3 Region::GetRegionAuthorityBounds()const
{
	return glm::vec3(Size[0], 0, Size[1]);
}