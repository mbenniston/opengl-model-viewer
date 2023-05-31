#include "blur_pass.h"
#include <array>
#include "../../constants.h"

BlurPass::BlurPass(int width, int height, float downscale, bool meanBlur) :
	m_screenQuad(2.0f, 2.0f),
	m_downscale(downscale),
	m_deferredShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/blur/blur.vert", 
		meanBlur ? RESOURCES_PATH + "shaders/blur/blur_mean.frag" : RESOURCES_PATH + "shaders/blur/blur.frag"))
{
	m_uniformLocations.inputSampler = m_deferredShader.getUniformLocation("u_input");
	m_uniformLocations.stdDev = m_deferredShader.getUniformLocation("u_stdDev");
	m_uniformLocations.kernelSize = m_deferredShader.getUniformLocation("u_kernelSize");
	m_uniformLocations.downscale = m_deferredShader.getUniformLocation("u_downscale");

	glSamplerParameteri(m_bufferSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	createFramebuffer(width, height);
}

void BlurPass::blur(const Texture& texture, int kernelSize, float stdDev)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, m_width, m_height);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, m_bufferSampler);

	m_deferredShader.bind();
	m_deferredShader.loadUniform(m_uniformLocations.inputSampler, 0);
	m_deferredShader.loadUniform(m_uniformLocations.kernelSize, kernelSize);
	m_deferredShader.loadUniform(m_uniformLocations.stdDev, stdDev);
	m_deferredShader.loadUniform(m_uniformLocations.downscale, m_downscale);
	m_screenQuad.draw();
	m_deferredShader.unbind();
	 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlurPass::createFramebuffer(int width, int height)
{
	m_width = static_cast<int>(width / m_downscale);
	m_height = static_cast<int>(height / m_downscale);

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

    std::array<GLenum, 1> drawBuffers = {
        GL_COLOR_ATTACHMENT0,
    };

    glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
