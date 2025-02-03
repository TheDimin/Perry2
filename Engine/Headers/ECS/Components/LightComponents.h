#pragma once


namespace Perry
{
	class PointLightComponent
	{
	public:
		PointLightComponent(const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f), float intensity = 1.0f);
		glm::vec4 Color;
		float Intensity;

	private:
		REFLECT()
	};

	class DirectionalLightComponent
	{
	public:
		DirectionalLightComponent(const glm::vec3& color = glm::vec3(1.f, 1.f, 1.f), float intensity = 1.0f);

		glm::vec4 Color;
		float Intensity;

		REFLECT()
	};

	class SpotLightComponent
	{
	public:
		SpotLightComponent(const glm::vec3& color = glm::vec3(1.f, 1.f, 1.f), float innerAngle = 4.f, float outerAngle = 2.f, float intensity = 10.0f);

		glm::vec4 Color;
		float InnerAngle;
		float OuterAngle;
		float Intensity;


		REFLECT()
	};

	class LightCalculationSystem : public Perry::System
	{
	public:
		LightCalculationSystem();
		void Init(entt::registry& reg) override;
		void Update(entt::registry& reg) override;

		void BindLights();

	protected:
		GraphInfo GetGraphBinding() override;
		entt::observer SpotLightObserver;
		entt::observer PointLightObserver;
		entt::observer DirectionalLightObserver;


		std::unique_ptr<MapBuffer<PointLight>> PointLightBuffer;
		std::unique_ptr<MapBuffer<DirectionalLight>> DirectionalLightBuffer;
		std::unique_ptr<MapBuffer<SpotLight>> SpotLightBuffer;

		REFLECT()
	};
} //namespace Perry