#include "pbr_pass.h"
#include <array>
#include "../../constants.h"

PbrPass::PbrPass(int width, int height) :
	m_deferredShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/pbr/pbr.vert",
		RESOURCES_PATH + "shaders/pbr/pbr.frag")),
	m_screenQuad(2.0f, 2.0f)											  
{
	m_uniformLocations.albedoSampler = m_deferredShader.getUniformLocation("u_albedo");
	m_uniformLocations.geometrySampler = m_deferredShader.getUniformLocation("u_geometry");
	m_uniformLocations.normalSampler = m_deferredShader.getUniformLocation("u_normal");
	m_uniformLocations.roughMetalSampler = m_deferredShader.getUniformLocation("u_roughMetal");
	m_uniformLocations.emissiveSampler = m_deferredShader.getUniformLocation("u_emissive");
	m_uniformLocations.lightDirectionViewSpace = m_deferredShader.getUniformLocation("u_lightDirectionViewSpace");
	m_uniformLocations.lightRadiance = m_deferredShader.getUniformLocation("u_lightRadiance");
	m_uniformLocations.irradianceEnvironmentSampler = m_deferredShader.getUniformLocation("u_irradianceEnvironmentMap");

	glSamplerParameteri(m_bufferSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	createFramebuffer(width, height);
}

void PbrPass::render(const DeferredPass& deferredRenderer, const Texture& ambientOcclusion, const Texture& irrandianceCubemap, const Texture& prefilteredCubemap, const Texture& brdfLut, glm::vec3 lightDir, glm::vec3 lightRadiance, const glm::mat4& viewMatrix)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferredRenderer.albedo());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferredRenderer.geometry());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferredRenderer.normal());

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, deferredRenderer.roughMetal());
	
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, deferredRenderer.emissive());

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, ambientOcclusion);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irrandianceCubemap);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredCubemap);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, brdfLut);

	glBindSampler(0, m_bufferSampler);
	glBindSampler(1, m_bufferSampler);
	glBindSampler(2, m_bufferSampler);
	glBindSampler(3, m_bufferSampler);
	glBindSampler(4, m_bufferSampler);

	m_deferredShader.bind();
	m_deferredShader.loadUniform(m_uniformLocations.albedoSampler, 0);
	m_deferredShader.loadUniform(m_uniformLocations.geometrySampler, 1);
	m_deferredShader.loadUniform(m_uniformLocations.normalSampler, 2);
	m_deferredShader.loadUniform(m_uniformLocations.roughMetalSampler, 3);
	m_deferredShader.loadUniform(m_uniformLocations.emissiveSampler, 4);
	m_deferredShader.loadUniform(m_uniformLocations.lightDirectionViewSpace, lightDir);
	m_deferredShader.loadUniform(m_uniformLocations.lightRadiance, lightRadiance);
	m_deferredShader.loadUniform(m_deferredShader.getUniformLocation("u_ambientOcculusion"), 5);
	m_deferredShader.loadUniform(m_uniformLocations.irradianceEnvironmentSampler, 6);
	m_deferredShader.loadUniform(m_deferredShader.getUniformLocation("u_inverseViewNormal"), glm::transpose(glm::inverse(glm::mat3(glm::inverse(viewMatrix)))));
	m_deferredShader.loadUniform(m_deferredShader.getUniformLocation("u_prefilteredEnvironmentMap"), 7);
	m_deferredShader.loadUniform(m_deferredShader.getUniformLocation("u_brdfLut"), 8);
	
	m_screenQuad.draw();
	m_deferredShader.unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindSampler(0, 0);
	glBindSampler(1, 0);
	glBindSampler(2, 0);
	glBindSampler(3, 0);
	glBindSampler(4, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PbrPass::createFramebuffer(int width, int height)
{
	m_framebufferWidth = width;
	m_framebufferHeight = height;

	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

	std::array<GLenum, 1> drawBuffers = {
		GL_COLOR_ATTACHMENT0,
	};

	glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
