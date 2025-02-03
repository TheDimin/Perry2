#pragma once

namespace Perry
{
	class Primitive;

	class SpriteComponent
	{
	public:
		SpriteComponent();
		SpriteComponent(const std::string& filePath, unsigned int framesInTexture = 1, float framesPerSecond = 1.0f, bool hasBlendTexture = false, glm::vec4 color = glm::vec4(1.f, 1.f, 1.f, 1.f), float emissiveFactor = 1.f);

		void SetAnimationStride(unsigned int startFrame, unsigned int endFrame, float animationSpeed);
		void SetTexture(const std::string& filepath, int slot);

		const unsigned int GetFrameCount() const { return m_FramesInTexture; }
		const unsigned int GetCurrentFrame() const { return m_CurrentFrame; }
		const unsigned int GetPreviousFrame() const { return m_PreviousFrame; }
		float GetFrameProgress() const;

		Resource<Material> m_Material;

		bool m_HasAnimationStride = false;
		unsigned int m_AnimationStart = 0;
		unsigned int m_AnimationEnd = 0;

		unsigned int m_FramesInTexture = 0;
		unsigned int m_CurrentFrame = 0;
		unsigned int m_PreviousFrame = 0;
		float m_AnimationSpeed = 0;
		float m_AnimationTimer = 0.0f;

		bool m_Paused = false;
		bool m_FlipX = false;
		bool m_FlipY = false;

		REFLECT()
	};

	class SpriteAnimationSystem : public System
	{
	public:
		void Init(entt::registry& reg) override;
		void Update(entt::registry& reg) override;

	protected:
		GraphInfo GetGraphBinding() override;
	public:
		REFLECT()
	};

}  // namespace Perry