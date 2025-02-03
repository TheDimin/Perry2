#pragma once

namespace Perry
{
	class Mesh;
	class Model;
	class Texture;

	class MeshComponent
	{
	public:
		MeshComponent() = default;
		MeshComponent(const Resource<Model>& model, unsigned int meshIndex);

		Mesh& GetMesh()const;
		Resource<Model> GetModel()const { return m_Model; }

		int GetMaterialCount() const;
		Resource<Material> GetMaterial(unsigned int) const;
		void SetMaterial(unsigned int, const Resource<Material>&);

		unsigned GetBatchId() const { assert(BatchID != (unsigned)-1); return BatchID; }
	private:
		friend struct PrimitiveBatch;

		unsigned int BatchID = -1;

		Resource<Model> m_Model{};
		unsigned int m_MeshIndex = 0;
		std::vector<Resource<Material>> m_Materials{};
		REFLECT()
	};
}
