#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"


class Renderer
{
private:

public:
	void Clear() const;
	void Draw(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
};