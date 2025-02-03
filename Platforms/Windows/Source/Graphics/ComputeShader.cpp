#include "pch.h"
#pragma hdrstop

using namespace Perry;

ComputeShader::ComputeShader(const std::string& name, const std::string& computeShader) : IResourceType(name), m_ComputePath(computeShader)
{
}

void ComputeShader::Load()
{
	Build();
	CompleteLoad();
}

void ComputeShader::Unload()
{
	glDeleteProgram(m_shaderId);
	m_shaderId = 0;
}


void ComputeShader::Rebuild()
{
	auto oldID = m_shaderId;
	if (Build())
	{
		glDeleteProgram(oldID);

		INFO(LOG_GRAPHICS, "Reloaded shader sucesfull");
	}
}

void ComputeShader::Bind() const
{
	assert(m_shaderId != 0);
	glUseProgram(m_shaderId);
}

bool ComputeShader::Build()
{
	static int success;
	static char infoLog[512];

	std::string computeCode;

	try
	{
		computeCode = Perry::FileIO::Read(DirectoryType::Shaders, m_ComputePath);
	}
	catch (std::ifstream::failure e)
	{
		ERROR(LOG_GRAPHICS, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
		return false;
	}

	const char* cCompute = computeCode.c_str();

	unsigned int compute;
	// compute shader
	compute = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute, 1, &cCompute, NULL);
	glCompileShader(compute);

	glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(compute, 512, NULL, infoLog);
		ERROR(LOG_GRAPHICS, "%s SHADER::COMPUTE::COMPILATION_FAILED\n  %s", m_ComputePath.data(), infoLog);
		return false;
	}

	// shader Program
	unsigned shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, compute);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		ERROR(LOG_GRAPHICS, "%s SHADER::PROGRAM::LINKING_FAILED\n  %s",m_ComputePath.data(), infoLog);

		glDeleteProgram(shaderProgram);
		return false;

	}
	glDeleteShader(compute);
	auto shaderName = Perry::Utilities::GetSimpleName(m_ResourcePath);
	glObjectLabel(GL_PROGRAM, shaderProgram, (GLsizei)shaderName.length(), shaderName.c_str());

	m_shaderId = shaderProgram;
	return true;
}