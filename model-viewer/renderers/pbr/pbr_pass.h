#pragma once
#include <glad.h>
#include "../../wrapper/quad.h"
#include "deferred_pass.h"

class PbrPass
{
public:
	PbrPass(int width, int height);
	void render(const DeferredPass& deferredRenderer, 
		const Texture& ambientOcclusion, 
		const Texture& irrandianceCubemap, 
		const Texture& prefilteredCubemap, 
		const Texture& brdfLut, 
		glm::vec3 lightDir, glm::vec3 lightRadiance, 
		const glm::mat4& viewMatrix);

	const Texture& color() const { return m_colorBuffer; }

	void createFramebuffer(int width, int height);
private:
	int m_framebufferWidth, m_framebufferHeight;

	Framebuffer m_framebuffer;
	Texture m_colorBuffer;
	RenderBuffer m_depthBuffer;

	Quad m_screenQuad;
	Shader m_deferredShader;
	Sampler m_bufferSampler;

	struct {
		GLint albedoSampler;
		GLint geometrySampler;
		GLint normalSampler;
		GLint roughMetalSampler;
		GLint lightDirectionViewSpace;
		GLint lightRadiance;
		GLint emissiveSampler;
		GLint irradianceEnvironmentSampler;
	} m_uniformLocations;
};