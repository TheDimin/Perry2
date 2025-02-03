#pragma once

namespace tinygltf
{
	class Model;
	struct Primitive;
}

namespace Perry
{
	class Model;
	/// <summary>
	/// Class that is able to setup its vertex/index buffer together with
	/// its required textures based on the passed along TinyglTF primitive.
	/// </summary>
	class Primitive
	{
	public:
		IMPLEMENT_MEM_DEBUG(Primitive)
		~Primitive();
		//TODO delete default constructor
		Primitive() = default;
		Primitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, Perry::Model& owningModel);

		/// <summary>
		/// Records all relative data (like vertex, index, texture) to the command
		/// list
		/// </summary>
		void Draw(unsigned Amount) const;

		unsigned VAO = 0;
		unsigned short m_MaterialIndex = -1;
		bool m_IndicesDraw = true;


		//render info
		int m_RenderMode = -1;
		size_t m_DataOffset = -1;
		int m_RenderDataType = -1;
		size_t m_RenderAmount = -1;


#ifdef RENDERER_DEPRECATED
		// DEPRECATED
		bool m_HasTangentsLoaded = false;
		const tinygltf::Primitive* m_tinygltfPrimitive;
#endif
	};

	class Mesh
	{
	public:
		IMPLEMENT_MEM_DEBUG(Mesh)
		Mesh() = default;

		std::vector<Primitive> m_Primitives;

		const glm::vec3& GetMinBounds()const { return m_Min; }
		const glm::vec3& GetMaxBounds()const { return m_Max; }

	private:
		friend class Model;
		glm::vec3 m_Min{};
		glm::vec3 m_Max{};
	};
}  // namespace Perry
