#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
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
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& sourceCode)
{
	unsigned int shaderId = glCreateShader(type);
	const char* rawsrc = sourceCode.c_str();
	glShaderSource(shaderId, 1, &rawsrc, nullptr);
	glCompileShader(shaderId);

	// Verify shader compilation status
	int result;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		// Failed, get message
		int messageLen;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &messageLen);
		char* message = (char*)_malloca(messageLen * sizeof(char));
		glGetShaderInfoLog(shaderId, messageLen, &messageLen, message);
		// Message output
		std::cerr << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
			" shader: " << std::endl << message << std::endl;
		// Delete failed shader and return 0 (invalid state)
		glDeleteShader(shaderId);
		return 0;
	}

	std::cout << "Compiled " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader " << shaderId << std::endl;
	return shaderId;
}

static unsigned int CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	// Compile shaders
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	// attach them to the program
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	// Delete intermediary - this is not *really* necessary and can be commented out for debugging
	glDeleteShader(vs);
	glDeleteShader(fs);
	std::cout << "Program created " << program << std::endl;

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cerr << "GLEW INIT ERROR!" << std::endl;

	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

	/* set some buffers */
	float positions[] = {
		-0.5f, -0.5f, // 0
		 0.5f, -0.5f, // 1
		 0.5f,  0.5f, // 2
		-0.5f,  0.5f  // 3
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0
	};


	unsigned int bufferId;
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}