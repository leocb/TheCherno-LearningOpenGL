#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// enable vsync
	glfwSwapInterval(1);

	// enable alpha blend
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// check if glew initialized correctly
	if (glewInit() != GLEW_OK)
		std::cerr << "GLEW INIT ERROR!" << std::endl;

	std::cout << "OpenGL Version: " 
		<< glGetString(GL_VERSION) << std::endl << std::endl;

	{
		/* set buffers */
		// This buffer is (pos.x, pos.y, tex.u, tex.v)
		float positions[] = {
			100.0f, 100.0f, 0.0f, 0.0f, // 0
			200.0f, 100.0f, 1.0f, 0.0f, // 1
			200.0f, 200.0f, 1.0f, 1.0f, // 2
			100.0f, 200.0f, 0.0f, 1.0f  // 3
		};

		unsigned int indices[] = {
			0,1,2,
			2,3,0
		};

		// Vertex array object
		VertexArray vao;

		// Vertex buffer object
		VertexBuffer vbo(positions, 4 * 2 * 2 * sizeof(float));

		// link vertex buffer to VAO - Define how the data is organized inside the buffer
		VertexBufferLayout layout;
		// each push also affects the index of the layout inside the vbo (used in the vertex shader)
		// in this case, positions will be at index 0
		// if we add another push, say, for textures, it will be at index 1
		layout.Push<float>(2); // positions
		layout.Push<float>(2); // texture coordinates
		vao.AddBuffer(vbo, layout);

		// link index buffer object (also linked to the VAO) - Define in what order to draw the vertices
		IndexBuffer ibo(indices, 2 * 3);

		// the projection matrix
		glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
		// the "camera" / view matrix
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
		// the model matrix
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
		// the mvp
		glm::mat4 mvp = proj * view * model;

		// Shaders
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.0f, 0.4f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", mvp);

		// Texture
		Texture texture("res/textures/Bart.png");
		texture.Bind(0);
		shader.SetUniform1i("u_Texture", 0);

		// Clear bindings we used to build the buffers
		vao.Unbind();
		vbo.Unbind();
		ibo.Unbind();
		shader.Unbind();

		// Set the renderer
		Renderer renderer;

		// some vars to control the red channel down bellow
		float r = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();
			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.4f, 0.8f, 1.0f);
			renderer.Draw(vao, ibo, shader);

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
	}

	glfwTerminate();
	return 0;
}