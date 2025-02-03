#pragma once

namespace Perry
{
	class Shader : public IResourceType
	{
	public:
		Shader(const std::string& name) :IResourceType(name) {}
		Shader(const std::string& name, const std::string& vertexPath);
		Shader(const std::string& name, const std::string& vertexPath
			, const std::string& fragmentPath);

		void Load() override;
		void Unload() override;

		//Shader(const Shader&) = delete;
		void Rebuild();
		void Bind() const;
		unsigned Get() const;
	private:
		bool Build(const std::string& vertexCode, const std::string& fragmentCode);

		std::string m_vertexPath = "UNDEFINED";
		std::string m_fragmentPath = "UNDEFINED";
		unsigned m_shaderId = 0;
	};
}
