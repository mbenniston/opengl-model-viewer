#pragma once
#include "vertex_array.h"
#include "vertex_buffer.h"

class Cube
{
public:
	Cube();

	void draw();
	void bind();
	void drawUnbound();
	void unbind();

private:
	
	VertexArray m_vertexArray;
	VertexBuffer m_vertexBuffer;
};