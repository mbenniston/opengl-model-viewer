#pragma once
#include <glad.h>
#include "handle.h"

namespace SamplerWrapper
{
	GLuint CreateSampler();
	void DeleteSampler(GLuint vb);
}

extern template class Handle<GLuint, SamplerWrapper::CreateSampler, SamplerWrapper::DeleteSampler>;
class Sampler : public Handle<GLuint, SamplerWrapper::CreateSampler, SamplerWrapper::DeleteSampler> {};
