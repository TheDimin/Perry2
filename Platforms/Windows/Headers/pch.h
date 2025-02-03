#pragma once

#include "Core.h"

#pragma push_macro("ERROR")
#include <windows.h>
#pragma pop_macro("ERROR")

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <fstream>

#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <glfw3native.h>

#include <steam_api.h>


#include "Log.h"

#include "JobSystem/PFiber.h"

#include "HotReloadSystem.h"


#include "ECS/Components/CameraComponent.h"

#include "Graphics/Buffer.h"
#include "Graphics/Texture/TextureDefinitions.h"
#include "Graphics/Texture/TextureCreator.h"
#include "Graphics/Material.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Graphics/ComputeShader.h"
#include "Graphics/Texture/Texture.h"
#include "Graphics/Texture/TextureCreator.h"
#include "Graphics/Primitive.h"
#include "Graphics/RenderInfo.h"
#include "Graphics/DebugRenderer.h"
#include "Graphics/Renderer.h"