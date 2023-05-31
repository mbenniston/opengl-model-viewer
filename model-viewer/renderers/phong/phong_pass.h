#pragma once
#include "../../wrapper/shader.h"
#include "../../wrapper/scene.h"
#include "../../wrapper/quad.h"
#include "../../wrapper/sampler.h"
#include "../../wrapper/cube.h"

class PhongPass
{
public:
	PhongPass(int width, int height);

	void render(
		const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix,
		const Scene& scene, glm::vec3 lightDirection, glm::vec3 lightColor,
		glm::vec3 ambient, float specularPower, float specularStrength);

	void drawSkybox(const Texture& diffuseCubemap, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

	void createFramebuffer(int width, int height);
private:
	int m_framebufferWidth, m_framebufferHeight;

	void renderNode(const Node& node, const glm::mat4& parentTransform);

	Shader m_phongShader;
	struct {
		GLint projectionMatrix;
		GLint viewMatrix;
		GLint modelMatrix;
		GLint baseSampler;
		GLint emissiveSampler;
		GLint baseFactor;
		GLint emissiveFactor;
		GLint lightDirection;
		GLint lightColor;
		GLint ambient;
		GLint specularPower;
		GLint specularStrength;
	} m_uniformLocations;

	Shader m_skyboxShader;
	struct {
		GLint mvpMatrix;
		GLint diffuseSampler;
	} m_skyboxUniformLocations;

	Cube m_cube;
	Texture m_whiteTexture;
	Texture m_blackTexture;
};