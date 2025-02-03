#pragma once

namespace Perry::Graphics
{
	struct TextureConstruct
	{
		WrapModes m_WrapMode;

		struct
		{
			Distance m_Min = Distance::Nearest;
			Distance m_Mag = Distance::Nearest;
		} m_Filters{};

		TTextureType m_Target = TTextureType::Texture2d;
		int m_Level = 0;
		TextureInternalFormat m_InternalFormat = TextureInternalFormat::RGBA_32f;
		int m_Width = -1;
		int m_Height = -1;
		int m_Border = 0;
		TextureChannels m_Format = TextureChannels::RGBA;
		unsigned int m_GpuType = 0;
		const void* data = nullptr;

		bool m_GenerateMipMaps = true;

		/// <summary>
		/// This texture cannot be loaded on disk, embedded or generated on the fly...
		/// </summary>
		bool m_ReadOnly = false;

		bool useTexStorage = false;
	};
}
