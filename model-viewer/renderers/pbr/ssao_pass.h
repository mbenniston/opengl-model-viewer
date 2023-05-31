#pragma once
#include "../../wrapper/framebuffer.h"
#include "../../wrapper/render_buffer.h"
#include "../../wrapper/texture.h"
#include "../../wrapper/quad.h"
#include "../../wrapper/shader.h"
#include "../../wrapper/sampler.h"

class SsaoPass
{
public:
	SsaoPass(int width, int height);

	const Texture& occlusion() const { return m_occlusionBuffer; }

	void render(const Texture& geometry, const Texture& normal,
		const glm::mat4& projection, int samples, float distance, float bias);

	void createFramebuffer(int width, int height);
private:
	int m_framebufferWidth, m_framebufferHeight;

	Framebuffer m_framebuffer;
	Texture m_occlusionBuffer;
	RenderBuffer m_depthBuffer;
	Sampler m_bufferSampler;
	Quad m_screenQuad;

	Shader m_deferredShader;
	struct {
		GLint geometrySampler;
		GLint normalSampler;
		GLint projection;
		GLint samples;
		GLint distance;
		GLint bias;
	} m_uniformLocations;
};