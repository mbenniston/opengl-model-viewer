#include "sampler.h"

template class Handle<GLuint, SamplerWrapper::CreateSampler, SamplerWrapper::DeleteSampler>;

namespace SamplerWrapper
{
	GLuint CreateSampler()
	{
		GLuint v;
		glGenSamplers(1, &v);
		return v;
	}

	void DeleteSampler(GLuint vb)
	{
		glDeleteSamplers(1, &vb);
	}
}

