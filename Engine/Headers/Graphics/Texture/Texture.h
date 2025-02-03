#pragma once

namespace Perry
{
	namespace Graphics
	{
		struct TextureConstruct;
	}
	class Texture : public IResourceType
	{
	public:

		~Texture();
		Texture(const std::string& p);
		Texture(const std::string& Hash, const Graphics::TextureConstruct& data);

		void Load() override;
		void Unload() override;

		void Reload();

		int GetInternalFormat() const;
		Graphics::TextureChannels m_Format;
		Graphics::TextureInternalFormat m_InternalFormat;
		unsigned int m_TextureBuffer = 0;

		int m_Width = -1, m_Height = -1;

		Graphics::TextureConstruct Descriptor;

	private:

		void LoadFromPath(const std::string& path);
	};
}
