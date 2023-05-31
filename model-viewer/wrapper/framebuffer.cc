#include "framebuffer.h"

template class Handle<GLuint, FramebufferWrapper::CreateFramebuffer, FramebufferWrapper::DestroyFramebuffer>;

namespace FramebufferWrapper
{
	GLuint CreateFramebuffer()
	{
		GLuint id;
		glGenFramebuffers(1, &id);
		return id;
	}

	void DestroyFramebuffer(GLuint id)
	{
		glDeleteFramebuffers(1, &id);
	}
}