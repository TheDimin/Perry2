#include "EditorPCH.h"
#pragma hdrstop

using namespace Perry;

TextureInfo::TextureInfo(const std::string& sourcePath)
{
	for (unsigned i = 0; i < 2; i++)
	{
		auto& typeInfo = assetInfo[i];

		std::filesystem::path assetPath;

		assetPath = i == 0 ? sourcePath : ToSourcePath(sourcePath);

		typeInfo.fileName = assetPath.filename().string();
		typeInfo.filePath = Perry::FileIO::GetPath(Perry::DirectoryType::Resource, assetPath.string());


		if (Perry::FileIO::Exist(Perry::DirectoryType::Resource, typeInfo.filePath))
			typeInfo.fileSize = static_cast<double>(file_size(std::filesystem::path(typeInfo.filePath))) / (1024 * 1024);
	}

	compressed = Perry::FileIO::Exist(Perry::DirectoryType::Resource, assetInfo[1].filePath);

	texture = Perry::ResourceManager<Perry::Texture>::Get(assetInfo[compressed].filePath);

}

TextureInfo::AssetInfo::AssetInfo()
{}

TextureInfo::TextureInfo() : assetInfo{}, compressed(false)
{
}

std::string ToSourcePath(const std::string& assetPath)
{
	const auto& path = std::filesystem::path(assetPath);
	return path.parent_path().append(SourcePrefix + path.filename().string()).string();
}

MaterialViewer::MaterialViewer()
{
}

void DrawMaterial(Resource<Perry::Material> mat)
{
	if (mat)
	{
		ImGui::ColorEdit4("AlbedoFactor", &mat->m_AlbedoFactor[0]);
		ImGui::DragFloat3("EmissiveFactor", &mat->m_EmissiveFactor[0]);
		ImGui::DragFloat3("SpecularFactor", &mat->m_SpecularFactor[0]);

		ImGui::DragFloat("NormalFactor", &mat->m_NormalFactor);
		ImGui::DragFloat("OcclusionFactor", &mat->m_OcclusionFactor);
		ImGui::DragFloat("MettalicFactor", &mat->m_MettalicFactor);
		ImGui::DragFloat("RoughnessFactor", &mat->m_RoughnessFactor);

		auto blend = mat->IsBlending();
		if (ImGui::Checkbox("Use Blending", &blend))
		{
			mat->UseBlending(blend);
		}

		int texturesize = (int)(ImGui::GetWindowWidth() / mat->m_Textures.size() - 25.f);

		if (ImGui::TreeNode(("Textures ##" + mat->GetPath()).c_str()))
		{
			int materialIndex = 0;
			for (const auto& materialTexture : mat->m_Textures)
			{
				ImGui::BeginGroup();
				ImGui::Image((ImTextureID)materialTexture.second->m_TextureBuffer, ImVec2((float)texturesize, (float)texturesize));
				ImGui::Text(materialTexture.first.c_str());
				ImGui::EndGroup();

				if (materialIndex < mat->m_Textures.size())
					ImGui::SameLine();
			}
			ImGui::TreePop();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + texturesize + 15);
		}


		if (ImGui::Button(("RebuildShader ##" + mat->GetPath()).c_str()))
		{
			mat->m_Shader->Rebuild();
		}
	}
}

void MaterialViewer::Draw()
{
	for (const auto& materialSourcePath : Perry::ResourceManager<Perry::Material>::GetAllPaths())
	{

		const auto value = Perry::ResourceManager<Perry::Material>::Get(materialSourcePath);
		if (value.IsLoaded())
			if (ImGui::TreeNode(materialSourcePath.c_str()))
			{
				DrawMaterial(value);
				ImGui::TreePop();
			}
	}
}

void RenderBatchViewer::Draw()
{
	/*
	auto rbs = Perry::GetEngine().GetLevel().GetSystem<Perry::RenderBatcherSystem>();

	int i = 0;
	for (auto& batch : rbs->batcher.m_SimpleBatch)
	{
		if (ImGui::TreeNode(("BATCH " + batch.m_Material->GetPath().substr(batch.m_Material->GetPath().find_last_of('/'))).c_str()))
		{
			for (auto& primBatch : batch.m_primitivebatch)
			{
				ImGui::Text("PrimBatchSize: %i", primBatch.batchCount);
			}
			DrawMaterial(batch.m_Material);
			ImGui::TreePop();
		}

		i++;
	}
	*/
}

void ShaderViewer::Draw()
{

	if (ImGui::Button("Reload All"))
	{
		for (auto& shader : Perry::ResourceManager<Shader>::GetAll())
		{
			shader->Rebuild();
		}

		for (auto& shader : Perry::ResourceManager<ComputeShader>::GetAll())
		{
			shader->Rebuild();
		}
	}
}

TextureViewer::TextureViewer()
{
	CreateTextureInfo(Perry::FileIO::GetFilesInDirectory(DirectoryType::Resource, "", ".png"), false);
	SortOnFileSize();
}

void TextureViewer::Draw()
{
	DrawTextureModal();
	ImGui::Checkbox("Only show loaded", &onlyShowLoaded);


	ImGui::NewLine();
	ImGui::NewLine();

	DrawTextures();
}

TextureInfo::~TextureInfo()
{
}

ResourceViewer::ResourceViewer()
{

}

void ResourceViewer::Draw()
{

	ImGui::Begin("ResourceViewer");


	ImGui::BeginTabBar("ResourceTabBar");
	if (ImGui::BeginTabItem("Textures"))
	{
		tv.Draw();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Materials"))
	{
		mv.Draw();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("RenderBatches"))
	{
		rbv.Draw();
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("ShaderViewer"))
	{
		sv.Draw();
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();


	ImGui::End();
}

void TextureViewer::CreateTextureInfo(const std::vector<std::string>& paths, bool forceLoad)
{
	//m_textureInfo.reserve(m_textureInfo.size() + paths.size());

	//for (const auto& path : paths)
	//{
	//	if (path.find(SourcePrefix) != std::string::npos) continue;

	//	TextureInfo info = {};
	//	
	//	if (!info.compressed)


	//		//std::shared
	//		m_textureInfo.emplace_back(info);
	//}
}

void TextureViewer::UpdateTextureInfo(TextureInfo& info)
{
	info.compressed = Perry::FileIO::Exist(DirectoryType::Resource, info.assetInfo[1].filePath);

	int index = !info.compressed;
	auto usedPath = info.assetInfo[index].filePath;

	info.assetInfo[index].fileSize = static_cast<double>(file_size(std::filesystem::path(Perry::FileIO::GetPath(DirectoryType::Resource, usedPath)))) / (1024 * 1024);

	if (!info.texture.IsLoaded())
	{
		printf("TextureRef is not valid !\n");
		//Easy don't have todo anything problem for later..
	}
	else
	{
		//Path the texture... tricky....
	//	auto start_time = std::chrono::high_resolution_clock::now();

		auto texture = info.texture;

		texture->Reload();


		//auto end_time_ = std::chrono::high_resolution_clock::now();
		//info.loadTime = std::chrono::duration<float>(end_time_ - start_time).count();
	}
}

void TextureViewer::SortOnFileSize()
{
	//std::sort(m_textureInfo.begin(), m_textureInfo.end(),
	//	[](const TextureInfo& a, const TextureInfo& b) -> bool
	//	{
	//		return a.assetInfo[a.compressed].fileSize > b.assetInfo[b.compressed].fileSize;
	//	});
}

void TextureViewer::ResizeImage(TextureInfo& info, int newWidth, int newHeight, int newchannels)
{
	int iX, iY, channels;
	TextureInfo::AssetInfo& sourceAsset = info.assetInfo[info.compressed];

	auto* oData = stbi_load((sourceAsset.filePath).c_str(), &iX, &iY, &channels, STBI_default);

	if (oData == nullptr)
	{
		ERROR(LOG_GENERIC, "ERROR FAILED TO LOAD OLD IMAGE: '%s'", info.assetInfo[0].filePath.c_str());
		return;
	}

	//Before we rewrite texture upload source incase it doesn't exist
	if (!info.compressed)
	{
		//SetFileAttributes(info.assetInfo[0].filePath.c_str(),
		//	GetFileAttributes(info.assetInfo[0].filePath.c_str()) & ~FILE_ATTRIBUTE_READONLY);

		stbi_write_png(info.assetInfo[1].filePath.c_str(), iX, iY, channels, oData, iX * channels);
	}
	// Allocate a new buffer for the resized image
	unsigned char* resized_image = (unsigned char*)malloc(newWidth * newHeight * newchannels);

	stbir_resize_uint8_linear(oData, iX, iY, iX * channels, resized_image, newWidth, newHeight, newWidth * newchannels, (stbir_pixel_layout)newchannels);

	stbi_write_png(info.assetInfo[0].filePath.c_str(), newWidth, newHeight, newchannels, resized_image, newWidth * newchannels);
	free(resized_image);
	stbi_image_free(oData);

	UpdateTextureInfo(info);
}

void TextureViewer::CopyTexture(const std::string& source, const std::string& target, TextureInfo& info)
{
	int iX, iY, channels;

	auto* oData = stbi_load(source.c_str(), &iX, &iY, &channels, STBI_default);

	if (oData == nullptr)
	{
		ERROR(LOG_GENERIC, "FAILED TO LOAD OLD IMAGE: '%s'", source.c_str());
		return;
	}

	stbi_write_png(target.c_str(), iX, iY, channels, oData, iX * channels);

	UpdateTextureInfo(info);
}


static float rescanTimer = 0;
static int rowCount = 8;
void TextureViewer::DrawTextures()
{
	rescanTimer += Perry::GetEngine().GetDeltaTime();

	if (rescanTimer > 2)
	{
		rescanTimer = 0;

		const auto paths = Perry::ResourceManager<Perry::Texture>::GetAllPaths();

		for (const auto& resourcePath : paths)
		{
			auto textureLoaded = m_loadedTexturePahts.find(resourcePath);
			if (textureLoaded == m_loadedTexturePahts.end())
			{
				if (!ResourceManager<Texture>::IsLoaded(resourcePath))continue;

				m_loadedTexturePahts.insert(resourcePath);
				m_textureInfo.emplace_back(resourcePath);
			}
		}
		SortOnFileSize();
	}


	ImGui::Columns(rowCount);
	for (auto& textureInfo : m_textureInfo)
	{

		DrawTextureInternal(textureInfo);
	}
	ImGui::Columns(1);
}

void TextureViewer::DrawTextureInternal(TextureInfo& textureInfo)
{
	unsigned useCount = 0;

	auto texture = textureInfo.texture;


	texture = Perry::ResourceManager<Perry::Texture>::Get(textureInfo.assetInfo[0].filePath);
	textureInfo.texture = texture;


	bool loaded = texture.IsLoaded();


	if (onlyShowLoaded)
		if (!loaded) return;

	ImGui::Text("AssetStatus: Loaded	:'%d' count: '%i'", loaded, static_cast<int>(useCount));

	ImGui::Text("%s", textureInfo.assetInfo[0].fileName.c_str());

	if (loaded)
	{
		ImGui::Text("SourceSize x: %i, y: %i", texture->m_Width, texture->m_Height);
		if (textureInfo.compressed)
			ImGui::Text("Texture is Compressed !");
		ImGui::Image((ImTextureID)texture->m_TextureBuffer, ImVec2(ImGui::GetColumnWidth(), ImGui::GetColumnWidth()));
		if (ImGui::IsItemHovered()) {

			ImGui::BeginTooltip();
			ImGui::Image((ImTextureID)texture->m_TextureBuffer, ImVec2(texture->m_Width / 2.f, texture->m_Height / 2.f));
			ImGui::EndTooltip();
		}

		if (ImGui::Button(("Compress Texture ##" + textureInfo.assetInfo[0].filePath).c_str()))
		{
			ImGui::OpenPopup("TextureEditModal");
			activeTextureInfoModal = &textureInfo;
			activeModalInfo = textureInfo;
		}

	}
	else
	{
		ImGui::Text("Not loaded !");

		//ImGui::PushID((element.path + "Element").c_str());
		//if (ImGui::Button("PRESS ME TO LOAD"))
		{
			//	element.texture = Perry::ResourceManager<Perry::Texture>::Get(element.path);
		};
		//ImGui::PopID();
	}

	ImGui::NextColumn();
}

void TextureViewer::DrawTextureModal()
{
	if (ImGui::BeginPopupModal("TextureEditModal"))
	{
		ImGui::DragInt("Width:", &activeModalInfo.width, 1, 0, activeModalInfo.sourceWidth);
		ImGui::DragInt("Height:", &activeModalInfo.height, 1, 0, activeModalInfo.sourceHeight);
		ImGui::DragInt("Channels:", &activeModalInfo.channels, 1, 0, activeModalInfo.sourceChannels);
		if (ImGui::Button("Resize"))
		{
			ResizeImage(*activeTextureInfoModal, activeModalInfo.width, activeModalInfo.height, activeModalInfo.channels);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
#undef DeleteFile
			CopyTexture(activeTextureInfoModal->assetInfo[1].filePath, activeTextureInfoModal->assetInfo[0].filePath, *activeTextureInfoModal);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

TextureViewer::ModalData::ModalData(const TextureInfo& tinfo)
{
	auto texture = tinfo.texture;

	width = texture->m_Width;
	height = texture->m_Height;

	channels = Perry::Graphics::PixelFormatChannelLookup.find(texture->m_Format)->second;

	sourceWidth = width;
	sourceHeight = height;
	sourceChannels = channels;
	textureInfo = &tinfo;
}