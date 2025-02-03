#pragma once

namespace Perry
{
	struct Entity;
	class TransformComponent;
}
namespace tinygltf
{
	struct Primitive;
	class Model;
}

namespace entt
{
	enum class entity : std::uint32_t;
}

namespace Perry
{
	class Mesh;
	class Primitive;
	class Texture;

	/// <summary>
	/// Cross-platform model class. Receives a file path, uses it to load in 3D model data using TinyglTF. 
	/// </summary>
	class Model : public IResourceType
	{
	public:
		template <typename U> friend class ResourceManager;
		Model(const std::string& filename);
		~Model();

		void Load() override;
		void Unload() override;

		/// <returns>Amount of indices of the given model</returns>
		unsigned int GetIndicesCount() const { return m_IndicesCount; }

		/// <returns>Amount of mesh in the given model</returns>
		unsigned int GetMeshCount() const { return m_MeshCount; }

		const tinygltf::Model& GetTinyGltfModel() const;
		Mesh& GetMesh(unsigned int index);
		Primitive& GetPrimitiveAt(int meshIndex, int primitiveIndex);
		const auto GetMaterials() const { return m_Materials; }
		Resource<Material> GetMaterial(unsigned index) const;
		int GetMaterialCount() const { return int(m_Materials.size()); };
		void SetMaterial(const Resource<Material>& mat, unsigned index);
		void RemoveMaterial(unsigned int id) { m_Materials.erase(m_Materials.begin() + id); }

		static void LoadHierarchy(Perry::Entity e, const Resource<Model>& model);
		static void LoadRecursive(Perry::Entity& self, int node, const Resource<Model>& model);



		void InitializeBuffers(tinygltf::Model& model);
		void BuildModelTree(tinygltf::Model& model);
		tinygltf::Model* m_tinygltfModel;

		std::vector<unsigned int> m_Buffers;
	private:
		static Perry::TransformComponent CreateTransform(const tinygltf::Node& node);
		static void SetData(Perry::Entity& self,const tinygltf::Node& node,const Resource<Model>& model,const TransformComponent& transform);

		void CalculateTangents(const tinygltf::Model* model, const tinygltf::Primitive* prim);

		friend class Material;
		std::vector<Mesh> m_Meshes;
		std::vector<Resource<Texture>> m_Textures;
		std::vector<Resource<Material>> m_Materials;
		unsigned int m_IndicesCount = 0;
		unsigned int m_MeshCount = 0;

		REFLECT()
	};
}
