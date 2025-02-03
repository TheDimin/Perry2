#pragma once

namespace entt
{
	enum class entity : uint32_t;
}

namespace Perry
{
	//Idealyy we get rid of this, but its a solution to dealing with single threaded openGL crap
	struct ThreadingSolver
	{
		Concurrency::concurrent_queue<unsigned> releasableBuffers;
	};

	struct TextureDebug;
	struct Fog;
	struct SSAO;
	struct Bloom;
	struct MonochromaticEffect;
	struct Outlines;
	struct Tonemapping;
	struct BlendRenderObject;
	struct SpriteAnimationData;
	struct InstanceData;
	struct SpotLight;
	struct DirectionalLight;
	struct PointLight;
	struct Line;
	class Shader;
	class TextComponent;
	class Window;
	class Model;
	class TransformComponent;
	class Primitive;

	/// <summary>
	/// Cross-platform supported renderer, handles its own initialization
	/// and gives member functions to allow the input data for draw calls.
	/// </summary>
	class Renderer
	{
	public:
		Renderer(Perry::Window* window) :Window(window) {}
		Renderer() = default;
		~Renderer();
		Renderer(const Renderer& r) = delete;
		Renderer(Renderer&& r) = default;

		void PlatformInit();

		void Init();

		/// <summary>
		/// Indicates the start of a new frame, gets used to for example
		/// to indicate to ImGui that a new frame has started
		/// </summary>
		void StartFrame();


		/// <summary>
		/// Updates the renderer
		/// </summary>
		/// <param name="deltaTime"></param>
		/// <returns></returns>
		bool Update(float deltaTime);

		/// <summary>
		/// Initiates draw calls. All currently bound meshes,
		/// debug colliders and lines get drawn towards the screen.
		/// </summary>
		void Render();

		Perry::Window* Window;

		glm::vec4 m_ClearColorValue = glm::vec4(0/255.f, 0/255.f, 0/255.f, 0.f);

		//it would be ideal to deprecate all of these...
		Tonemapping m_Tonemapping;
		Fog m_Fog;
		SSAO m_SSAO;
		Bloom m_Bloom;
		MonochromaticEffect m_MonochromaticEffect;
		Outlines m_Outlines;

		float m_LightIntensityFactor = 1.0f;

		std::thread::id RenderThread;
	private:
		friend class Debug;
		void EndFrame();

		ArrayBuffer<InstanceData> instanceMeshBufferData = ArrayBuffer<InstanceData>("InstancingData");
		unsigned int m_SpriteSamplers[2]{};

		Perry::Resource<Shader> m_GbufferCollectShader;
		Perry::Resource<Shader> m_DefferredShading;
		Perry::Resource<ComputeShader> m_DefferredComputeShading;
		unsigned int m_GBuffer;
		Perry::Resource<Texture> m_BufferTextures[6];

		DebugRenderer debugRenderer;
		// Imgui API
		friend class EngineManager;
		void ImguiDraw();

		friend class BufferBase;
		ThreadingSolver commandBackLog;
	};
}  // namespace Perry
