#pragma once

#include "Test.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "../Assert.h"
#include "../VertexArray.h"
#include "../VertexBuffer.h"
#include "../Texture.h"
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../Renderer.h"

namespace test {
	class TestTexture2D : public Test
	{
	private:
		glm::vec3 m_TranslationA;
		glm::vec3 m_TranslationB;
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<IndexBuffer> m_IBO;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		glm::mat4 m_Proj, m_View;
	public:
		TestTexture2D();
		~TestTexture2D();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}