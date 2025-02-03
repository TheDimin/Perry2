#pragma once

namespace Perry
{
	class Window
	{
	public:
		Window();
		~Window();

		int GetWindowWidth() const;
		int GetWindowHeight() const;
		bool IsMinimized() const;

		void SetTitle(const std::string& newTitle);

		void SetWindowSize(const glm::ivec2& size);
		const glm::ivec2& GetSize()const;

		WindowHandle GetHandle() const;
	private:
		friend class Renderer;

		void OnResize();

		glm::ivec2 m_Size = { 1920  ,1080  };

		WindowHandle m_Window ;
	};
}
