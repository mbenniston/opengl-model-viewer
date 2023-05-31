#include "vertex_buffer.h"

namespace VertexBufferWrapper
{
	GLuint CreateVertexBuffer()
	{
		GLuint v;
		glGenBuffers(1, &v);
		return v;
	}

	void DeleteVertexBuffer(GLuint vb)
	{
		glDeleteBuffers(1, &vb);
	}
}

template class Handle<GLuint, VertexBufferWrapper::CreateVertexBuffer, VertexBufferWrapper::DeleteVertexBuffer>;



