#include "pch.h"
#include "Engine.h"
#include "GameBase.h"
#include "ECS/Entity.h"
#include "ECS/Components/TransformComponent.h"
#include "Transform.h"
#include "World/Region.h"
#include "World/World.h"
#include "ECS/System.h"
#include "ECS/Components/LightComponents.h"
#include "ECS/Systems/RenderBatcherSystem.h"
#pragma hdrstop

using namespace Perry;
using namespace Perry::Graphics;
#define CSTRINGPARAM(MESSAGE) sizeof(MESSAGE),MESSAGE

struct UniqueBufferInfo
{
	std::string name = "";
	Graphics::TextureConstruct construct;
};

std::string GetFrameBufferErrorCode(unsigned code)
{
	switch (code)
	{
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";

	case GL_FRAMEBUFFER_UNSUPPORTED:
		return "GL_FRAMEBUFFER_UNSUPPORTED";
	}

	return "unkown code: " + std::to_string(code);
}

void Renderer::Init()
{
	RenderThread = std::this_thread::get_id();
	debugRenderer.Init();

	instanceMeshBufferData.Initialize();

	glGenSamplers(2, &m_SpriteSamplers[0]);

	for (int i = 0; i < 2; ++i)
	{
		auto mode = i == 0 ? GL_REPEAT : GL_CLAMP_TO_BORDER;
		glSamplerParameteri(m_SpriteSamplers[i], GL_TEXTURE_WRAP_S, mode);
		glSamplerParameteri(m_SpriteSamplers[i], GL_TEXTURE_WRAP_T, mode);
		glSamplerParameteri(m_SpriteSamplers[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(m_SpriteSamplers[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	glGenFramebuffers(1, &m_GBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);


	//unsigned PositionBuffer, NormalBuffer, ColorSpecBuffer;

	m_GbufferCollectShader = ResourceManager<Shader>::Load("GBufferShader", "GatherGBuffer.Vertex", "GatherGBuffer.Fragment");
	m_DefferredShading = Perry::ResourceManager<Shader>::Load("GbufferShading", "ShaderGBuffer.Vertex", "ShaderGBuffer.Fragment");
	m_DefferredComputeShading = ResourceManager<ComputeShader>::Load("ComputeShading", "DeferredShading.Compute");

	constexpr  unsigned bufferCount = sizeof(m_BufferTextures) / sizeof(m_BufferTextures[0]);

	auto windowSize = Window->GetSize();

	//
	TextureConstruct textureConstruct;
	textureConstruct.m_Filters = { Distance::Nearest,Distance::Nearest };
	textureConstruct.m_WrapMode = WrapMode::CLAMP;
	textureConstruct.m_Width = windowSize.x;
	textureConstruct.m_Height = windowSize.y;
	textureConstruct.m_GpuType = GL_FLOAT;
	textureConstruct.m_GenerateMipMaps = false;
	textureConstruct.m_InternalFormat = TextureInternalFormat::RGBA_16f;
	textureConstruct.useTexStorage = true;

	UniqueBufferInfo bufferInfo[bufferCount]
	{
		{"OutputGbuffer",textureConstruct},
		{"PositionGBuffer",textureConstruct},
		{"NormalRoughMetGBuffer",textureConstruct},
			{"AlbedoGBuffer",textureConstruct},
		{"EmissiveAOGBuffer",textureConstruct},
		{"DepthStencelGBuffer",textureConstruct}
	};

	bufferInfo[1].construct.m_InternalFormat = TextureInternalFormat::RGBA_32f;

	{
		bufferInfo[5].construct.m_GpuType = GL_UNSIGNED_INT_24_8;
		bufferInfo[5].construct.m_Format = TextureChannels::DEPTH_STENCIL;
		bufferInfo[5].construct.useTexStorage = false;
		bufferInfo[5].construct.m_InternalFormat = TextureInternalFormat::DEPTH24_STENCIL8;
	}
	for (int i = 0; i < bufferCount; ++i)
	{
		const auto& uniqueBufferInfo = bufferInfo[i];

		m_BufferTextures[i] = ResourceManager<Texture>::Load(uniqueBufferInfo.name, uniqueBufferInfo.construct);


		if (i != bufferCount - 1)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_BufferTextures[i]->m_TextureBuffer, 0);
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_BufferTextures[i]->m_TextureBuffer, 0);
		}
	}


	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(sizeof(attachments) / sizeof(attachments[0]), attachments);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	ASSERT_MSG(LOG_GRAPHICS, fboStatus == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete: %s", GetFrameBufferErrorCode(fboStatus).c_str());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::Render()
{//
	PROFILER_MARKC("Render", "Graphics")

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(m_ClearColorValue.r, m_ClearColorValue.g, m_ClearColorValue.b, m_ClearColorValue.a);
	glClear(GL_COLOR_BUFFER_BIT);

	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	auto camE = Camera::GetCamera();
	if (camE.Valid() && camE.GetRegistry().all_of<CameraComponent, TransformComponent>(camE.GetID()))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer);
		glClearColor(m_ClearColorValue.r, m_ClearColorValue.g, m_ClearColorValue.b, m_ClearColorValue.a);
		glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		auto&& [cam, camTransform] = camE.FindComponent<CameraComponent, TransformComponent>();

		Transform::SetRegistery(&camE.GetRegistry());
		glm::vec3 cameraPosition = Transform::GetPosition(camTransform);


		auto& Regions = World::GetWorld().ActiveRegions();

		for (auto region : Regions)
		{
			for (auto& systemInstance : region->SystemInstances)
			{
				//systemInstance->DrawImGUI();
			}

			Transform::SetRegistery(&region->GetRegistry());

			region->GetSystem<LightCalculationSystem>()->BindLights();

			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			auto* rbs = region->GetSystem<RenderBatcherSystem>();
			std::scoped_lock rbslock = std::scoped_lock(rbs->GlobalReadMutex);

			{
				glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, CSTRINGPARAM("Meshes"));

				instanceMeshBufferData.Clear();
				for (auto& materialBatch : rbs->batcher.m_SimpleBatch)
				{
					if (!materialBatch.m_Material->IsLoaded() /*|| !materialBatch.m_Material->m_Shader->IsLoaded()*/)
						continue;

					materialBatch.m_Material->Bind(m_GbufferCollectShader);


					GLint projectionViewLocation = glGetUniformLocation(m_GbufferCollectShader->Get(), "pv");
					glm::mat4 finalTranspose = glm::translate(cam.m_PVMatrix, glm::vec3(region->Location.x, 0, region->Location.y));
					glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, &finalTranspose[0][0]);

					for (auto& primitiveBatch : materialBatch.m_primitivebatch)
					{

						primitiveBatch.m_InstanceData.Bind(3);
						primitiveBatch.m_Prim->Draw(primitiveBatch.m_InstanceData.GetSize());

					}
				}
				glPopDebugGroup();
			}
		}

		//
		auto screenSize = Window->GetSize();

		Regions[0]->GetSystem<LightCalculationSystem>()->BindLights();

		m_DefferredComputeShading->Bind();
		GLint camPosLocation = glGetUniformLocation(m_DefferredComputeShading->Get(), "viewPos");
		glUniform3fv(camPosLocation, 1, &cameraPosition[0]);



		for (int i = 0; i < 5; ++i)
		{
			auto accessType = i == 0 ? GL_WRITE_ONLY : GL_READ_ONLY;
			auto internalFormat = m_BufferTextures[i]->GetInternalFormat();

			glBindImageTexture(i, m_BufferTextures[i]->m_TextureBuffer, 0, GL_FALSE, 0, accessType, internalFormat);

#ifdef VALIDATE_BINDING
			GLint param;
			glGetTextureParameteriv(m_BufferTextures[i]->m_TextureBuffer, GL_IMAGE_FORMAT_COMPATIBILITY_TYPE, &param);

			if (param == GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE)
			{
				WARN(LOG_GRAPHICS, "Compatible by size");
			}
			else if (param == GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS)
			{
				WARN(LOG_GRAPHICS, "GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS");
			}
			else if (param == GL_NONE)
			{
				WARN(LOG_GRAPHICS, "GL_NONE");
			}
			else
			{
				ERROR(LOG_GRAPHICS, "Unkown error for format tyep...");
			}
			WARN(LOG_GRAPHICS, "ey");
#endif
		}
		//
		glDispatchCompute(screenSize.x / 32, screenSize.y / 32, 1);
		//
		// make sure writing to image has finished before read
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(0, 0, screenSize.x, screenSize.y, 0, 0, screenSize.x, screenSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		debugRenderer.Render(camE);
	}
	EndFrame();

	std::vector<GLuint> buffersToDelete;
	unsigned bufferID = 0;
	while (commandBackLog.releasableBuffers.try_pop(bufferID))
	{
		buffersToDelete.emplace_back(bufferID);
	}

	if (!buffersToDelete.empty())
		glDeleteBuffers(buffersToDelete.size(), buffersToDelete.data());

}

//
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{

	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 102) return;

	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
	//
	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	}std::cout << std::endl;
	std::cout << "---------------" << std::endl;

	std::cout << std::endl;
}