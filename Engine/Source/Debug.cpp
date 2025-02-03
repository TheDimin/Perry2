#include "EnginePCH.h"
#pragma hdrstop

#include "Debug.h"

#include "Engine.h"
using namespace Perry;


void Debug::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
{
	GetEngine().GetRenderer().debugRenderer.AddLine({ start,end ,color });
}

void Debug::DrawSquare(const glm::vec3& center, float size, const glm::vec3& color)
{
	// TODO square can be optimized
	float halfSize = size * 0.5f;

	// Define the four corners of the square in the XY plane (assuming Z is constant)
	glm::vec3 topLeft = center + glm::vec3(-halfSize, 0.0f, halfSize);
	glm::vec3 topRight = center + glm::vec3(halfSize, 0.0f, halfSize);
	glm::vec3 bottomRight = center + glm::vec3(halfSize, 0.0f, -halfSize);
	glm::vec3 bottomLeft = center + glm::vec3(-halfSize, 0.0f, -halfSize);

	// Draw the four edges of the square by connecting the corners
	DrawLine(topLeft, topRight, color); // Top edge
	DrawLine(topRight, bottomRight, color); // Right edge
	DrawLine(bottomRight, bottomLeft, color); // Bottom edge
	DrawLine(bottomLeft, topLeft, color); // Left edge
}

void Debug::DrawBox(const glm::vec3& center, glm::vec3 HalfExtend, const glm::vec3& color)
{
	// Define the four corners of the square in the XY plane (assuming Z is constant)
	glm::vec3 p1 = center + glm::vec3(HalfExtend.x, HalfExtend.y, HalfExtend.z);
	glm::vec3 p2 = center + glm::vec3(-HalfExtend.x, HalfExtend.y, HalfExtend.z);
	glm::vec3 p3 = center + glm::vec3(-HalfExtend.x, -HalfExtend.y, HalfExtend.z);
	glm::vec3 p4 = center + glm::vec3(-HalfExtend.x, -HalfExtend.y, -HalfExtend.z);
	glm::vec3 p5 = center + glm::vec3(HalfExtend.x, -HalfExtend.y, -HalfExtend.z);
	glm::vec3 p6 = center + glm::vec3(HalfExtend.x, HalfExtend.y, -HalfExtend.z);


	glm::vec3 p7 = center + glm::vec3(-HalfExtend.x, HalfExtend.y, -HalfExtend.z);
	glm::vec3 p8 = center + glm::vec3(HalfExtend.x, -HalfExtend.y, HalfExtend.z);

	// Draw the four edges of the square by connecting the corners
	DrawLine(p1, p2, color);
	DrawLine(p2, p3, color);
	DrawLine(p3, p4, color);
	DrawLine(p4, p5, color);
	DrawLine(p5, p6, color);
	DrawLine(p6, p1, color);


	DrawLine(p8, p1, color);
	DrawLine(p8, p3, color);
	DrawLine(p8, p5, color);

	DrawLine(p4, p7, color);
	DrawLine(p6, p7, color);
	DrawLine(p7, p2, color);
}