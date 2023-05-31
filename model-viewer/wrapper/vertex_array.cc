#include "vertex_array.h"

namespace VertexArrayWrapper
{
	GLuint CreateVertexArray()
	{
		GLuint v;
		glGenVertexArrays(1, &v);
		return v;
	}

	void DeleteVertexArray(GLuint vb)
	{
		glDeleteVertexArrays(1, &vb);
	}
}

template class Handle<GLuint, VertexArrayWrapper::CreateVertexArray, VertexArrayWrapper::DeleteVertexArray>;
