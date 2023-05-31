#pragma once
#include <glad.h>
#include "handle.h"

namespace FramebufferWrapper
{
	GLuint CreateFramebuffer();
	void DestroyFramebuffer(GLuint id);
}

extern template class Handle<GLuint, FramebufferWrapper::CreateFramebuffer, FramebufferWrapper::DestroyFramebuffer>;
class Framebuffer : public Handle<GLuint, FramebufferWrapper::CreateFramebuffer, FramebufferWrapper::DestroyFramebuffer> {};