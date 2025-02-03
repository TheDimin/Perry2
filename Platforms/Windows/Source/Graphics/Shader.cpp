#include "pch.h"
#pragma hdrstop

using namespace Perry;

Shader::Shader(const std::string& name, const std::string& vertexPath) :
	IResourceType(name), m_vertexPath(vertexPath), m_fragmentPath("")
{
}

Shader::Shader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) :
	IResourceType(name), m_vertexPath(vertexPath), m_fragmentPath(fragmentPath)
{

}

void Shader::Load()
{
	std::string vertexCode = Perry::FileIO::Read(DirectoryType::Shaders, m_vertexPath);
	std::string fragmentCode = Perry::FileIO::Read(DirectoryType::Shaders, m_fragmentPath);

	Build(vertexCode, fragmentCode);
	CompleteLoad();
}

void Shader::Unload()
{
	glDeleteProgram(m_shaderId);
	m_shaderId = 0;
}


void Shader::Rebuild()
{
	auto oldID = m_shaderId;

	std::string vertexCode = Perry::FileIO::Read(DirectoryType::Shaders, m_vertexPath);
	std::string fragmentCode = Perry::FileIO::Read(DirectoryType::Shaders, m_fragmentPath);


	if (Build(vertexCode, fragmentCode))
	{
		glDeleteProgram(oldID);

		printf("Reloaded shader sucesfull\n");
	}
}
void Shader::Bind() const
{
	glUseProgram(m_shaderId);
}
unsigned Shader::Get() const
{
	assert(m_shaderId != 0);
	return m_shaderId;
}

bool Shader::Build(const std::string& vertexCode, const std::string& fragmentCode)
{
	static int success;
	static char infoLog[512];



	const char* cVertex = vertexCode.c_str();

	unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
	auto vertexName = Perry::Utilities::GetSimpleName(m_vertexPath);
	glObjectLabel(GL_SHADER, vertexShader, static_cast<GLsizei>(vertexName.length()), vertexName.c_str());
	glShaderSource(vertexShader, 1, &cVertex, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);


	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
			infoLog << std::endl;
		return false;
	}


	unsigned fragmentShader = 0;
	if (!m_fragmentPath.empty())
	{

		const char* cFragment = fragmentCode.c_str();

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		auto fragmentName = Perry::Utilities::GetSimpleName(m_fragmentPath);
		glObjectLabel(GL_SHADER, fragmentShader, (GLsizei)fragmentName.length(), fragmentName.c_str());
		glShaderSource(fragmentShader, 1, &cFragment, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
				infoLog << std::endl;
			return false;
		}
	}


	unsigned shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	if (!m_fragmentPath.empty())
		glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Linking\n" <<
			infoLog << std::endl;


		glDeleteProgram(shaderProgram);
		return false;

	}
	auto shaderName = Perry::Utilities::GetSimpleName(m_ResourcePath);
	glObjectLabel(GL_PROGRAM, shaderProgram, (GLsizei)shaderName.length(), shaderName.c_str());
	glDeleteShader(vertexShader);
	if (!m_fragmentPath.empty())
		glDeleteShader(fragmentShader);

	m_shaderId = shaderProgram;
	return true;
}
