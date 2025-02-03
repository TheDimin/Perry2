#pragma once

namespace Perry
{
	class ComputeShader : public IResourceType
	{
	public:
		ComputeShader(const std::string& name) :IResourceType(name) {}
		ComputeShader(const std::string& name, const std::string& computeShader);

		void Load() override;
		void Unload() override;

		//Shader(const Shader&) = delete;
		void Rebuild();
		void Bind() const;
		unsigned Get() const
		{
			assert(m_shaderId != 0);
			return m_shaderId;
		}
	private:
		bool Build();

		std::string m_ComputePath = "UNDEFINED";
		unsigned m_shaderId = 0;
	};
}
