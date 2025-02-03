#pragma once

namespace Perry::Graphics
{

	enum class TextureChannels
	{
		RGBA,
		RGB,
		RG,
		RED,
		DEPTH_STENCIL
	};

	enum class TextureInternalFormat
	{
		RGBA_16f,
		RGBA_32f,
		DEPTH24_STENCIL8
	};


	static const std::unordered_map<TextureChannels, int> PixelFormatChannelLookup{
		{TextureChannels::RGBA,4},
		{TextureChannels::RGB,3},
		{TextureChannels::RG,2}		,
		{TextureChannels::RED,1}
	};


	enum class TTextureType
	{
		Texture2d
	};

	enum class Distance :int
	{
		Nearest = TINYGLTF_TEXTURE_FILTER_NEAREST,
		Linear = TINYGLTF_TEXTURE_FILTER_LINEAR,
		NEAREST_MIPMAP_NEAREST = TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR = TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR
	};

	enum class WrapMode
	{
		REPEAT = TINYGLTF_TEXTURE_WRAP_REPEAT,
		CLAMP = TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE,
		MIRROR = TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT,
		BORDER = 0x812D, //Not sure yet what it supposed to be...
	};

	struct WrapModes
	{
		WrapModes() = default;
		WrapModes(WrapMode Mode) :
			m_Horizontal(Mode),
			m_Vertical(Mode)
		{}

		WrapMode m_Horizontal = WrapMode::REPEAT;
		WrapMode m_Vertical = WrapMode::REPEAT;
	};
}
