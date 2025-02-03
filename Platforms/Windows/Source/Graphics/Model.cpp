#include "pch.h"
#pragma hdrstop

using namespace Perry;

void Model::BuildModelTree(tinygltf::Model& model)
{
	for (int meshID = 0; meshID < static_cast<int>(model.meshes.size()); meshID++)
	{
		auto& tinyMesh = model.meshes[meshID];

		double BoundsMin[3]{ 999999999999 };
		double BoundsMax[3]{ -999999999999 };

		std::vector<GLuint> vaoReserve = std::vector<GLuint>(tinyMesh.primitives.size());
		glGenVertexArrays((GLsizei)tinyMesh.primitives.size(), vaoReserve.data());
		m_Meshes[meshID].m_Primitives.resize(tinyMesh.primitives.size());
		for (size_t primItt = 0; primItt < tinyMesh.primitives.size(); primItt++)
		{
			const auto& tinyPrim = tinyMesh.primitives[primItt];

			m_Meshes[meshID].m_Primitives[primItt] = Perry::Primitive(model, tinyPrim, *this);
			auto& PerryPrim = m_Meshes[meshID].m_Primitives[primItt];

			PerryPrim.VAO = vaoReserve[primItt];

			glBindVertexArray(PerryPrim.VAO);

			{
				//glGetAttribLocation //glGetUniformLocation

				const auto& accessor = model.accessors[tinyPrim.attributes.find("POSITION")->second];
				const auto& bufferView = model.bufferViews[accessor.bufferView];

				glBindBuffer(bufferView.target, m_Buffers[accessor.bufferView]);

				glVertexAttribPointer(0,
					tinygltf::GetNumComponentsInType(accessor.type),
					accessor.componentType,
					accessor.normalized,
					(GLsizei)bufferView.byteStride, (void*)(accessor.byteOffset));
				glEnableVertexAttribArray(0);

				assert(!accessor.sparse.isSparse);

				if (accessor.type == TINYGLTF_TYPE_VEC3)
				{
					for (int i = 0; i < 3; ++i)
					{
						if (accessor.minValues[i] < BoundsMin[i])
							BoundsMin[i] = accessor.minValues[i];

						if (accessor.maxValues[i] > BoundsMax[i])
							BoundsMax[i] = accessor.maxValues[i];
					}
				}
			}

			if (tinyPrim.attributes.find("TEXCOORD_0") != tinyPrim.attributes.end())
			{
				//glGetAttribLocation //glGetUniformLocation

				const auto& accessor = model.accessors[tinyPrim.attributes.find("TEXCOORD_0")->second];
				const auto& bufferView = model.bufferViews[accessor.bufferView];

				glBindBuffer(bufferView.target, m_Buffers[accessor.bufferView]);

				glVertexAttribPointer(1,
					tinygltf::GetNumComponentsInType(accessor.type),
					accessor.componentType,
					accessor.normalized,
					(GLsizei)bufferView.byteStride, (void*)(accessor.byteOffset));
				glEnableVertexAttribArray(1);

				assert(!accessor.sparse.isSparse);
			}
			else {
				printf("No valid textureCoord\n");

				if (tinyPrim.attributes.find("COLORS_0") != tinyPrim.attributes.end())
				{
					//glGetAttribLocation //glGetUniformLocation

					const auto& accessor = model.accessors[tinyPrim.attributes.find("COLORS_0")->second];
					const auto& bufferView = model.bufferViews[accessor.bufferView];

					glBindBuffer(bufferView.target, m_Buffers[accessor.bufferView]);

					glVertexAttribPointer(2,
						tinygltf::GetNumComponentsInType(accessor.type),
						accessor.componentType,
						accessor.normalized,
						(GLsizei)bufferView.byteStride,
						(void*)(accessor.byteOffset));
					glEnableVertexAttribArray(2);

					assert(!accessor.sparse.isSparse);
				}
			}

			if (tinyPrim.attributes.find("NORMAL") != tinyPrim.attributes.end())
			{
				//glGetAttribLocation //glGetUniformLocation

				const auto& accessor = model.accessors[tinyPrim.attributes.find("NORMAL")->second];
				const auto& bufferView = model.bufferViews[accessor.bufferView];

				glBindBuffer(bufferView.target, m_Buffers[accessor.bufferView]);

				glVertexAttribPointer(2,
					tinygltf::GetNumComponentsInType(accessor.type),
					accessor.componentType,
					accessor.normalized,
					(GLsizei)bufferView.byteStride, (void*)(accessor.byteOffset));
				glEnableVertexAttribArray(2);


				assert(!accessor.sparse.isSparse);
			}
			else
				printf("No valid NORMAL\n");


			if (tinyPrim.attributes.find("TANGENT") != tinyPrim.attributes.end())
			{
				//glGetAttribLocation //glGetUniformLocation

				const auto& accessor = model.accessors[tinyPrim.attributes.find("TANGENT")->second];
				const auto& bufferView = model.bufferViews[accessor.bufferView];

				glBindBuffer(bufferView.target, m_Buffers[accessor.bufferView]);

				glVertexAttribPointer(3,
					tinygltf::GetNumComponentsInType(accessor.type),
					accessor.componentType,
					accessor.normalized,
					(GLsizei)bufferView.byteStride, (void*)(accessor.byteOffset));
				glEnableVertexAttribArray(3);
			}
			else
			{
				CalculateTangents(&model, &tinyPrim);
				INFO(LOG_GRAPHICS, "No valid Tangent on %s", GetPath().c_str());
			}

			if (tinyPrim.indices != -1)
			{
				const auto& accessor = model.accessors[tinyPrim.indices];

				const auto& IndiceView = model.bufferViews[accessor.bufferView];

				glBindBuffer(IndiceView.target, m_Buffers[accessor.bufferView]);

				assert(!accessor.sparse.isSparse);
			}
		}
		//We current need thsi line. TODO fix requirement
		glBindVertexArray(0);

		m_Meshes[meshID].m_Max = glm::vec3(BoundsMax[0], BoundsMax[1], BoundsMax[2]);
		m_Meshes[meshID].m_Min = glm::vec3(BoundsMin[0], BoundsMin[1], BoundsMin[2]);
	}
}

size_t CalculateIndex(const tinygltf::BufferView& bufferView, const tinygltf::Accessor& accessor, size_t index)
{
	assert(index <= accessor.count);
	int byteStride = accessor.ByteStride(bufferView);
	assert(byteStride != -1);
	size_t findex = (bufferView.byteOffset + accessor.byteOffset) + byteStride * index;
	assert(findex <= (bufferView.byteOffset + accessor.byteOffset + bufferView.byteLength));
	return findex;
}

void Model::CalculateTangents(const tinygltf::Model* model, const tinygltf::Primitive* prim)
{
	if (prim->attributes.find("TEXCOORD_0") == prim->attributes.end())
	{
		printf("Failed to find texCoord for model, how do we calculate tangents now ????\n");
		return;
	}

	if (prim->attributes.find("NORMAL") == prim->attributes.end())
	{
		printf("Failed to find NORMAL for model, how do we calculate tangents now ????\n");
		return;
	}


	const auto& indicesAccessor = model->accessors[prim->indices];
	const auto& indicesBufferView = model->bufferViews[indicesAccessor.bufferView];
	const auto& indicesBuffer = model->buffers[indicesBufferView.buffer];
	const auto indiceSize = tinygltf::GetComponentSizeInBytes(indicesAccessor.componentType);

	const auto& positionAccessor = model->accessors[prim->attributes.find("POSITION")->second];
	const auto& positionBufferView = model->bufferViews[positionAccessor.bufferView];
	const auto& positionBuffer = model->buffers[positionBufferView.buffer];

	const auto& texCoordAccessor = model->accessors[prim->attributes.find("TEXCOORD_0")->second];
	const auto& texCoordBufferView = model->bufferViews[texCoordAccessor.bufferView];
	const auto& texCoordBuffer = model->buffers[texCoordBufferView.buffer];

	const auto& normalAccessor = model->accessors[prim->attributes.find("NORMAL")->second];
	const auto& normalBufferView = model->bufferViews[normalAccessor.bufferView];
	const auto& normalBuffer = model->buffers[normalBufferView.buffer];

	assert(prim->indices != -1); //TODO handle non indexed

	GLuint gpuBuffer;
	glCreateBuffers(1, &gpuBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gpuBuffer);
	glObjectLabel(GL_BUFFER, gpuBuffer, -1, "CalculatedTangents");
	size_t bufferSize = sizeof(glm::vec4) * indicesAccessor.count;
	glm::vec4* data = new glm::vec4[indicesAccessor.count];
	m_Buffers.emplace_back(gpuBuffer);

	glm::vec3 pos[3]{};
	glm::vec3 normal[3]{};
	glm::vec2 uv[3]{};
	for (size_t i = 0; i < indicesAccessor.count; i += 3)
	{
		for (int ii = 0; ii < 3; ++ii)
		{
			const auto& it = *reinterpret_cast<const unsigned char*> (&indicesBuffer.data[CalculateIndex(indicesBufferView, indicesAccessor, i + ii)]);
			pos[ii] = *reinterpret_cast<const glm::vec3*>(&positionBuffer.data[CalculateIndex(positionBufferView, positionAccessor, it)]);
			normal[ii] = *reinterpret_cast<const glm::vec3*>(&normalBuffer.data[CalculateIndex(normalBufferView, normalAccessor, it)]);
			uv[ii] = *reinterpret_cast<const glm::vec2*>(&texCoordBuffer.data[CalculateIndex(texCoordBufferView, texCoordAccessor, it)]);
		}

		float x1 = pos[1].x - pos[0].x;
		float x2 = pos[2].x - pos[0].x;
		float y1 = pos[1].y - pos[0].y;
		float y2 = pos[2].y - pos[0].y;
		float z1 = pos[1].z - pos[0].z;
		float z2 = pos[2].z - pos[0].z;

		float s1 = uv[1].x - uv[0].x;
		float s2 = uv[2].x - uv[0].x;
		float t1 = uv[1].y - uv[0].y;
		float t2 = uv[2].y - uv[0].y;

		float rdiv = s1 * t2 - s2 * t1;
		float r;
		if (rdiv == 0.0f)
		{
			r = 0.0f;
		}
		else
		{
			r = 1.0f / rdiv;
		}

		glm::vec3 t = glm::vec3(
			(t2 * x1 - t1 * x2) * r,
			(t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r
		);

		glm::vec3 tan2 = glm::vec3(
			(s1 * x2 - s2 * x1) * r,
			(s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r
		);

		for (int ii = 0; ii < 3; ++ii)
		{
			const glm::vec3& n = normal[ii];

			glm::vec3 xyz = t - (n * glm::dot(n, t));
			if (glm::length(xyz) > 0.00001f)
			{
				xyz = glm::normalize(xyz);
			}

			float w;
			if (glm::dot(glm::cross(n, t), tan2) < 0.0f)
			{
				w = -1.0f;
			}
			else
			{
				w = 1.0f;
			}

			data[i + ii] = glm::vec4(xyz, w);

			assert(bufferSize > i + ii);
		}
	}

	glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
	delete[] data;

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(3);
}

void Model::InitializeBuffers(tinygltf::Model& model)
{
	m_Buffers.resize(model.bufferViews.size());
	//glGenBuffers(static_cast<GLsizei>(model.buffers.size()), m_Buffers.data());

	for (int i = 0; i < model.bufferViews.size(); ++i)
	{
		auto& bufferview = model.bufferViews[i];
		auto& buffer = model.buffers[bufferview.buffer];
		//TODO this is how we detect if its a texture for now... IMO its bad... whats the right way tho ?
		if (bufferview.target == 0)
		{
			INFO(LOG_GRAPHICS, "Texture buffer found ! \n");
			//Created below.. not here.
		}
		else
		{
			GLuint gpuBuffer;
			glCreateBuffers(1, &gpuBuffer);
			glBindBuffer(bufferview.target, gpuBuffer);

			glObjectLabel(GL_BUFFER, gpuBuffer, (GLsizei)buffer.name.length(), buffer.name.c_str());


			glBufferData(bufferview.target, static_cast<long long>(bufferview.byteLength), &buffer.data[bufferview.byteOffset], GL_STATIC_DRAW);
			m_Buffers[i] = gpuBuffer;
		}
	}
}


void Model::Unload()
{
	delete m_tinygltfModel;
	m_Meshes.clear();
	m_Textures.clear();
	m_Materials.clear();
	m_IndicesCount = 0;
	m_MeshCount = 0;

	glDeleteBuffers(static_cast<unsigned int>(m_Buffers.size()), m_Buffers.data());
	m_Buffers.clear();
}