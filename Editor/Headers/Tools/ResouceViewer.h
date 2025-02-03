#pragma once

namespace Perry
{
	class Texture;
}

struct TextureInfo
{
	TextureInfo(const std::string& sourcePath);
	struct AssetInfo
	{
		std::string filePath = "";
		std::string fileName = "";

		double fileSize = 0;
		//Resolution would be nice but we can't without loading the file...
		AssetInfo();
	};

	AssetInfo assetInfo[2]{};

	bool compressed;//NOt correct term, but we treat textures compressed if file with COM_<imagename>.<extension> exists
	Perry::Resource<Perry::Texture> texture;
	~TextureInfo();
	TextureInfo();
};

static const inline std::string SourcePrefix = std::string("SOURCE_");
std::string ToSourcePath(const std::string& assetPath);

class MaterialViewer
{
public:
	MaterialViewer();
	void Draw();
};

class RenderBatchViewer
{
public:
	void Draw();
};

class ShaderViewer
{
public:
	void Draw();
};

class TextureViewer
{
public:
	TextureViewer();
	void Draw();
private:
	void CreateTextureInfo(const std::vector<std::string>& paths, bool forceLoad = true);
	void UpdateTextureInfo(TextureInfo& info);
	void SortOnFileSize();
	void SortOnLoadTime();
	void ResizeImage(TextureInfo& info, int newWidth, int newHeight, int channels);
	void CopyTexture(const std::string& source, const std::string& target, TextureInfo& info);

	void DrawTextures();
	void DrawTextureInternal(TextureInfo& textureInfo);

	std::vector< TextureInfo> m_textureInfo;
	std::set<std::string> m_loadedTexturePahts;//loaded into resourceViewer.. Not memory!

	void DrawTextureModal();

	struct ModalData
	{
		ModalData() = default;
		ModalData(const TextureInfo&);
		int width, height, channels;
		int sourceWidth, sourceHeight, sourceChannels;

		const TextureInfo* textureInfo = nullptr;
	} activeModalInfo;

	bool onlyShowLoaded = false;
	TextureInfo* activeTextureInfoModal = nullptr;

};

class ResourceViewer
{
public:
	ResourceViewer();
	void Draw();

private:

	MaterialViewer mv{};
	TextureViewer tv{};
	RenderBatchViewer rbv{};
	ShaderViewer sv{};
};