#pragma once

namespace Perry
{
	class Shader;
	class Texture;
	class Model;

	/// <summary>
	/// TextureName used in material shader struct
	///	Note that this has to match order of KnownTextureType
	/// </summary>
	static const char* TextureSlotNameLookup[] = {
	"ambientTexture",	//Diffuse
	"normalTexture"	,	//Normal
	"MetallicRoughnessTexture", //metalicRoughness...
	"OcclusionTexture", //metalicRoughness...
	"EmissiveTexture",
		"ORMTexture",
	};

	struct MaterialCreator
	{
		glm::vec4 m_AlbedoFactor = glm::vec4(1.f, 1.f, 1.f, 1.f);
		glm::vec3 m_EmissiveFactor = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 m_SpecularFactor = glm::vec3(1.f, 1.f, 1.f);
		float m_NormalFactor = 1.f;

		float m_OcclusionFactor = 1.f;
		float m_MettalicFactor = 1.f;
		float m_RoughnessFactor = 1.f;

		bool m_UsesBlending = false;

		std::map<std::string, Resource<Texture>> m_Textures;
		Resource<Shader> m_Shader;
	};

	class Material : public IResourceType
	{
	public:
		~Material() =default;

		void SetSampler(unsigned int sampler, int index = -1);
		Resource<Perry::Texture> CreateTexture(const std::string& path, const tinygltf::Model& model, const Perry::Model& pModel, const tinygltf::Texture& texture);
		bool IsDefaultMaterial() const { return m_GltfDefault; }
		void Bind();
		void Bind(Resource<Shader> shader);
		
		void Load() override;
		void Unload() override;

		/// <summary>
		/// We are only considerd loaded when all textures,and shaders are loaded
		/// </summary>
		/// <returns></returns>
		bool IsLoaded() const override;

		void UseBlending(bool state);
		bool IsBlending()const { return m_UsesBlending; }

		std::map<std::string, Perry::Resource<Texture>> m_Textures;

		glm::vec4 m_AlbedoFactor = glm::vec4(1.f, 1.f, 1.f, 1.f);
		glm::vec3 m_EmissiveFactor = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 m_SpecularFactor = glm::vec3(1.f, 1.f, 1.f);
		float m_NormalFactor = 1.f;

		float m_OcclusionFactor = 1.f;
		float m_MettalicFactor = 1.f;
		float m_RoughnessFactor = 1.f;

		Resource<Shader> m_Shader;

		Material()=default;
		Material(const std::string& path, const MaterialCreator& matCreator);
		
		Material(const std::string& path);
		Material(const std::string& path, const Perry::Model& model, tinygltf::Material& material);
	private:
		//template <typename U> friend class ResourceManager;
		//Material() = delete;
		bool m_UsesBlending = false;

		bool m_GltfDefault = false;


	public:
		//Shader m_Shader;
		REFLECT();
	};
}
