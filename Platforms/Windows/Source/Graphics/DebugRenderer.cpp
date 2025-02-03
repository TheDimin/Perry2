#include "pch.h"
#include "ECS/Entity.h"
#include "ECS/Components/CameraComponent.h"
#pragma hdrstop

void Perry::DebugRenderer::Init()
{
	// Generate and bind a Vertex Array Object (VAO)
    glGenVertexArrays(1, &LineVAO);
    glBindVertexArray(LineVAO);

	debugLineBuffer = ArrayBuffer<RenderLine>("DebugLines",GL_ARRAY_BUFFER);
	debugLineBuffer.Initialize();
	debugLineBuffer.GPUInitialize();
	
	//This should not be allowed here :)
	glEnableVertexAttribArray(0);  // Start position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderLine), (void*)sizeof(ContainerBufferBase::HeaderInfo));

	glEnableVertexAttribArray(1);  // Color
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RenderLine),(void*)(sizeof(ContainerBufferBase::HeaderInfo)+offsetof(RenderLine,Color)));

	glBindBuffer(debugLineBuffer.GetBufferType(), 0);
	//AGL_ATTRIBUTE_VERTEX
	
	lineRenderShader = ResourceManager<Shader>::Load("LineShader","Line.Vertex","Line.Fragment");
}

void Perry::DebugRenderer::AddLine(const Line& line)
{
	SCOPE_LOCK(LineBufferMutex)
	debugLineBuffer.AddElement({line.Start,line.Color});
	debugLineBuffer.AddElement({line.End,line.Color});
}

void Perry::DebugRenderer::Render(const Entity& camE)
{
	lineRenderShader->Bind();
	glBindVertexArray(LineVAO);
	
	glBindBuffer(debugLineBuffer.GetBufferType(),debugLineBuffer.GetGpuHandle());
	debugLineBuffer.UploadData();

	auto mvp = camE.FindComponent<CameraComponent>().m_PVMatrix;
	glUniformMatrix4fv(glGetUniformLocation(lineRenderShader->Get(), "mvp"), 1, GL_FALSE, &mvp[0][0]);

	glDrawArrays(GL_LINES, 0, debugLineBuffer.GetSize()*2);
}

void Perry::DebugRenderer::EndFrame()
{
	SCOPE_LOCK(LineBufferMutex)
	debugLineBuffer.Clear();
}