#pragma once
#include <glad.h>
#include "handle.h"

namespace VertexArrayWrapper
{
	GLuint CreateVertexArray();
	void DeleteVertexArray(GLuint vb);
}

extern template class Handle<GLuint, VertexArrayWrapper::CreateVertexArray, VertexArrayWrapper::DeleteVertexArray>;
class VertexArray : public Handle<GLuint, VertexArrayWrapper::CreateVertexArray, VertexArrayWrapper::DeleteVertexArray> {};
