#include "EnginePCH.h"
#include "Graphics/GLTFLoader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#pragma hdrstop

using namespace Perry;

IMPLEMENT_REFLECT_OBJECT(Model)
{//Custom serialization for models

	meta.data<&Model::m_ResourcePath>("m_FilePath"_hs)
		PROP_DISPLAYNAME("filepath")
		PROP_READONLY;
}
FINISH_REFLECT()

inline glm::vec3 to_vec3(std::vector<double> array)
{
	return glm::vec3((float)array[0], (float)array[1], (float)array[2]);
}

inline glm::quat to_quat(std::vector<double> array)
{
	return glm::quat((float)array[3], (float)array[0], (float)array[1], (float)array[2]);
}

void DecomposeMatrix(const glm::mat4& transform, glm::vec3& translation, glm::vec3& scale, glm::quat& rotation)
{
	auto m44 = transform;
	translation.x = m44[3][0];
	translation.y = m44[3][1];
	translation.z = m44[3][2];


	scale.x = glm::length(glm::vec3(m44[0][0], m44[0][1], m44[0][2]));
	scale.y = glm::length(glm::vec3(m44[1][0], m44[1][1], m44[1][2]));
	scale.z = glm::length(glm::vec3(m44[2][0], m44[2][1], m44[2][2]));

	glm::mat4 myrot(
		m44[0][0] / scale.x, m44[0][1] / scale.x, m44[0][2] / scale.x, 0,
		m44[1][0] / scale.y, m44[1][1] / scale.y, m44[1][2] / scale.y, 0,
		m44[2][0] / scale.z, m44[2][1] / scale.z, m44[2][2] / scale.z, 0,
		0, 0, 0, 1
	);
	rotation = quat_cast(myrot);
}

struct UserLoadData
{
	std::string folderPath;
};


bool LoadImageDataFunc(tinygltf::Image* image, const int image_idx, std::string* err,
	std::string* warn, int req_width, int req_height,
	const unsigned char* bytes, int size, void* user)
{
	UserLoadData* uld = static_cast<UserLoadData*>(user);

	if (ResourceManager<Texture>::Get(uld->folderPath + image->name).IsLoaded())
	{
		image->mimeType = "Perry/Texture";
		image->uri = uld->folderPath + image->name;
		return true;
	}

	if (image->mimeType == "image/ktx2")
	{
		image->image.assign(bytes, bytes + size);
		printf("Ktx dected not supported ! \n");
		return false;
	}

	auto llll = tinygltf::LoadImageData(image, image_idx, err, warn, req_width, req_height, bytes, size, user);

	if (!warn->empty() || !err->empty())
		printf(" noob \n");


	return llll;
}

Model::Model(const std::string& filePath) :
	IResourceType(filePath.substr(FileIO::GetPath(DirectoryType::Resource).size())), m_Meshes({}), m_tinygltfModel(new tinygltf::Model)
{
	PROFILER_MARKC("Initialize Model","Graphics")
	tinygltf::TinyGLTF modelLoader{};
	UserLoadData uld{
		GetPath().substr(0,GetPath().find_last_of('/') + 1)
	};

	//modelLoader.SetImageLoader(&LoadImageDataFunc, &uld);
	std::string err;
	std::string warn;
	bool loaded = false;
	stbi_set_flip_vertically_on_load(false);
	if (GetPath().find(".glb") != std::string::npos)
	{
		loaded = modelLoader.LoadBinaryFromFile(m_tinygltfModel, &err, &warn, FileIO::GetPath(DirectoryType::Resource, GetPath()));
	}
	else if (GetPath().find(".gltf") != std::string::npos)
	{
		//ExpandFilePathFunction
		tinygltf::FsCallbacks callbacks{
			//	&tinygltf::FileExists,&ExpandFilePathFunction,&tinygltf::ReadWholeFile,&tinygltf::WriteWholeFile
		};
		//callbacks.ExpandFilePath = &ExpandFilePathFunction;
		//modelLoader.SetFsCallbacks(callbacks);
		loaded = modelLoader.LoadASCIIFromFile(m_tinygltfModel, &err, &warn, FileIO::GetPath(DirectoryType::Resource, GetPath()));
	}
	else
	{
		if (FileIO::Exist(DirectoryType::Resource, GetPath() + ".glb"))
		{
			loaded = modelLoader.LoadBinaryFromFile(m_tinygltfModel, &err, &warn, FileIO::GetPath(DirectoryType::Resource, GetPath() + ".glb"));
		}
		else
		{
			loaded = modelLoader.LoadASCIIFromFile(m_tinygltfModel, &err, &warn, FileIO::GetPath(DirectoryType::Resource, GetPath() + ".gltf"));
		}
	}



	if (!err.empty())
	{
		ERROR(LOG_GRAPHICS, "GLTF load returned error: %s\n	Model: %s", err.c_str(), GetPath().c_str());
		return;
	}

	if (!warn.empty())
	{

		ERROR(LOG_GRAPHICS, "GLTF load returned a warning '%s'\n	for file: %s", err.c_str(), GetPath().c_str());
	}

	if (!loaded)
	{

		ERROR(LOG_GRAPHICS, "Failed to load glTF '%s' but no error has been reported", GetPath().c_str());
		return;
	}
}

Model::~Model()
{
	delete m_tinygltfModel;
}

void Model::Load()
{
	PROFILER_MARKC("Upload model to GPU","Graphics")
	m_Meshes.resize(m_tinygltfModel->meshes.size());

	// Load meshes
	InitializeBuffers(*m_tinygltfModel);


	m_Materials.resize(m_tinygltfModel->materials.size());
	auto offset = GetPath().find_last_of('/');
	if (offset == std::string::npos)
		offset = GetPath().find_last_of('\\');

	auto filename = GetPath().substr(offset);
	// Load materials
	for (int i = 0; i < static_cast<int>(m_tinygltfModel->materials.size()); i++)
	{
		auto& tinyMaterial = m_tinygltfModel->materials[i];
		m_Materials[i] = (Perry::ResourceManager<Material>::Load(filename + "::M_" + tinyMaterial.name, *this, tinyMaterial));
	}

	BuildModelTree(*m_tinygltfModel);


	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		//m_IndicesCount += m_Meshes[i].GetIndicesCount();
	}

	m_MeshCount = static_cast<unsigned>(m_Meshes.size());

	CompleteLoad();
}

void Model::LoadHierarchy(Perry::Entity e, const Resource<Model>& model)
{
	if (model->m_tinygltfModel->defaultScene == -1)return;

	const auto& scene = model->m_tinygltfModel->scenes[model->m_tinygltfModel->defaultScene];

	//lets assume root and then model, we drop the root node
	if (scene.nodes.size() == 1 && model->m_tinygltfModel->nodes[0].children.size() == 0)
	{
		const tinygltf::Node& node = model->m_tinygltfModel->nodes[0];

		SetData(e, node, model, e.FindComponent<TransformComponent>());

		if (!e.FindComponent<NameComponent>().name.empty())
			e.SetName(scene.name);
		return;
	}
	else
	{
		for (const auto node : scene.nodes)
		{
			LoadRecursive(e, node, model);
		}
	}
	
	if (!e.FindComponent<NameComponent>().name.empty())
		e.SetName(scene.name);
}

const tinygltf::Model& Model::GetTinyGltfModel() const
{
	return *m_tinygltfModel;
};

Mesh& Model::GetMesh(unsigned index)
{
	return m_Meshes[index];
};

Primitive& Model::GetPrimitiveAt(int meshIndex, int primitiveIndex)
{
	return m_Meshes[meshIndex].m_Primitives[primitiveIndex];
};

Resource<Material> Model::GetMaterial(unsigned index) const
{
	return m_Materials[index];
}

void Model::SetMaterial(const Resource<Material>& mat, unsigned index)
{
	m_Materials[index] = mat;
}

void Model::LoadRecursive(Perry::Entity& self, int nodeID, const Resource<Model>& model)
{
	const tinygltf::Node& node = model->m_tinygltfModel->nodes[nodeID];


	auto childEntity = Entity::Create(self.GetRegistry());

	Transform::SetParent(self, childEntity,false);

	const TransformComponent& trans = CreateTransform(node);

	SetData(childEntity, node, model, trans);

	for (auto element : node.children)
	{
		LoadRecursive(childEntity, element, model);
	}
}
Perry::TransformComponent Model::CreateTransform(const tinygltf::Node& node)
{
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 rotation = glm::vec3(0.f);
	glm::vec3 scale = glm::vec3(1.f);
	glm::quat quat{};

	if (!node.matrix.empty())
	{
		glm::mat4 mat = glm::make_mat4(node.matrix.data());
		DecomposeMatrix(mat, position, scale, quat);
		rotation = quat* rotation; //used to be glm::rotate(
	}
	else
	{
		if (!node.translation.empty())
			position = to_vec3(node.translation);

		if (!node.rotation.empty() && node.rotation.size() == 3)
			rotation = to_vec3(node.rotation);

		if (!node.rotation.empty() && node.rotation.size() == 4)
		{
			quat = to_quat(node.rotation);
			rotation = quat* rotation; //used to be glm::rotate(
		}

		if (!node.scale.empty())
			scale = to_vec3(node.scale);
	}
	return TransformComponent(position, quat, scale);
}


void Model::SetData(Perry::Entity& self, const tinygltf::Node& node, const Resource<Model>& model, const TransformComponent& transform)
{
	self.GetRegistry().replace<TransformComponent>(self, transform);

	if (node.mesh >= 0)
	{
		if (!self.FindComponent<NameComponent>().name.empty())
			self.SetName(model->m_tinygltfModel->meshes[node.mesh].name);

		ImHexMemTracker::Dump();

		self.GetRegistry().emplace<MeshComponent>(self, model, node.mesh);
	}
	else
	{
		if (!self.FindComponent<NameComponent>().name.empty())
			self.SetName(node.name);
		// Extra's are not loaded from houdini... But this is how you would get them.
		// for (auto elements : node.extras.Keys())
		//{
		//	printf("%s :: %c \n", elements.c_str(), tt.extras.Get(elements).Type());
		//}

		for (auto element : node.extensions)
		{
			auto r = GLTFLoader::m_EntityExtensions.find(element.first);

			if (r != GLTFLoader::m_EntityExtensions.end())
			{
				r->second(self, *model->m_tinygltfModel, element.second);
			}
		}
	}

	{
		NameComponent& nameComponent = self.FindComponent<NameComponent>();
		std::string nodeName = nameComponent.name;
		std::stringstream stream(nodeName);
		std::string value{};

		// First element is string is the actual name
		std::getline(stream, value, '_');
		// Note(Damian): using this as actual name is nice but for now having the properties flags also shown is useful.
		// nameComponent.name = value;

		while (std::getline(stream, value, '_'))
		{
			auto r = GLTFLoader::m_EntityProperties.find(value);

			if (r != GLTFLoader::m_EntityProperties.end())
			{
				if (node.mesh < 0)
					r->second(self, *model->m_tinygltfModel, nullptr);
				else
					r->second(self, *model->m_tinygltfModel, &model->GetMesh(node.mesh));
			}
		}
	}
}