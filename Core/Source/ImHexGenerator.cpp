#include "Core.h"

ImHexMemTracker* GetTracker()
{
	static ImHexMemTracker* memTracker = new ImHexMemTracker();
	return memTracker;
}

void ImHexMemTracker::Mark(void* object, const char* name, std::size_t size)
{
	GetTracker()->markers.insert({ object,{name,size} });
}

void ImHexMemTracker::Forget(void* object)
{
	ImHexMemTracker* tracker = GetTracker();
	auto target = tracker->markers.find(object);
	if (target != tracker->markers.end())
	{
		tracker->markers.erase(target);
	}
}

std::string ImHexMemTracker::Dump()
{
	std::string builder;

	builder += "{\"bookmarks\":[";

	int i = 1;
	for (auto&& marker : GetTracker()->markers)
	{
		if (i != 1)
			builder += ",";
		std::string addressStr = std::to_string(reinterpret_cast<std::uintptr_t>(marker.first));

		builder += R"({"color": 1341756994,"comment": "\n","id": )" + std::to_string(i) +
			R"(,"locked": true,"name": ")" + marker.second.name + R"(","region": {"address":)" + addressStr + ",\"size\": " + std::to_string(marker.second.size) + "}}";

		i++;
	}

	builder += "]}";

	return builder;
}