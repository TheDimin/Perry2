#pragma once

namespace Perry
{
	struct Entity;
	// TODO nice ifdef around debug :)
	class DebugRenderer
	{
	public:
		void Init();
		void AddLine(const Line& line);

		void Render(const Entity& camE);

		void EndFrame();

		Resource<Shader> lineRenderShader;
		
		ArrayBuffer<RenderLine> debugLineBuffer ;

		unsigned LineVAO=0;
		std::mutex LineBufferMutex;
	};
}