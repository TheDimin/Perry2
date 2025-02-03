#include "EditorPCH.h"
#pragma hdrstop

#include "World/World.h"

using namespace Perry;

IMPLEMENT_REFLECT_TOOL(RegionInfoTool)
{
}
FINISH_REFLECT();

void RegionInfoTool::Draw()
{
	//	Zoom += ImGui::GetIO().MouseWheel *0.1f;

	ImGui::Text("zoom: %f", Zoom);

	static int activeRegionIndex = 0;
	static int activeTabIndex = 0;
	World& world = World::GetWorld();
	ImGui::ListBox(
		"Regions",
		&activeRegionIndex,
		[](void*, int indx) { return World::GetWorld().Regions[indx]->Name.c_str(); },
		nullptr,
		world.Regions.size());


	if (world.Regions.empty())
		return;

	auto& activeRegion = world.Regions[activeRegionIndex];
	GetEditor().SelectedRegistry = &activeRegion->Registry;
	ImGui::BeginTabBar("RegionOptions");

	if (ImGui::BeginTabItem("Entities"))
	{
		ImGui::BeginChild("SystemsRegion");
		viewer.Draw(activeRegion->GetRegistry());
		ImGui::EndChild();
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Systems"))
	{
		ImGui::BeginChild("SystemsRegion");
		DrawRegionSystems(*activeRegion);
		ImGui::EndChild();
		ImGui::EndTabItem();
	}


	if (ImGui::BeginTabItem("Info"))
	{
		float delta = activeRegion->GetRegionDeltaTime();
		ImGui::Text("LocalDeltaTime: %f", delta);

		ImGui::DragInt2("Location",&activeRegion->Location[0]);

		if (ImGui::Button("Save"))
		{
			World::GetWorld().Save("LOL");
		}

		if (ImGui::Button("Load"))
		{
			World::GetWorld().Load("LOL");
		}

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}
void RegionInfoTool::DrawRegionBox(const Region& region)
{
	glm::vec2 size = region.Size;
	glm::vec2 location = region.Location;

	// location -= size * 0.5f; // Render on top left

	location *= Zoom;
	size *= Zoom;

	location += Offset; // Scroll Offset
	location += glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

	ImRect rect{};
	rect.Translate(ImVec2(location.x, location.y));
	rect.Expand(ImVec2(size.x, size.y));

	ImU32 color = IM_COL32(255, 0, 0, 255); // Red with full opacity

	auto drawlist = ImGui::GetWindowDrawList();
	drawlist->AddRect(rect.Min, rect.Max, color, 0, 0, 1);
}

void DrawRegionRecursive(const Region& region, unsigned long long p)
{
	auto& graph = region.JobGraph;
	auto& jobs = region.SystemInstances;

	for (auto&& [outEdge, current] : graph.out_edges(p))
	{
	}
}

void RegionInfoTool::DrawRegionSystems(const Region& region)
{
	auto& graph = region.JobGraph;
	auto& jobs = region.SystemInstances;

	// ImGui::TreeNode("")
	for (auto&& vertex : graph.vertices())
	{
		auto& graphJob = jobs[vertex];
		/*
		auto jobName = graphJob.type().prop(p_DisplayName).value().cast<std::string>();

		auto vertexEdges = region.JobGraph.out_edges(vertex);

		// When it has no in-edges means we depend on nothing and can run at any time !
		if (vertexEdges.begin() == vertexEdges.end())
		{
			// The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
			// use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
			auto node_flags =
				ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
			ImGui::TreeNodeEx(jobName.c_str(), node_flags);
		}
		else
		{
			if (ImGui::TreeNode(jobName.c_str()))
			{
				for (auto edge : vertexEdges)
				{
					ImGui::Text("%llu -> %llu", edge.first, edge.second);

					auto& firstJob = jobs[edge.first];
					auto& secondJob = jobs[edge.second];

					auto firstJobName = firstJob.type().prop(p_DisplayName).value().cast<std::string>();
					auto secondJobName = secondJob.type().prop(p_DisplayName).value().cast<std::string>();
					ImGui::Text("%s -> %s", firstJobName.c_str(), secondJobName.c_str());

					DrawRegionRecursive(region, edge.first);
				}
				ImGui::TreePop();
			}
		}
		*/
		// ImGuiGraphNode::NodeGraphAddNode(graphJob.type().info().name().data());
	}

	// if (auto in_edges = region.JobGraph.in_edges(vertex); in_edges.begin() == in_edges.end())
	//{
	// ImGui::Text("Edge: %llu", vertex);
	//  ...
	//  ImGui::Text("%i",vertex);
	//}

	// ImGuiGraphNode::NodeGraphAddEdge()
	// ImGuiGraphNode::EndNodeGraph();
}