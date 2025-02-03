#include "pch.h"
#pragma hdrstop
using namespace Perry;
using namespace Utilities;
using namespace Perry::Graphics;

Texture::~Texture()
{
	glDeleteTextures(1, &m_TextureBuffer);
}

Texture::Texture(const std::string& p) :
	IResourceType(p), m_Format(TextureChannels::RGBA)
{
}

void Texture::Reload()
{
	LoadFromPath(GetPath());

	printf("Compress called for: %s \n", GetPath().c_str());
}



void Texture::LoadFromPath(const std::string& path)
{//
	int channels;

	auto usedpath = path;

	auto* data = stbi_load(usedpath.c_str(), &m_Width, &m_Height, &channels, STBI_default);

	assert(data != nullptr);

	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);

	auto textureName = Perry::Utilities::GetSimpleName(path);
	glObjectLabel(GL_TEXTURE, m_TextureBuffer, (GLsizei)textureName.length(), textureName.c_str());

	//Custom values not supported for now...
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	GLenum format;
	switch (channels) {
	case 1: format = GL_RED; m_Format = TextureChannels::RED; break;
	case 2: format = GL_RG; m_Format = TextureChannels::RG; break;
	case 3: format = GL_RGB; m_Format = TextureChannels::RGB; break;
	case 4: format = GL_RGBA; m_Format = TextureChannels::RGBA; break;
	default:
		fprintf(stderr, "Unsupported image format\n");
		stbi_image_free(data);
		assert(false);
		return;
	}

	if (Descriptor.useTexStorage)
	{
		throw "NOT IMPLEMENTED";
	}
	else {
		glTexImage2D(GL_TEXTURE_2D,
			0,
			format,
			m_Width, m_Height,
			0, format,
			GL_UNSIGNED_BYTE,
			data);


		if (Descriptor.m_GenerateMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);
}

static const std::unordered_map<TTextureType, GLenum > TargetLookup =
{
	{TTextureType::Texture2d,GL_TEXTURE_2D}
};
static const std::unordered_map<TextureChannels, GLint  > PixelFormatLookup =
{
	{TextureChannels::RGBA,GL_RGBA},
	{TextureChannels::RGB,GL_RGB},
	{TextureChannels::RG,GL_RG},
	{TextureChannels::RED,GL_RED},
	{TextureChannels::DEPTH_STENCIL,GL_DEPTH_STENCIL}
};


static const std::unordered_map<TextureInternalFormat, int> InternalFormatLookup{
	{TextureInternalFormat::RGBA_16f,GL_RGBA16F,},
	{TextureInternalFormat::RGBA_32f,GL_RGBA32F},
	{TextureInternalFormat::DEPTH24_STENCIL8,GL_DEPTH24_STENCIL8},
};


Texture::Texture(const std::string& Hash, const TextureConstruct& data) :
	IResourceType(Hash.substr(Hash.find(FileIO::GetPath(DirectoryType::Resource)))),
	m_Format(data.m_Format),
	m_InternalFormat(data.m_InternalFormat),
	m_Width(data.m_Width),
	m_Height(data.m_Height),
	Descriptor(data)
{
	//TODO: this one shoulnd't be allowed to set this instead should go trough Load...
}

void Texture::Load()
{
	PROFILER_MARKC("Load Texture","Graphics")

	glGenTextures(1, &m_TextureBuffer);

	if (!GetPath().empty() && std::filesystem::path(GetPath()).has_extension())
	{
			LoadFromPath(GetPath());
	}
	else
	{
		GLenum textureType = TargetLookup.find(Descriptor.m_Target)->second;
		glBindTexture(textureType, m_TextureBuffer);

		auto textureName = Perry::Utilities::GetSimpleName(GetPath());
		glObjectLabel(GL_TEXTURE, m_TextureBuffer, (GLsizei)textureName.length(), textureName.c_str());


		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, (GLenum)Descriptor.m_Filters.m_Min);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, (GLenum)Descriptor.m_Filters.m_Mag);

		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, (GLenum)Descriptor.m_WrapMode.m_Horizontal);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, (GLenum)Descriptor.m_WrapMode.m_Vertical);


		auto internalFormat = InternalFormatLookup.find(Descriptor.m_InternalFormat)->second;
		auto pixelFormat = PixelFormatLookup.find(Descriptor.m_Format)->second;
		if (Descriptor.m_Format != TextureChannels::RGBA && Descriptor.m_Format != TextureChannels::DEPTH_STENCIL)
		{
			internalFormat += 1;
		}

		if (Descriptor.useTexStorage)
		{
			glTexStorage2D(
				textureType,      // target
				1,                  // levels of mipmapping
				internalFormat,         // internal format
				Descriptor.m_Width, Descriptor.m_Height
			);
		}
		else {
			glTexImage2D(textureType,
				Descriptor.m_Level,
				internalFormat,
				Descriptor.m_Width, Descriptor.m_Height,
				Descriptor.m_Border,
				pixelFormat,
				Descriptor.m_GpuType,
				Descriptor.data);

			if (Descriptor.m_GenerateMipMaps)
				glGenerateMipmap(textureType);
		}
	}

	CompleteLoad();
}

void Texture::Unload()
{
	glDeleteTextures(1, &m_TextureBuffer);
}

int Texture::GetInternalFormat() const
{
	return InternalFormatLookup.find(m_InternalFormat)->second;
}
