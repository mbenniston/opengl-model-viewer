#pragma once
#include "../../wrapper/framebuffer.h"
#include "../../wrapper/render_buffer.h"
#include "../../wrapper/shader.h"
#include "../../wrapper/scene.h"
#include "../../wrapper/quad.h"
#include "../../wrapper/sampler.h"

class GoochPass
{
public:
	GoochPass(int width, int height);

	void render(
		const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, 
		const Scene& scene, glm::vec3 direction, float blue, float yellow, float alpha, float beta, 
			int scale, float depthThreshold, float normalThreshold, 
			float depthNormalThreshold,	
			float depthNormalThresholdScale);

	const Texture& albedo() const { return m_albedoBuffer; }

	void createFramebuffer(int width, int height);
private:
	int m_framebufferWidth, m_framebufferHeight;

	void renderNode(const Node& node, const glm::mat4& parent_transform);

	Framebuffer m_framebuffer;
	Texture m_albedoBuffer;
	Texture m_geometryBuffer;
	Texture m_normalBuffer;
	Texture m_temperatureBuffer;
	RenderBuffer m_depthBuffer;
	Sampler m_bufferSampler;

	Shader m_deferredShader;
	
	struct {
		GLint projectionMatrix;
		GLint viewMatrix;
		GLint modelMatrix;
		GLint baseSampler;
		GLint baseFactor;
		GLint blue;
		GLint yellow;
		GLint alpha;
		GLint beta;
	} m_shadingUniformLocations;

	Shader m_lightingShader;

	struct {
		GLint albedoSampler;
		GLint geometrySampler;
		GLint normalSampler;
		GLint temperatureSampler;
		GLint depthSampler;
		GLint lightDirection;

		GLint scale;
		GLint depthThreshold;
		GLint normalThreshold;
		GLint depthNormalThreshold;
		GLint depthNormalThresholdScale;
	} m_lightingUniformLocations;

	Quad m_screenQuad;
};