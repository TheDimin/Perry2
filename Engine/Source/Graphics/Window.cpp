#include "EnginePCH.h"
#include "Window.h"

using namespace Perry;

int Window::GetWindowWidth() const
{
	return m_Size.x;
}

int Window::GetWindowHeight() const
{
	return m_Size.y;
}

const glm::ivec2& Window::GetSize() const
{
	return m_Size;
}


WindowHandle Window::GetHandle() const
{
	return m_Window;
}