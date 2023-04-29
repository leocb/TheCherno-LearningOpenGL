#pragma once

#include <GL/glew.h>

#ifdef _DEBUG

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) \
	GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#else

#define ASSERT(x)
#define GLCall(x) x

#endif

// read all error flags from OpenGL (clear them)
void GLClearError();

// if a error flag is set, log it in the console window
bool GLLogCall(const char* function, const char* file, int line);
