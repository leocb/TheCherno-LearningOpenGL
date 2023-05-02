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

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
			000.0f, 000.0f, 0.0f, 0.0f, // 0
			100.0f, 000.0f, 1.0f, 0.0f, // 1
			100.0f, 100.0f, 1.0f, 1.0f, // 2
			000.0f, 100.0f, 0.0f, 1.0f  // 3
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
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
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


		// imgui
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();


		// some vars to control the red channel down bellow
		float r = 0.0f;
		float increment = 0.05f;

		glm::vec3 translationA(50.0f, 50.0f, 0.0f);
		glm::vec3 translationB(0.0f, 0.0f, 0.0f);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			// imgui
			glfwPollEvents();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// window
			static float f = 0.0f;
			{
				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
				ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::Text("fps %.1f (%.3fms)", io.Framerate, 1000.0f / io.Framerate);
				ImGui::End();
			}

			renderer.Clear();

			// move the model
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(vao, ibo, shader);
			}
			{
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				glm::mat4 mvp = proj * view * model;
				shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp);
				renderer.Draw(vao, ibo, shader);
			}


			// change red channel color
			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;

			r += increment;

			// imgui
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}


	// imgui Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}