#include "Shader.h"
#include "Renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filepath)
	: m_filepath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, const glm::vec2& value)
{
	GLCall(glUniform2f(GetUniformLocation(name), value.x, value.y));
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
{
	GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4& value)
{
	GLCall(glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w));
}

void Shader::SetUniformMat3(const std::string& name, const glm::mat3& matrix)
{
	GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) {
		std::cerr << "Warning, uniform not found: " << name << std::endl;
	}

	m_UniformLocationCache[name] = location;

	return location;
}

// parse the shader file and extract the vertex and fragment shaders
ShaderProgramSource Shader::ParseShader(const std::string& filePath)
{
	// enum (used to select the correct string stream index
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	// File reading 
	std::ifstream stream(filePath);
	std::string line;

	// string source separation
	ShaderType type = ShaderType::NONE;
	std::stringstream ss[2];

	// read file and separate each line by type
	while (getline(stream, line)) {
		if (line.find("shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;

			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			if (type == ShaderType::NONE) continue;

			//add the line to the source of this shader type
			ss[(int)type] << line << '\n';
		}
	}

	// return the program shader source struct
	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& sourceCode)
{
	// create a new shader program
	GLCall(unsigned int shaderId = glCreateShader(type));

	// set the source code and compile
	const char* rawsrc = sourceCode.c_str();
	GLCall(glShaderSource(shaderId, 1, &rawsrc, nullptr));
	GLCall(glCompileShader(shaderId));

	// Verify shader compilation status
	int result;
	GLCall(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		// Failed, get message
		// first get the message length
		int messageLen;
		GLCall(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &messageLen));
		// allocate memory in the stack (NOT in the heap) and get the message
		char* message = (char*)_malloca(messageLen * sizeof(char));
		GLCall(glGetShaderInfoLog(shaderId, messageLen, &messageLen, message));

		// Message output in the console window
		std::cerr << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
			" shader: " << std::endl << message << std::endl;

		// Delete failed shader and return 0 (invalid state)
		GLCall(glDeleteShader(shaderId));
		return 0;
	}

	// all done.
	std::cout << "Compiled "
		<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
		<< " shader " << shaderId << std::endl;

	return shaderId;
}

unsigned int Shader::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	// Compile the shaders
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	// link them to the program
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Delete intermediary - this is not *really* necessary and can be commented out for GPU debugging
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
	std::cout << "Program created " << program << std::endl;

	return program;
}
