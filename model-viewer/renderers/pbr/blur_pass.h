#pragma once
#include "../../wrapper/framebuffer.h"
#include "../../wrapper/quad.h"
#include "../../wrapper/render_buffer.h"
#include "../../wrapper/texture.h"
#include "../../wrapper/shader.h"
#include "../../wrapper/sampler.h"

class BlurPass
{
public:
	BlurPass(int width, int height, float downscale, bool meanBlur=false);

	void blur(const Texture& texture, int kernelSize, float stdDev);

	const Texture& blurred() const { return m_colorBuffer; }

	void createFramebuffer(int width, int height);
private:
	int m_width;
	int m_height;
	float m_downscale;

	Framebuffer m_framebuffer;
	Texture m_colorBuffer;
	RenderBuffer m_depthBuffer;
	Sampler m_bufferSampler;

	Quad m_screenQuad;
	Shader m_deferredShader;
	struct {
		GLint inputSampler;
		GLint kernelSize;
		GLint stdDev;
		GLint downscale;
	} m_uniformLocations;
};