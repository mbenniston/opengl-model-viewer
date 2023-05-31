#include "render_buffer.h"

template class Handle<GLuint, RenderBufferWrapper::CreateRenderBuffer, RenderBufferWrapper::DeleteRenderBuffer>;

namespace RenderBufferWrapper
{
	GLuint CreateRenderBuffer()
	{
		GLuint v;
		glGenRenderbuffers(1, &v);
		return v;
	}

	void DeleteRenderBuffer(GLuint vb)
	{
		glDeleteRenderbuffers(1, &vb);
	}
}