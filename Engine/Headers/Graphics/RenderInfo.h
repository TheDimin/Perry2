#pragma once

namespace entt
{
	enum class entity : std::uint32_t;
}

namespace Perry
{
	class SpotLightComponent;
	class DirectionalLightComponent;
	class PointLightComponent;

	struct PointLight
	{
		PointLight() = default;
		PointLight(const Perry::TransformComponent& transform, const PointLightComponent& lightSource);
		glm::vec3 m_Position;		//12
		float m_Intensity;		//4
		glm::vec3 m_Color;			//12
		float m_Padding[1]{ 0 };
	};

	struct DirectionalLight
	{
		DirectionalLight() = default;
		DirectionalLight(const Perry::TransformComponent& transform, const DirectionalLightComponent& lightSource);
		glm::vec3 m_Direction;		//12
		float m_Intensity;		//4
		glm::vec3 m_Color;			//16
		float m_Padding[1]{ 0 };
	};

	struct SpotLight
	{
		SpotLight() = default;
		SpotLight(const Perry::TransformComponent& transform, const SpotLightComponent& lightSource);

		glm::vec3 m_Position;
		glm::vec3 m_Color;
		glm::vec3 m_Direction;
		// glm::mat4 m_LightSpaceMat;
		float m_InnerAngle;
		float m_OuterAngle;
		float m_Intensity;
		// int32_t m_ShadowMapIndex = -1;
	};

	struct RenderLine {
		glm::vec3 Start;
		glm::vec3 Color;
		float _Padding[2];
	};

	struct Line {
		glm::vec3 Start;
		glm::vec3 End;
		glm::vec3 Color;
	};

	enum class TonemappingMode
	{
		LINEAR,
		REINHARD,
		REINHARDSQ,
		ACES_FILMIC
	};

	struct Tonemapping
	{
		TonemappingMode tonemappingMode = TonemappingMode::LINEAR;

		float m_Exposure = 0.2f;
		float m_Gamma = 1.9f;

		// Linear
		float m_MaxLuminance = 1.0f;

		// Reinhard
		float m_ReinhardConstant = 1.0f;

		// ACES
		float m_ShoulderStrength = 0.22f;
		float m_LinearStrength = 0.3f;
		float m_LinearAngle = 0.1f;
		float m_ToeStrength = 0.2f;
		float m_ToeNumerator = 0.01f;
		float m_ToeDenominator = 0.3f;
		float m_LinearWhite = 11.2f;
	};

	enum class FogMode
	{
		LINEAR,
		EXP,
		EXP2
	};

	struct Fog
	{
		bool m_Enabled = true;

		FogMode m_FogMode = FogMode::LINEAR;

		float m_Start = 80.0f;
		float m_End = 410.0f;
		float m_Density = 0.004f;

		glm::vec4 m_Color = glm::vec4(0.72f, 0.43f, 0.43f, 0.61f);
	};

	enum class SSAOQuality
	{
		LOW,
		MEDIUM,
		HIGH
	};

	struct SSAO
	{
		bool m_Enabled = false;

		SSAOQuality m_Quality = SSAOQuality::MEDIUM;
		float m_Radius = 0.6f;
		float m_Bias = 0.08f;
		float m_Intensity = 1.68f;
	};

	struct Bloom
	{
		bool m_Enabled = false;

		float m_Intensity = 1.1f;
		float m_Radius = 3.5f;
	};
	struct MonochromaticEffect
	{
		bool m_Enabled = false;
		float m_Threshold = 0.02f;
		glm::vec3 m_LitColor = glm::vec3(1.f, 0.f, 0.f);
		float m_Radius = 1300.f;
		glm::vec3 m_ExcludeColor = glm::vec3(0.f, 0.f, 0.f);
		float m_ExcludeColorRange = 0.0f;
	};
	struct Outlines
	{
		bool m_Enabled = false;
		float m_LineWidth = 1.f;
	};

	enum BlendObjectType {
		MESH,
		SPRITE,
		PARTICLE
	};

	struct BlendRenderObject {
		BlendObjectType m_Type;
		entt::entity m_Entity;
		int m_PrimitiveID; // used only for meshes
		const struct Particle* m_Particle; // used only for particle systems
	};

	struct SpriteAnimationData
	{
		int m_FrameCount = 1;
		int m_CurrentFrame = 1;
		int m_FlipX = false;
		int m_FlipY = false;



		/*	int m_PreviousFrame = 1;
			float m_FrameProgress = 0;
			int m_UsesBlending = false;
			float m_EmissiveFactor = 1;
			BOOL m_ClampSampling = false;

			BOOL m_InterpolateFrames = false;*/
			//float _PADDING[1] = { -1 };
	};


	struct TextureDebug
	{
		bool baseColor = false;
		bool normals = false;
		bool normalMap = false;
		bool metallic = false;
		bool roughness = false;
		bool emissive = false;
		bool occlusion = false;
	};

	struct InstanceData
	{
		glm::mat4 modelMatrix;
	};
}
