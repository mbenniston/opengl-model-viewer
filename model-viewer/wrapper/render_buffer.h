#pragma once
#include <glad.h>
#include "handle.h"

namespace RenderBufferWrapper
{
	GLuint CreateRenderBuffer();
	void DeleteRenderBuffer(GLuint vb);
}

extern template class Handle<GLuint, RenderBufferWrapper::CreateRenderBuffer, RenderBufferWrapper::DeleteRenderBuffer>;
class RenderBuffer : public Handle<GLuint, RenderBufferWrapper::CreateRenderBuffer, RenderBufferWrapper::DeleteRenderBuffer> {};
