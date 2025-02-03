#include "pch.h"
#pragma hdrstop

using namespace Perry;
using namespace Perry::Utilities;

Primitive::~Primitive()
{
}


Primitive::Primitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, Perry::Model& owningModel)
{
	m_IndicesDraw = primitive.indices > -1;
	m_RenderMode = primitive.mode;
	m_MaterialIndex = primitive.material;

	//Deprecated, here for backup...
	m_tinygltfPrimitive = &primitive;

	if (m_IndicesDraw) {
		const auto& indexAccesor = model.accessors[primitive.indices];
		m_RenderAmount = indexAccesor.count;
		m_RenderDataType = indexAccesor.componentType;
		m_DataOffset = indexAccesor.byteOffset;
	}
	else
	{
		const auto& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
		m_DataOffset = accessor.byteOffset;
		m_RenderAmount = accessor.count;
	}

}

void Primitive::Draw(unsigned Amount) const
{
	if (Amount == 0)return;
	assert(Amount != 0);
	glBindVertexArray(VAO);
	if (m_IndicesDraw)
	{
		glDrawElementsInstanced(
			m_RenderMode,
			(GLsizei)m_RenderAmount,
			m_RenderDataType,
			(void*)(m_DataOffset), Amount);
	}//
	else
	{
		//we assume only 1 buffer exist in this case
		glDrawArraysInstanced(m_RenderMode, (GLsizei)m_DataOffset, (GLsizei)m_RenderAmount, Amount);
	}
 }