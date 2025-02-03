#include "pch.h"
#pragma hdrstop

#include "Utilities.h"


using namespace Perry;
using namespace Utilities;


Window::Window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);


	m_Size.x = GetLaunchParameters().GetInt("WindowWidth", m_Size.x);
	m_Size.y = GetLaunchParameters().GetInt("WindowHeight", m_Size.y);

	m_Window = glfwCreateWindow(m_Size.x, m_Size.y, "Perry Engine", NULL, NULL);
	if (m_Window == NULL)
	{
		printf("Failed to create GLFW window \n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(m_Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD \n");
		return;
	}

	glViewport(0, 0, m_Size.x, m_Size.y);
}

Window::~Window()
{
}

bool Perry::Window::IsMinimized() const
{
	return false;
}

void Window::SetTitle(const std::string& newTitle)
{
	glfwSetWindowTitle(m_Window, newTitle.c_str());
}

void Window::SetWindowSize(const glm::ivec2& size)
{
}

void Window::OnResize()
{

}
