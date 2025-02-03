#include "pch.h"
#pragma hdrstop


static std::unordered_map<int, Perry::Graphics::TextureChannels> PixelFormatLookUp{
	{GL_RGBA,Perry::Graphics::TextureChannels::RGBA}
};

Perry::Resource<Perry::Texture> Perry::Material::CreateTexture(const std::string& path, const tinygltf::Model& model, const Perry::Model& pModel, const tinygltf::Texture& texture)
{
	if (ResourceManager<Perry::Texture>::IsLoaded(path))
	{
		return ResourceManager<Perry::Texture>::Get(path);
	}


	const tinygltf::Image* image = nullptr;
	bool compressedTexture = texture.extensions.find("KHR_texture_basisu") != texture.extensions.end();
	if (compressedTexture)
	{
		int imageIndex = texture.extensions.find("KHR_texture_basisu")->second.GetNumberAsInt();
		image = &model.images[imageIndex];
#ifdef KTK_TEXTURE
		auto& bufferview = model.bufferViews[image->bufferView];
		auto& buffer = model.buffers[bufferview.buffer];

		ktxTexture* kTexture;
		KTX_error_code result;
		//ktx_size_t offset;
		//ktx_uint8_t* image;
		//ktx_uint32_t level, layer, faceSlice;
		GLuint gltexture = 0;
		GLenum target, glerror;

		result = ktxTexture_CreateFromMemory(image->image.data(), image->image.size(),
			KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
			&kTexture);

		result = ktxTexture_GLUpload(kTexture, &gltexture, &target, &glerror);

		ktxTexture_Destroy(kTexture);
		// ...
		// GL rendering using the texture
		// ...


		return ResourceManager<Perry::Texture>::Get(m_tMaterial->name + std::to_string(imageIndex), gltexture);
#endif
	}

	image = &model.images[texture.source];


	if (image->mimeType == "Perry/Texture")
	{
		return ResourceManager<Perry::Texture>::Get(image->uri);
	}

	auto textureConstruct = Graphics::TextureConstruct();

	textureConstruct.data = image->image.data();
	textureConstruct.m_Width = image->width;
	textureConstruct.m_Height = image->height;
	textureConstruct.m_Format = Perry::Graphics::TextureChannels::RGBA;
	textureConstruct.m_GpuType = !compressedTexture ? image->pixel_type : TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;

	if (texture.sampler != -1)
	{
		const auto& sampler = model.samplers[texture.sampler];
		if (sampler.minFilter != -1)
			textureConstruct.m_Filters.m_Min = (Graphics::Distance)sampler.minFilter;
		if (sampler.magFilter != -1)
			textureConstruct.m_Filters.m_Mag = (Graphics::Distance)sampler.magFilter;
	}
	else
	{
		textureConstruct.m_WrapMode = Graphics::WrapMode::REPEAT;
	}


	return ResourceManager<Perry::Texture>::Load(path, textureConstruct);
}

void Perry::Material::Load()
{

	INFO(LOG_RESOURCE, "Materials don't use load/unload yet...");
	CompleteLoad();
}

void Perry::Material::Unload()
{
}

bool Perry::Material::IsLoaded() const
{
	for (auto&& [textureName, texture] : m_Textures)
	{
		if (!texture.IsLoaded())
			return false;
	}

	return /*m_Shader.IsLoaded() &&*/ IResourceType::IsLoaded();
}

void Perry::Material::UseBlending(bool state)
{
	if (state == m_UsesBlending) return;

	if (IsDefaultMaterial())
	{
		INFO(LOG_GRAPHICS, "Attempted to set blending on default mat\n");
		return;
	}

	//GetEngine().GetLevel().GetSystem<RenderBatcherSystem>()->OnMaterialBlendChange(this);
	m_UsesBlending = state;
}

std::string GetTexturePath(const Perry::Model& pModel, const tinygltf::Material& m_tMaterial, const tinygltf::Texture& texture)
{
	const tinygltf::Image* image = nullptr;
	bool compressedTexture = texture.extensions.find("KHR_texture_basisu") != texture.extensions.end();
	if (compressedTexture)
	{
		int imageIndex = texture.extensions.find("KHR_texture_basisu")->second.GetNumberAsInt();
		image = &pModel.m_tinygltfModel->images[imageIndex];

	}
	else
		image = &pModel.m_tinygltfModel->images[texture.source];

	//This is bit tricky sometimes texture has no Uri, its stored inside the buffer.
//Name is also not garunteed... yet we want a valid indentifier....

//Idealy we use filesystem path but not allowed on switch...

	auto lo = pModel.GetPath().find_last_of('\\');
	if (lo == std::string::npos)
		lo = pModel.GetPath().find_last_of('/');

	std::string texturePath = pModel.GetPath().substr(0, lo + 1);

	if (!image->uri.empty())
	{
		texturePath += image->uri;
	}
	else if (!image->name.empty())
	{
		texturePath += image->name;

		if (image->mimeType == "image/jpeg")
		{
			texturePath += ".jpg";
		}
		else if (image->mimeType == "image/png")
		{
			texturePath += ".png";
		}
		else
		{
			ERROR(LOG_FILEIO, "UNKOWN MIME TYPE: %s");
		}//
	}
	else if (!image->uri.empty())
	{
		texturePath += image->uri;
	}
	else
	{
		if (texture.name.empty())
			texturePath += m_tMaterial.name + std::to_string(texture.source);
		else
			texturePath += texture.name;
	}
	return texturePath;
}

Perry::Material::Material(const std::string& path) : IResourceType(path)
{
	printf("Unsuported material type !\n");
}

Perry::Material::Material(const std::string& path, const MaterialCreator& matCreator) :
	IResourceType(path), m_AlbedoFactor(matCreator.m_AlbedoFactor), m_EmissiveFactor(matCreator.m_EmissiveFactor),
	m_SpecularFactor(matCreator.m_SpecularFactor), m_NormalFactor(matCreator.m_NormalFactor),
	m_OcclusionFactor(matCreator.m_OcclusionFactor), m_MettalicFactor(matCreator.m_MettalicFactor), m_RoughnessFactor(matCreator.m_RoughnessFactor),
	m_UsesBlending(matCreator.m_UsesBlending),
	m_Textures(matCreator.m_Textures), m_Shader(matCreator.m_Shader)
{

}

Perry::Material::Material(const std::string& path, const Perry::Model& model, tinygltf::Material& material)
	: IResourceType(path), m_GltfDefault(true)
{
	if (material.pbrMetallicRoughness.baseColorTexture.index != -1)
	{
		const auto& baseColor = material.pbrMetallicRoughness.baseColorTexture;
		m_AlbedoFactor = glm::vec4(material.pbrMetallicRoughness.baseColorFactor[0], material.pbrMetallicRoughness.baseColorFactor[1], material.pbrMetallicRoughness.baseColorFactor[2], material.pbrMetallicRoughness.baseColorFactor[3]);
		assert(baseColor.texCoord == 0);
		const auto& texture = model.GetTinyGltfModel().textures[baseColor.index];

		std::string texturePath = GetTexturePath(model, material, texture);

		m_Textures.insert({ TextureSlotNameLookup[0],CreateTexture(texturePath,model.GetTinyGltfModel(),model, texture) });
	}

	if (material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
	{
		const auto& textureInfo = material.pbrMetallicRoughness.metallicRoughnessTexture;
		assert(textureInfo.texCoord == 0);
		const auto& texture = model.GetTinyGltfModel().textures[textureInfo.index];
		std::string texturePath = GetTexturePath(model, material, texture);

		m_Textures.insert({ TextureSlotNameLookup[2],CreateTexture(texturePath,model.GetTinyGltfModel(),model, texture) });
	}

	if (material.normalTexture.index != -1)
	{
		const auto& textureInfo = material.normalTexture;
		assert(textureInfo.texCoord == 0);
		const auto& texture = model.GetTinyGltfModel().textures[textureInfo.index];
		std::string texturePath = GetTexturePath(model, material, texture);

		m_Textures.insert({ TextureSlotNameLookup[1],CreateTexture(texturePath,model.GetTinyGltfModel(),model, texture) });
	}

	if (material.occlusionTexture.index != -1)
	{
		const auto& textureInfo = material.occlusionTexture;
		assert(textureInfo.texCoord == 0);
		const auto& texture = model.GetTinyGltfModel().textures[textureInfo.index];
		std::string texturePath = GetTexturePath(model, material, texture);

		m_Textures.insert({ TextureSlotNameLookup[3],CreateTexture(texturePath,model.GetTinyGltfModel(),model, texture) });
	}

	if (material.emissiveTexture.index != -1)
	{
		const auto& textureInfo = material.emissiveTexture;
		m_EmissiveFactor = glm::vec3(material.emissiveFactor[0], material.emissiveFactor[1], material.emissiveFactor[2]);
		assert(textureInfo.texCoord == 0);
		const auto& texture = model.GetTinyGltfModel().textures[textureInfo.index];
		std::string texturePath = GetTexturePath(model, material, texture);

		m_Textures.insert({ TextureSlotNameLookup[4],CreateTexture(texturePath,model.GetTinyGltfModel(),model, texture) });
	}

	if (ResourceManager<Shader>::IsLoaded("Mesh"))
	{
		m_Shader = ResourceManager<Shader>::Get("Mesh");
	}
	else
	{
		m_Shader = ResourceManager<Shader>::Load(std::string("Mesh"), "../Shaders/Mesh.Vertex", "../Shaders/Mesh.Fragment");
	}

	m_UsesBlending = material.alphaMode == "BLEND";
}

void Perry::Material::Bind()
{
	Bind(m_Shader);
}

void Perry::Material::Bind(Resource<Shader> shader)
{
	shader->Bind();
	int i = 0;
	for (auto& [textureSlotName, texture] : m_Textures)
	{
		int slot = glGetUniformLocation(shader->Get(), ("material." + std::string(textureSlotName)).c_str());
		if (slot == -1)
		{
			//printf("Unused texture !\n");
			continue;
		}
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texture->m_TextureBuffer);

		glUniform1i(slot, i); // Set the sampler to use texture unit 0
		i++;
	}

	glUniform1f(glGetUniformLocation(shader->Get(), "material.normal"), m_NormalFactor);


	glUniform4fv(glGetUniformLocation(shader->Get(), "material.albedo"), 1, &m_AlbedoFactor[0]);
	glUniform1f(glGetUniformLocation(shader->Get(), "material.metallic"), m_MettalicFactor);
	glUniform1f(glGetUniformLocation(shader->Get(), "material.roughness"), m_RoughnessFactor);
	glUniform3fv(glGetUniformLocation(shader->Get(), "material.emissive"), 1, &m_EmissiveFactor[0]);
	glUniform1f(glGetUniformLocation(shader->Get(), "material.occlusion"), m_OcclusionFactor);
}

void Perry::Material::SetSampler(unsigned sampler, int index)
{
	if (index < 0)
	{
		for (auto& [textureSlotName, texture] : m_Textures)
		{
			glBindSampler(texture->m_TextureBuffer, sampler);
		}

		return;
	}
	printf("SetSampler on index %i is not yet supported\n", index);
}



IMPLEMENT_REFLECT_OBJECT(Perry::Material)
{
	meta.data<&Material::m_AlbedoFactor>("ALBEDO"_hs)
		PROP_DISPLAYNAME("ALBEDO");
}
FINISH_REFLECT()