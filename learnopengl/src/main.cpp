#include "main.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>

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
		char* message = (char*)alloca(messageLen * sizeof(char));
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
	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	unsigned int bufferId;
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Shaders!
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"}\n"
		;
	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n"
		;

	unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}