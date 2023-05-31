#pragma once
#include "../../wrapper/framebuffer.h"
#include "../../wrapper/render_buffer.h"
#include "../../wrapper/shader.h"
#include "../../wrapper/scene.h"
#include "../../wrapper/cube.h"

class DeferredPass
{
public:
	DeferredPass(int width, int height);

	void start(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
	void render(const Scene& scene);
	void stop();
	void drawSkybox(const Texture& diffuseCubemap, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

	const Texture& albedo() const { return m_albedoBuffer; }
	const Texture& geometry() const { return m_geometryBuffer; }
	const Texture& normal() const { return m_normalBuffer; }
	const Texture& roughMetal() const { return m_roughMetalBuffer; }
	const Texture& emissive() const { return m_emissiveBuffer; }
	const Framebuffer& framebuffer() const { return m_framebuffer; }

	void createFramebuffer(int width, int height);
private:
	int m_framebufferWidth, m_framebufferHeight;

	void renderNode(const Node& node, const glm::mat4& parentTransform, Shader& shader);

	Framebuffer m_framebuffer;
	Texture m_albedoBuffer;
	Texture m_geometryBuffer;
	Texture m_normalBuffer;
	Texture m_emissiveBuffer;
	Texture m_roughMetalBuffer;
	RenderBuffer m_depthBuffer;

	Texture m_whiteTexture;
	Texture m_blackTexture;

	Shader m_deferredShader;
	struct {
		GLint projectionMatrix;
		GLint viewMatrix;
		GLint modelMatrix;
		GLint baseSampler;
		GLint roughMetalSampler;
		GLint emissiveSampler;
		GLint occlusionSampler;
		GLint baseFactor;
		GLint roughFactor;
		GLint metalFactor;
		GLint emissiveFactor;
	} m_uniformLocations;

	Shader m_skyboxShader;
	struct {
		GLint mvpMatrix;
		GLint diffuseSampler;
	} m_skyboxUniformLocations;
	
	Cube m_cube;
};