#include "bloom_pass.h"
#include "../../constants.h"

BloomPass::BloomPass() :
	m_screenQuad(2.0f, 2.0f),
	m_deferredShader(Shader::CreateFromPath(
		RESOURCES_PATH + "shaders/bloom/bloom.vert",
		RESOURCES_PATH + "shaders/bloom/bloom.frag"))
{
	m_uniformLocations.sourceSampler = m_deferredShader.getUniformLocation("u_source");
	m_uniformLocations.bloomSampler = m_deferredShader.getUniformLocation("u_bloom");
	m_uniformLocations.lensDirtSampler = m_deferredShader.getUniformLocation("u_lensDirt");

	m_uniformLocations.bloomFactor = m_deferredShader.getUniformLocation("u_bloomFactor");
	m_uniformLocations.lensDirtIntensity = m_deferredShader.getUniformLocation("u_lensDirtIntensity");

	glSamplerParameteri(m_bufferSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void BloomPass::render(const Texture& source, const Texture& bloom, const Texture& lensDirt, float bloomFactor, float lensDirtIntensity)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, source);
	glBindSampler(0, m_bufferSampler);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloom);
	glBindSampler(1, m_bufferSampler);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lensDirt);
	glBindSampler(2, m_bufferSampler);

	m_deferredShader.bind();
	m_deferredShader.loadUniform(m_uniformLocations.sourceSampler, 0);
	m_deferredShader.loadUniform(m_uniformLocations.bloomSampler, 1);
	m_deferredShader.loadUniform(m_uniformLocations.lensDirtSampler, 2);
	m_deferredShader.loadUniform(m_uniformLocations.bloomFactor, bloomFactor);
	m_deferredShader.loadUniform(m_uniformLocations.lensDirtIntensity, lensDirtIntensity);
	m_screenQuad.draw();
	m_deferredShader.unbind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(1, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(2, 0);
}
