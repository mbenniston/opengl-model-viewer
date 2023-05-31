#include "ssao_pass.h"
#include <array>
#include "../../constants.h"

SsaoPass::SsaoPass(int width, int height) :
	m_deferredShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/ssao/ssao.vert",
		RESOURCES_PATH + "shaders/ssao/ssao.frag")),
	m_screenQuad(2.0f, 2.0f)
{
	m_uniformLocations.geometrySampler = m_deferredShader.getUniformLocation("u_geometry");
	m_uniformLocations.normalSampler = m_deferredShader.getUniformLocation("u_normal");

	m_uniformLocations.projection = m_deferredShader.getUniformLocation("u_projection");
	m_uniformLocations.samples = m_deferredShader.getUniformLocation("u_samples");
	m_uniformLocations.distance = m_deferredShader.getUniformLocation("u_distance");
	m_uniformLocations.bias = m_deferredShader.getUniformLocation("u_bias");

	glSamplerParameteri(m_bufferSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	createFramebuffer(width, height);
}

void SsaoPass::render(const Texture& geometry, const Texture& normal,
	const glm::mat4& projection, int samples, float distance, float bias)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, geometry);
	glBindSampler(0, m_bufferSampler);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	glBindSampler(1, m_bufferSampler);

	m_deferredShader.bind();
	m_deferredShader.loadUniform(m_uniformLocations.geometrySampler, 0);
	m_deferredShader.loadUniform(m_uniformLocations.normalSampler, 1);

	m_deferredShader.loadUniform(m_uniformLocations.projection, projection);
	m_deferredShader.loadUniform(m_uniformLocations.samples, samples);
	m_deferredShader.loadUniform(m_uniformLocations.distance, distance);
	m_deferredShader.loadUniform(m_uniformLocations.bias, bias);

	m_screenQuad.draw();
	m_deferredShader.unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(1, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SsaoPass::createFramebuffer(int width, int height)
{
	m_framebufferWidth = width;
	m_framebufferHeight = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glBindTexture(GL_TEXTURE_2D, m_occlusionBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_occlusionBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

    std::array<GLenum, 1> drawBuffers = {
        GL_COLOR_ATTACHMENT0,
    };

	glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
