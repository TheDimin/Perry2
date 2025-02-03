#include "pch.h"
// Graphics Components
#include "Graphics/Renderer.h"

#include "Utilities.h"
#include "Window.h"


// enable optimus!
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

using namespace Perry;
using namespace Utilities;

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
	GLsizei length, const char* message, const void* userParam);

void Renderer::PlatformInit()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(Window->GetHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
	glfwSwapInterval(0);
	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
#ifndef _SHIPPING
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}////
#endif

	glEnable(GL_DEPTH_TEST);

	const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
	const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

	INFO(LOG_GRAPHICS, "Renderer initialized with %s : %s \n", (char*)vendor, (char*)renderer);

	Init();
}

Renderer::~Renderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

void Renderer::StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Renderer::EndFrame()
{
	debugRenderer.EndFrame();
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, sizeof("Dear ImGUI"), "Dear ImGUI");
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glPopDebugGroup();
	glfwSwapBuffers(Window->GetHandle());
	glFlush();
}

bool Renderer::Update(float deltaTime)
{
	glfwPollEvents();

	return glfwWindowShouldClose(Window->GetHandle());
}