#include "TestTexture2D.h"

#include "imgui/imgui.h"


namespace test {

	TestTexture2D::TestTexture2D()
		: m_TranslationA{ 50.0f, 50.0f, 0.0f }
		, m_TranslationB{ 00.0f, 00.0f, 0.0f }
		, m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
		, m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))

	{
		//GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

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
		m_VAO = std::make_unique<VertexArray>();
		// Vertex buffer object
		m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 2 * 2 * sizeof(float));

		// link vertex buffer to VAO - Define how the data is organized inside the buffer
		VertexBufferLayout layout;
		// each push also affects the index of the layout inside the vbo (used in the vertex shader)
		// in this case, positions will be at index 0
		// if we add another push, say, for textures, it will be at index 1
		layout.Push<float>(2); // positions
		layout.Push<float>(2); // texture coordinates
		m_VAO->AddBuffer(*m_VBO, layout);

		// link index buffer object (also linked to the VAO) - Define in what order to draw the vertices
		m_IBO = std::make_unique<IndexBuffer>(indices, 2 * 3);

		// Shaders
		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");

		// Texture
		m_Texture = std::make_unique<Texture>("res/textures/Bart.png");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);
		
	}

	TestTexture2D::~TestTexture2D()
	{
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture2D::OnRender()
	{
		Renderer renderer;
		m_Texture->Bind(0);

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4("u_MVP", mvp);
			renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		}
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4("u_MVP", mvp);
			renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		}
	}

	void TestTexture2D::OnImGuiRender()
	{
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::Text("fps %.1f (%.3fms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	}
}
