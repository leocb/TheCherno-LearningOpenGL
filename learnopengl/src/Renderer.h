#pragma once

#include <GL/glew.h>

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
void GLClearError();

// if a error flag is set, log it in the console window
bool GLLogCall(const char* function, const char* file, int line);
