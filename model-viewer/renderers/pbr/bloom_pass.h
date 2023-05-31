#pragma once
#include "../../wrapper/quad.h"
#include "../../wrapper/texture.h"
#include "../../wrapper/shader.h"
#include "../../wrapper/sampler.h"

class BloomPass
{
public:
	BloomPass();

	void render(const Texture& source, const Texture& bloom, const Texture& lensDirt, float bloomFactor, float lensDirtIntensity);

private:
	Sampler m_bufferSampler;

	Quad m_screenQuad;
	Shader m_deferredShader;
	struct {
		GLint sourceSampler;
		GLint bloomSampler;
		GLint lensDirtSampler;
		GLint bloomFactor;
		GLint lensDirtIntensity;
	} m_uniformLocations;
};