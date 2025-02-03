#pragma once

namespace entt
{
	enum class entity :uint32_t;
}

template<typename T>
struct RangeView
{
	T* start;
	unsigned size;
};

class SaveFileViewer : public Perry::ToolBase
{
protected:
	void Init() override;
	void Draw() override;

public:
	REFLECT();
	void* sourceData = nullptr;

	RangeView<entt::entity> entitySource;
};