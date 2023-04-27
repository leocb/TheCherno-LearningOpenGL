#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();
#ifdef _DEBUG
#define GLCall(x) \
	GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

// read all error flags from OpenGL (clear them)
static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

// if a error flag is set, log it in the console window
static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << std::hex << error << std::dec << "): " << function << " @ " << file << ":" << line << std::endl;
		return false;
	}

	return true;
}

// struct used to store the shader program sources
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

// parse the shader file and extract the vertex and fragment shaders
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
			ss[(int)type] << line << '\n'; //add the line to the source of this shader type
		}
	}

	// return the program shader source struct
	return { ss[0].str(), ss[1].str() };
}

// compile the shader program for the GPU
static unsigned int CompileShader(unsigned int type, const std::string& sourceCode)
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
	std::cout << "Compiled " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader " << shaderId << std::endl;
	return shaderId;
}

static unsigned int CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
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

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// enable vsync
	glfwSwapInterval(1);

	// check if glew initialized correctly
	if (glewInit() != GLEW_OK)
		std::cerr << "GLEW INIT ERROR!" << std::endl;

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl << std::endl;

	/* set buffers */
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

	// VAO - vertex attribute object
	unsigned int vao; 
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	// Vertex buffer
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

	// link vertex buffer to vao (index 0) - Define how the data is organized inside the buffer
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

	// index buffer object (also linked to the VAO) - Define in what order to draw the vertices
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	// Shaders
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	// Shader uniform
	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.2, 0.4f, 0.8f, 1.0f)); // define a "default" value (necessary?)


	// Clear bindings we used to build the buffers
	GLCall(glBindVertexArray(0));
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


	// some vars to control the red channel down bellow
	float r = 0.0f;
	float increment = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		// Clear everything
		glClear(GL_COLOR_BUFFER_BIT);

		// Shader
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, r, 0.4f, 0.8f, 1.0f));

		// VAO
		GLCall(glBindVertexArray(vao));

		// Draw call
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // 6 is the number of elements (vertex index),

		// change red channel color
		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	GLCall(glDeleteProgram(shader));

	glfwTerminate();
	return 0;
}