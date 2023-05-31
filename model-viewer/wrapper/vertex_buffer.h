#pragma once
#include <glad.h>
#include "handle.h"

namespace VertexBufferWrapper
{
	GLuint CreateVertexBuffer();
	void DeleteVertexBuffer(GLuint vb);
}

extern template class Handle<GLuint, VertexBufferWrapper::CreateVertexBuffer, VertexBufferWrapper::DeleteVertexBuffer>;
class VertexBuffer : public Handle<GLuint, VertexBufferWrapper::CreateVertexBuffer, VertexBufferWrapper::DeleteVertexBuffer> {};
