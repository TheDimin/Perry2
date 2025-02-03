#pragma once

namespace Perry
{
	class Debug
	{
	public:
		/// <summary>
		/// Removes a line to the list of lines that need to be drawn
		/// </summary>
		/// <param name="start"></param>
		/// <param name="end"></param>
		/// <param name="color"></param>
		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1));

		/// <summary>
		/// Draws a square.
		/// </summary>
		/// <param name="center"></param>
		/// <param name="size"></param>
		/// <param name="color"></param>
		static void DrawSquare(const glm::vec3& center, float size, const glm::vec3& color = glm::vec3(1));

		static void DrawBox(const glm::vec3& center, glm::vec3 HalfExtend, const glm::vec3& color = glm::vec3(1));
	};

}