#pragma once

namespace Perry
{
	class TransformSystem :public Perry::System
	{
	public:
		TransformSystem();
		void Init(entt::registry& reg) override;
		void Update(entt::registry& reg) override;

		entt::observer updatedTransforms;

		REFLECT();
	};
}